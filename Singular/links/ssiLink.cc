/****************************************
 * Computer Algebra System SINGULAR     *
 ****************************************/
/***************************************************************
 * File:    ssiLink.h
 *  Purpose: declaration of sl_link routines for ssi
 ***************************************************************/
#define TRANSEXT_PRIVATES 1 /* allow access to transext internals */

#include "kernel/mod2.h"

#include "misc/intvec.h"
#include "misc/options.h"

#include "reporter/si_signals.h"
#include "reporter/s_buff.h"
#include "reporter/si_signals.h"

#include "coeffs/bigintmat.h"
#include "coeffs/longrat.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"
#include "polys/ext_fields/transext.h"
#include "polys/simpleideals.h"
#include "polys/matpol.h"

#include "kernel/oswrapper/timer.h"
#include "kernel/oswrapper/timer.h"
#include "kernel/oswrapper/feread.h"
#include "kernel/oswrapper/rlimit.h"

#include "Singular/tok.h"
#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/subexpr.h"
#include "Singular/links/silink.h"
#include "Singular/cntrlc.h"
#include "Singular/feOpt.h"
#include "Singular/lists.h"
#include "Singular/blackbox.h"
#include "Singular/links/ssiLink.h"

#ifdef HAVE_SIMPLEIPC
#include "Singular/links/simpleipc.h"
#endif

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>          /* for portability */
#include <ctype.h>   /*for isdigit*/
#include <netdb.h>
#include <netinet/in.h> /* for htons etc.*/


#define SSI_VERSION 16
// 5->6: changed newstruct representation
// 6->7: attributes
// 7->8: qring
// 8->9: module: added rank
// 9->10: tokens in grammar.h/tok.h reorganized
// 10->11: extended ring descr. for named coeffs (not in used until 4.1)
// 11->12: add rank to ideal/module, add smatrix
// 12->13: NC rings
// 13->14: ring references
// 14->15: bigintvec, prune_map, mres_map
// 15->16: schema subversion table

#define SSI_SCHEMA_TOKEN 25
#define SSI_SCHEMA_TABLE_VERSION 1

enum ssiSchemaId
{
  SSI_SCHEMA_NUMBER=1,
  SSI_SCHEMA_RING,
  SSI_SCHEMA_POLY,
  SSI_SCHEMA_IDEAL,
  SSI_SCHEMA_MATRIX,
  SSI_SCHEMA_MODULE,
  SSI_SCHEMA_LIST,
  SSI_SCHEMA_PROC,
  SSI_SCHEMA_INTVEC,
  SSI_SCHEMA_BIGINTMAT,
  SSI_SCHEMA_ATTRIBUTES,
  SSI_SCHEMA_COMMAND,
  SSI_SCHEMA_RING_PROPERTIES,
  SSI_SCHEMA_BLACKBOX
};

struct ssiSchemaVersionEntry
{
  int id;
  int version;
  const char *name;
};

static const ssiSchemaVersionEntry ssiSchemaVersions[] =
{
  { SSI_SCHEMA_NUMBER,          1, "number" },
  { SSI_SCHEMA_RING,            1, "ring" },
  { SSI_SCHEMA_POLY,            1, "polynomial" },
  { SSI_SCHEMA_IDEAL,           1, "ideal" },
  { SSI_SCHEMA_MATRIX,          1, "matrix" },
  { SSI_SCHEMA_MODULE,          1, "module" },
  { SSI_SCHEMA_LIST,            1, "list" },
  { SSI_SCHEMA_PROC,            1, "proc" },
  { SSI_SCHEMA_INTVEC,          1, "intvec" },
  { SSI_SCHEMA_BIGINTMAT,       1, "bigintmat" },
  { SSI_SCHEMA_ATTRIBUTES,      1, "attributes" },
  { SSI_SCHEMA_COMMAND,         1, "command" },
  { SSI_SCHEMA_RING_PROPERTIES, 1, "ring-properties" },
  { SSI_SCHEMA_BLACKBOX,        1, "blackbox" }
};

static const int ssiSchemaVersionCount =
  (int)(sizeof(ssiSchemaVersions)/sizeof(ssiSchemaVersions[0]));

static int ssiCurrentSchemaVersion(int id)
{
  for (int i=0; i<ssiSchemaVersionCount; i++)
    if (ssiSchemaVersions[i].id==id) return ssiSchemaVersions[i].version;
  return 0;
}

static const char *ssiSchemaName(int id)
{
  for (int i=0; i<ssiSchemaVersionCount; i++)
    if (ssiSchemaVersions[i].id==id) return ssiSchemaVersions[i].name;
  return "unknown";
}

static void ssiInitSchemaVersions(ssiInfo *d)
{
  if (d==NULL) return;
  memset(d->schema_versions, 0, sizeof(d->schema_versions));
  for (int i=0; i<ssiSchemaVersionCount; i++)
  {
    int id=ssiSchemaVersions[i].id;
    if ((id>0) && (id<SSI_SCHEMA_VERSION_COUNT))
      d->schema_versions[id]=(unsigned short)ssiSchemaVersions[i].version;
  }
}

static int ssiSchemaVersion(const ssiInfo *d, int id)
{
  if ((d!=NULL) && (id>0) && (id<SSI_SCHEMA_VERSION_COUNT)
  && (d->schema_versions[id]!=0))
    return d->schema_versions[id];
  return ssiCurrentSchemaVersion(id);
}

static void ssiSetSchemaVersion(ssiInfo *d, int id, int version)
{
  if ((d!=NULL) && (id>0) && (id<SSI_SCHEMA_VERSION_COUNT) && (version>0))
    d->schema_versions[id]=(unsigned short)version;
}

static BOOLEAN ssiRequireSchemaVersion(const ssiInfo *d, int id,
                                       const char *format)
{
  int have=ssiSchemaVersion(d, id);
  int current=ssiCurrentSchemaVersion(id);
  if ((current>0) && (have>current))
  {
    Werror("%s: %s schema version %d is newer than supported version %d",
           format, ssiSchemaName(id), have, current);
    return TRUE;
  }
  return FALSE;
}

EXTERN_VAR BOOLEAN FE_OPT_NO_SHELL_FLAG;
VAR link_list ssiToBeClosed=NULL;
VAR volatile BOOLEAN ssiToBeClosed_inactive=TRUE;

// forward declarations:
static void ssiWritePoly_R(const ssiInfo *d, poly p, const ring r);
static void ssiWritePoly_R_S(poly p, const ring r);
static void ssiWriteIdeal_R(const ssiInfo *d, int typ,const ideal I, const ring r);
static poly ssiReadPoly_R(const ssiInfo *D, const ring r);
static poly ssiReadPoly_R_S(char **s, const ring r);
static ideal ssiReadIdeal_R(const ssiInfo *d,const ring r);

// the helper functions:
static BOOLEAN ssiSetCurrRing(const ring r) /* returned: not accepted */
{
  //  if (currRing!=NULL)
  //  Print("need to change the ring, currRing:%s, switch to: ssiRing%d\n",IDID(currRingHdl),nr);
  //  else
  //  Print("no ring, switch to ssiRing%d\n",nr);
  if (r==currRing)
  {
    rIncRefCnt(r);
    currRingHdl=rFindHdl(r,currRingHdl);
    return TRUE;
  }
  else if ((currRing==NULL) || (!rEqual(r,currRing,1)))
  {
    char name[20];
    int nr=0;
    idhdl h=NULL;
    loop
    {
      snprintf(name,20,"ssiRing%d",nr); nr++;
      h=IDROOT->get(name, 0);
      if (h==NULL)
      {
        h=enterid(name,0,RING_CMD,&IDROOT,FALSE);
        IDRING(h)=rIncRefCnt(r);
        r->ref=2;/*ref==2: d->r and h */
        break;
      }
      else if ((IDTYP(h)==RING_CMD)
      && (rEqual(r,IDRING(h),1)))
      {
        rIncRefCnt(IDRING(h));
        break;
      }
    }
    rSetHdl(h);
    return FALSE;
  }
  else
  {
    rKill(r);
    rIncRefCnt(currRing);
    return TRUE;
  }
}
static void ssiCheckCurrRing(const ring r)
{
  if ((r!=currRing)
  ||(currRingHdl==NULL)
  ||(IDRING(currRingHdl)!=r))
  {
    char name[20];
    int nr=0;
    idhdl h=NULL;
    loop
    {
      snprintf(name,20,"ssiRing%d",nr); nr++;
      h=IDROOT->get(name, 0);
      if (h==NULL)
      {
        h=enterid(name,0,RING_CMD,&IDROOT,FALSE);
        IDRING(h)=rIncRefCnt(r);
        r->ref=2;/*ref==2: d->r and h */
        break;
      }
      else if ((IDTYP(h)==RING_CMD)
      && (rEqual(r,IDRING(h),1)))
      {
        break;
      }
    }
    rSetHdl(h);
  }
  assume((currRing==r) || rEqual(r,currRing));
}
// the implementation of the functions:
void ssiWriteInt(const ssiInfo *d,const int i)
{
  fprintf(d->f_write,"%d ",i);
}
void ssiWriteInt_S(const int i)
{
  StringAppend("%d ",i);
}

static void ssiWriteString(const ssiInfo *d,const char *s)
{
  fprintf(d->f_write,"%d %s ",(int)strlen(s),s);
}
static void ssiWriteString_S(const char *s)
{
  StringAppend("%d %s ",(int)strlen(s),s);
}

static void ssiWriteSchemaTable(const ssiInfo *d)
{
  fprintf(d->f_write, "%d %d %d ",
          SSI_SCHEMA_TOKEN, SSI_SCHEMA_TABLE_VERSION, ssiSchemaVersionCount);
  for (int i=0; i<ssiSchemaVersionCount; i++)
  {
    int id=ssiSchemaVersions[i].id;
    fprintf(d->f_write, "%d %d ", id, ssiSchemaVersion(d, id));
  }
  fputc('\n', d->f_write);
}

static void ssiReadSchemaTable(ssiInfo *d)
{
  int table_version=s_readint(d->f_read);
  int count=s_readint(d->f_read);
  if (table_version>SSI_SCHEMA_TABLE_VERSION)
  {
    Print("ssi: schema table version %d is newer than supported version %d\n",
          table_version, SSI_SCHEMA_TABLE_VERSION);
  }
  for (int i=0; i<count; i++)
  {
    int id=s_readint(d->f_read);
    int version=s_readint(d->f_read);
    ssiSetSchemaVersion(d, id, version);
  }
}

static void ssiWriteBigInt(const ssiInfo *d, const number n)
{
  n_WriteFd(n,d,coeffs_BIGINT);
}
static void ssiWriteBigInt_S(const number n)
{
  n_WriteFd_S(n,coeffs_BIGINT);
}

static void ssiWriteNumber_CF(const ssiInfo *d, const number n, const coeffs cf)
{
  // syntax is as follows:
  // case 1 Z/p:   3 <int>
  // case 2 Q:     3 4 <int>
  //        or     3 0 <mpz_t nominator> <mpz_t denominator>
  //        or     3 1  dto.
  //        or     3 3 <mpz_t nominator>
  //        or     3 5 <mpz_t raw nom.> <mpz_t raw denom.>
  //        or     3 6 <mpz_t raw nom.> <mpz_t raw denom.>
  //        or     3 8 <mpz_t raw nom.>
  if (getCoeffType(cf)==n_transExt)
  {
    fraction f=(fraction)n;
    ssiWritePoly_R(d,NUM(f),cf->extRing);
    ssiWritePoly_R(d,DEN(f),cf->extRing);
  }
  else if (getCoeffType(cf)==n_algExt)
  {
    ssiWritePoly_R(d,(poly)n,cf->extRing);
  }
  else if (cf->cfWriteFd!=NULL)
  {
    n_WriteFd(n,d,cf);
  }
  else WerrorS("coeff field not implemented");
}

static void ssiWriteNumber_CF_S(const number n, const coeffs cf)
{
  // syntax is as follows:
  // case 1 Z/p:   3 <int>
  // case 2 Q:     3 4 <int>
  //        or     3 0 <mpz_t nominator> <mpz_t denominator>
  //        or     3 1  dto.
  //        or     3 3 <mpz_t nominator>
  //        or     3 5 <mpz_t raw nom.> <mpz_t raw denom.>
  //        or     3 6 <mpz_t raw nom.> <mpz_t raw denom.>
  //        or     3 8 <mpz_t raw nom.>
  if (getCoeffType(cf)==n_transExt)
  {
    fraction f=(fraction)n;
    ssiWritePoly_R_S(NUM(f),cf->extRing);
    ssiWritePoly_R_S(DEN(f),cf->extRing);
  }
  else if (getCoeffType(cf)==n_algExt)
  {
    ssiWritePoly_R_S((poly)n,cf->extRing);
  }
  else if (cf->cfWriteFd_S!=NULL)
  {
    n_WriteFd_S(n,cf);
  }
  else WerrorS("coeff field not implemented");
}

static void ssiWriteNumber(const ssiInfo *d, const number n)
{
  ssiWriteNumber_CF(d,n,d->r->cf);
}

static void ssiWriteRing_R(ssiInfo *d,const ring r)
{
  /* 5 <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... <extRing> <Q-ideal> */
  /* ch=-1: transext, coeff ring follows */
  /* ch=-2: algext, coeff ring and minpoly follows */
  /* ch=-3: cf name follows */
  /* ch=-4: NULL*/
  /* ch=-5: reference <int> */
  /* ch=-6: new reference <int> <ring> */
  if (r!=NULL)
  {
    for(int i=0;i<SI_RING_CACHE;i++)
    {
      if (d->rings[i]==r)
      {
        fprintf(d->f_write,"-5 %d ",i);
        return;
      }
    }
    for(int i=0;i<SI_RING_CACHE;i++)
    {
      if (d->rings[i]==NULL)
      {
        d->rings[i]=rIncRefCnt(r);
        fprintf(d->f_write,"-6 %d ",i);
        break;
      }
    }
    if (rField_is_Q(r) || rField_is_Zp(r))
      fprintf(d->f_write,"%d %d ",n_GetChar(r->cf),r->N);
    else if (rFieldType(r)==n_transExt)
      fprintf(d->f_write,"-1 %d ",r->N);
    else if (rFieldType(r)==n_algExt)
      fprintf(d->f_write,"-2 %d ",r->N);
    else /*dummy*/
    {
      fprintf(d->f_write,"-3 %d ",r->N);
      ssiWriteString(d,nCoeffName(r->cf));
    }

    int i;
    for(i=0;i<r->N;i++)
    {
      fprintf(d->f_write,"%d %s ",(int)strlen(r->names[i]),r->names[i]);
    }
    /* number of orderings:*/
    i=0;
    // remember dummy ring: everything 0:
    if (r->order!=NULL) while (r->order[i]!=0) i++;
    fprintf(d->f_write,"%d ",i);
    /* each ordering block: */
    i=0;
    if (r->order!=NULL) while(r->order[i]!=0)
    {
      fprintf(d->f_write,"%d %d %d ",r->order[i],r->block0[i], r->block1[i]);
      switch(r->order[i])
      {
        case ringorder_a:
        case ringorder_wp:
        case ringorder_Wp:
        case ringorder_ws:
        case ringorder_Ws:
        case ringorder_aa:
        {
          int s=r->block1[i]-r->block0[i]+1; // #vars
          for(int ii=0;ii<s;ii++)
            fprintf(d->f_write,"%d ",r->wvhdl[i][ii]);
        }
        break;
        case ringorder_M:
        {
          int s=r->block1[i]-r->block0[i]+1; // #vars
          for(int ii=0;ii<s*s;ii++)
          {
            fprintf(d->f_write,"%d ",r->wvhdl[i][ii]);
          }
        }
        break;

        case ringorder_a64:
        case ringorder_L:
        case ringorder_IS:
          Werror("ring oder not implemented for ssi:%d",r->order[i]);
          break;

        default: break;
      }
      i++;
    }
    if ((rFieldType(r)==n_transExt)
    || (rFieldType(r)==n_algExt))
    {
      ssiWriteRing_R(d,r->cf->extRing); /* includes alg.ext if rFieldType(r)==n_algExt */
    }
    /* Q-ideal :*/
    if (r->qideal!=NULL)
    {
      ssiWriteIdeal_R(d,IDEAL_CMD,r->qideal,r);
    }
    else
    {
      fputs("0 ",d->f_write/*ideal with 0 entries */);
    }
  }
  else /* dummy ring r==NULL*/
  {
    fputs("0 0 0 0 "/*,r->ch,r->N, blocks, q-ideal*/,d->f_write);
  }
  if (rIsLPRing(r)) // cannot be combined with 23 2
  {
    fprintf(d->f_write,"23 1 %d %d ",SI_LOG2(r->bitmask),r->isLPring);
  }
  else
  {
    unsigned long bm=0;
    int b=0;
    bm=rGetExpSize(bm,b,r->N);
    if (r->bitmask!=bm)
    {
      fprintf(d->f_write,"23 0 %d ",SI_LOG2(r->bitmask));
    }
    if (rIsPluralRing(r))
    {
      fputs("23 2 ",d->f_write);
      ssiWriteIdeal(d,MATRIX_CMD,(ideal)r->GetNC()->C);
      ssiWriteIdeal(d,MATRIX_CMD,(ideal)r->GetNC()->D);
    }
  }
}

static void ssiWriteIdeal_R_S(int typ,const ideal I, const ring R);
static void ssiWriteRing_R_S(ring r)
{
  /* 5 <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... <extRing> <Q-ideal> */
  /* ch=-1: transext, coeff ring follows */
  /* ch=-2: algext, coeff ring and minpoly follows */
  /* ch=-3: cf name follows */
  /* ch=-4: NULL*/
  /* ch=-5: reference <int> */
  /* ch=-6: new reference <int> <ring> */
  if (r!=NULL)
  {
    if (rField_is_Q(r) || rField_is_Zp(r))
      StringAppend("%d %d ",n_GetChar(r->cf),r->N);
    else if (rFieldType(r)==n_transExt)
      StringAppend("-1 %d ",r->N);
    else if (rFieldType(r)==n_algExt)
      StringAppend("-2 %d ",r->N);
    else /*dummy*/
    {
      StringAppend("-3 %d ",r->N);
      ssiWriteString_S(nCoeffName(r->cf));
    }

    int i;
    for(i=0;i<r->N;i++)
    {
      StringAppend("%d %s ",(int)strlen(r->names[i]),r->names[i]);
    }
    /* number of orderings:*/
    i=0;
    // remember dummy ring: everything 0:
    if (r->order!=NULL) while (r->order[i]!=0) i++;
    StringAppend("%d ",i);
    /* each ordering block: */
    i=0;
    if (r->order!=NULL) while(r->order[i]!=0)
    {
      StringAppend("%d %d %d ",r->order[i],r->block0[i], r->block1[i]);
      switch(r->order[i])
      {
        case ringorder_a:
        case ringorder_wp:
        case ringorder_Wp:
        case ringorder_ws:
        case ringorder_Ws:
        case ringorder_aa:
        {
          int s=r->block1[i]-r->block0[i]+1; // #vars
          for(int ii=0;ii<s;ii++)
            StringAppend("%d ",r->wvhdl[i][ii]);
        }
        break;
        case ringorder_M:
        {
          int s=r->block1[i]-r->block0[i]+1; // #vars
          for(int ii=0;ii<s*s;ii++)
          {
            StringAppend("%d ",r->wvhdl[i][ii]);
          }
        }
        break;

        case ringorder_a64:
        case ringorder_L:
        case ringorder_IS:
          Werror("ring oder not implemented for ssi:%d",r->order[i]);
          break;

        default: break;
      }
      i++;
    }
    if ((rFieldType(r)==n_transExt)
    || (rFieldType(r)==n_algExt))
    {
      ssiWriteRing_R_S(r->cf->extRing); /* includes alg.ext if rFieldType(r)==n_algExt */
    }
    /* Q-ideal :*/
    if (r->qideal!=NULL)
    {
      ssiWriteIdeal_R_S(IDEAL_CMD,r->qideal,r);
    }
    else
    {
      StringAppendS("0 "/*ideal with 0 entries */);
    }
  }
  else /* dummy ring r==NULL*/
  {
    StringAppendS("0 0 0 0 "/*,r->ch,r->N, blocks, q-ideal*/);
  }
  if (rIsLPRing(r)) // cannot be combined with 23 2
  {
    StringAppend("23 1 %d %d ",SI_LOG2(r->bitmask),r->isLPring);
  }
  else
  {
    unsigned long bm=0;
    int b=0;
    bm=rGetExpSize(bm,b,r->N);
    if (r->bitmask!=bm)
    {
      StringAppend("23 0 %d ",SI_LOG2(r->bitmask));
    }
    if (rIsPluralRing(r))
    {
      StringAppendS("23 2 ");
      ssiWriteIdeal_R_S(MATRIX_CMD,(ideal)r->GetNC()->C,r);
      ssiWriteIdeal_R_S(MATRIX_CMD,(ideal)r->GetNC()->D,r);
    }
  }
}

static void ssiWriteRing(ssiInfo *d,const ring r)
{
  /* 5 <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... <extRing> <Q-ideal> */
  /* ch=-1: transext, coeff ring follows */
  /* ch=-2: algext, coeff ring and minpoly follows */
  /* ch=-3: cf name follows */
  /* ch=-4: NULL */
  /* ch=-5: reference <int> */
  /* ch=-6: new reference <int> <ring> */
  if ((r==NULL)||(r->cf==NULL))
  {
    fputs("-4 ",d->f_write);
    return;
  }
  if (r==currRing) // see recursive calls for transExt/algExt
  {
    if (d->r!=NULL) rKill(d->r);
    d->r=r;
  }
  if (r!=NULL)
  {
    /*d->*/rIncRefCnt(r);
  }
  ssiWriteRing_R(d,r);
}

char* ssiWriteRing_S(const ring r)
{
  StringSetS("");
  ssiWriteRing_R_S(r);
  return StringEndS();
}
static void ssiWritePoly_R(const ssiInfo *d, poly p, const ring r)
{
  fprintf(d->f_write,"%d ",pLength(p));//number of terms

  while(p!=NULL)
  {
    ssiWriteNumber_CF(d,pGetCoeff(p),r->cf);
    //nWrite(fich,pGetCoeff(p));
    fprintf(d->f_write,"%ld ",p_GetComp(p,r));//component

    for(int j=1;j<=rVar(r);j++)
    {
      fprintf(d->f_write,"%ld ",p_GetExp(p,j,r ));//x^j
    }
    pIter(p);
  }
}
static void ssiWritePoly_R_S(poly p, const ring r)
{
  StringAppend("%d ",pLength(p));//number of terms

  while(p!=NULL)
  {
    ssiWriteNumber_CF_S(pGetCoeff(p),r->cf);
    //nWrite(fich,pGetCoeff(p));
    StringAppend("%ld ",p_GetComp(p,r));//component

    for(int j=1;j<=rVar(r);j++)
    {
      StringAppend("%ld ",p_GetExp(p,j,r ));//x^j
    }
    pIter(p);
  }
}

static void ssiWritePoly(const ssiInfo *d, poly p)
{
  ssiWritePoly_R(d,p,d->r);
}

char* ssiWritePoly_S(poly p, const ring r)
{
  StringSetS("");
  ssiWritePoly_R_S(p,r);
  return StringEndS();
}
static void ssiWriteIdeal_R(const ssiInfo *d, int typ,const ideal I, const ring R)
{
   // syntax: 7 # of elements <poly 1> <poly2>.....(ideal,module,smatrix)
   // syntax: 8 <rows> <cols> <poly 1> <poly2>.....(matrix)
   // syntax
   matrix M=(matrix)I;
   int mn;
   if (typ==MATRIX_CMD)
   {
     mn=MATROWS(M)*MATCOLS(M);
     fprintf(d->f_write,"%d %d ", MATROWS(M),MATCOLS(M));
   }
   else
   {
     mn=IDELEMS(I);
     fprintf(d->f_write,"%d ",IDELEMS(I));
   }

   for(int i=0;i<mn;i++)
   {
     ssiWritePoly_R(d,I->m[i],R);
   }
}
static void ssiWriteIdeal_R_S(int typ,const ideal I, const ring R)
{
   // syntax: 7 # of elements <poly 1> <poly2>.....(ideal,module,smatrix)
   // syntax: 8 <rows> <cols> <poly 1> <poly2>.....(matrix)
   // syntax
   matrix M=(matrix)I;
   int mn;
   if (typ==MATRIX_CMD)
   {
     mn=MATROWS(M)*MATCOLS(M);
     StringAppend("%d %d ", MATROWS(M),MATCOLS(M));
   }
   else
   {
     mn=IDELEMS(I);
     StringAppend("%d ",IDELEMS(I));
   }

   for(int i=0;i<mn;i++)
   {
     ssiWritePoly_R_S(I->m[i],R);
   }
}
void ssiWriteIdeal(const ssiInfo *d, int typ,const ideal I)
{
  ssiWriteIdeal_R(d,typ,I,d->r);
}
char* ssiWriteIdeal_S(const ideal I, const ring R)
{
  StringSetS("");
  ssiWriteIdeal_R_S(IDEAL_CMD,I,R);
  return StringEndS();
}
char* ssiWriteMatrix_S(const matrix M, const ring R)
{
  StringSetS("");
  ssiWriteIdeal_R_S(MATRIX_CMD,(ideal)M,R);
  return StringEndS();
}
char* ssiWriteModule_S(const ideal M, const ring R) /* also for smatrix*/
{
  StringSetS("");
  ssiWriteIdeal_R_S(MODUL_CMD,M,R);
  return StringEndS();
}

static void ssiWriteCommand(si_link l, command D)
{
  ssiInfo *d=(ssiInfo*)l->data;
  // syntax: <num ops> <operation> <op1> <op2> ....
  fprintf(d->f_write,"%d %d ",D->argc,D->op);
  if (D->argc >0) ssiWrite(l, &(D->arg1));
  if (D->argc < 4)
  {
    if (D->argc >1) ssiWrite(l, &(D->arg2));
    if (D->argc >2) ssiWrite(l, &(D->arg3));
  }
}

static void ssiWriteProc(const ssiInfo *d,procinfov p)
{
  if (p->data.s.body==NULL)
    iiGetLibProcBuffer(p);
  if (p->data.s.body!=NULL)
    ssiWriteString(d,p->data.s.body);
  else
    ssiWriteString(d,"");
}
static void ssiWriteProc_S(procinfov p)
{
  if (p->data.s.body==NULL)
    iiGetLibProcBuffer(p);
  if (p->data.s.body!=NULL)
    ssiWriteString_S(p->data.s.body);
  else
    ssiWriteString_S("");
}

static void ssiWriteList(si_link l,lists dd)
{
  ssiInfo *d=(ssiInfo*)l->data;
  int Ll=dd->nr;
  fprintf(d->f_write,"%d ",Ll+1);
  int i;
  for(i=0;i<=Ll;i++)
  {
    ssiWrite(l,&(dd->m[i]));
  }
}
static void ssiWriteList_S(lists dd, const ring R)
{
  int Ll=dd->nr;
  StringAppend("%d ",Ll+1);
  int i;
  for(i=0;i<=Ll;i++)
  {
    ssiWrite_S(&(dd->m[i]),R);
  }
}
static void ssiWriteIntvec(const ssiInfo *d,intvec * v)
{
  fprintf(d->f_write,"%d ",v->length());
  int i;
  for(i=0;i<v->length();i++)
  {
    fprintf(d->f_write,"%d ",(*v)[i]);
  }
}
static void ssiWriteIntvec_S(intvec * v)
{
  StringAppend("%d ",v->length());
  int i;
  for(i=0;i<v->length();i++)
  {
    StringAppend("%d ",(*v)[i]);
  }
}
static void ssiWriteIntmat(const ssiInfo *d,intvec * v)
{
  fprintf(d->f_write,"%d %d ",v->rows(),v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    fprintf(d->f_write,"%d ",(*v)[i]);
  }
}
static void ssiWriteIntmat_S(intvec * v)
{
  StringAppend("%d %d ",v->rows(),v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    StringAppend("%d ",(*v)[i]);
  }
}

static void ssiWriteBigintmat(const ssiInfo *d,bigintmat * v)
{
  fprintf(d->f_write,"%d %d ",v->rows(),v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    ssiWriteBigInt(d,(*v)[i]);
  }
}
static void ssiWriteBigintmat_S(bigintmat * v)
{
  StringAppend("%d %d ",v->rows(),v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    ssiWriteBigInt_S((*v)[i]);
  }
}

static void ssiWriteBigintvec(const ssiInfo *d,bigintmat * v)
{
  fprintf(d->f_write,"%d ",v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    ssiWriteBigInt(d,(*v)[i]);
  }
}
static void ssiWriteBigintvec_S(bigintmat * v)
{
  StringAppend("%d ",v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    ssiWriteBigInt_S((*v)[i]);
  }
}

static char *ssiReadString(const ssiInfo *d)
{
  char *buf;
  int l;
  l=s_readint(d->f_read);
  buf=(char*)omAlloc0(l+1);
  int throwaway =s_getc(d->f_read); /* skip ' '*/
  throwaway=s_readbytes(buf,l,d->f_read);
  //if (throwaway!=l) printf("want %d, got %d bytes\n",l,throwaway);
  buf[l]='\0';
  return buf;
}
static char *ssiReadString_S(char** s)
{
  char *buf;
  int l;
  l=s_readint_S(s);
  buf=(char*)omAlloc0(l+1);
  (*s)++; /* skip ' '*/
  for(int i=0;i<l;i++) { buf[i]=(**s); (*s)++; }
  buf[l]='\0';
  return buf;
}

int ssiReadInt(const ssiInfo *d)
{
  return s_readint(d->f_read);
}
int ssiReadInt_S(char **s)
{
  return s_readint_S(s);
}

static number ssiReadNumber_CF(const ssiInfo *d, const coeffs cf)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_NUMBER, "ssi")) return NULL;
  if (cf->cfReadFd!=ndReadFd)
  {
     return n_ReadFd(d,cf);
  }
  else if (getCoeffType(cf) == n_transExt)
  {
    // poly poly
    fraction f=(fraction)n_Init(1,cf);
    p_Delete(&NUM(f),cf->extRing);
    NUM(f)=ssiReadPoly_R(d,cf->extRing);
    DEN(f)=ssiReadPoly_R(d,cf->extRing);
    return (number)f;
  }
  else if (getCoeffType(cf) == n_algExt)
  {
    // poly
    return (number)ssiReadPoly_R(d,cf->extRing);
  }
  else WerrorS("coeffs not implemented in ssiReadNumber");
  return NULL;
}

static number ssiReadNumber_CF_S(char **s, const coeffs cf)
{
  if (cf->cfReadFd_S!=ndReadFd_S)
  {
     return n_ReadFd_S(s,cf);
  }
  else if (getCoeffType(cf) == n_transExt)
  {
    // poly poly
    fraction f=(fraction)n_Init(1,cf);
    p_Delete(&NUM(f),cf->extRing);
    NUM(f)=ssiReadPoly_R_S(s,cf->extRing);
    DEN(f)=ssiReadPoly_R_S(s,cf->extRing);
    return (number)f;
  }
  else if (getCoeffType(cf) == n_algExt)
  {
    // poly
    return (number)ssiReadPoly_R_S(s,cf->extRing);
  }
  else WerrorS("coeffs not implemented in ssiReadNumber");
  return NULL;
}

static number ssiReadBigInt(const ssiInfo *d)
{
  number n=ssiReadNumber_CF(d,coeffs_BIGINT);
  if (n==NULL) return n;
  if ((SR_HDL(n) & SR_INT)==0)
  {
    if (n->s!=3) Werror("invalid sub type in bigint:%d",n->s);
  }
  return n;
}
static number ssiReadBigInt_S(char**s)
{
  number n=ssiReadNumber_CF_S(s,coeffs_BIGINT);
  if ((SR_HDL(n) & SR_INT)==0)
  {
    if (n->s!=3) Werror("invalid sub type in bigint:%d",n->s);
  }
  return n;
}

static number ssiReadNumber(ssiInfo *d)
{
  return ssiReadNumber_CF(d,d->r->cf);
}

static ring ssiReadRing(ssiInfo *d)
{
/* syntax is <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... <Q-ideal> */
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_RING, "ssi")) return NULL;
  int ch;
  int new_ref=-1;
  ch=s_readint(d->f_read);
  if (ch==-6)
  {
    new_ref=s_readint(d->f_read);
    ch=s_readint(d->f_read);
  }
  if (ch==-5)
  {
    int index=s_readint(d->f_read);
    ring r=d->rings[index];
    rIncRefCnt(r);
    return r;
  }
  if (ch==-4)
    return NULL;
  int N=s_readint(d->f_read);
  char **names;
  coeffs cf=NULL;
  if (ch==-3)
  {
    char *cf_name=ssiReadString(d);
    cf=nFindCoeffByName(cf_name);
    if (cf==NULL)
    {
      Werror("cannot find cf:%s",cf_name);
      omFreeBinAddr(cf_name);
      return NULL;
    }
  }
  if (N!=0)
  {
    names=(char**)omAlloc(N*sizeof(char*));
    for(int i=0;i<N;i++)
    {
      names[i]=ssiReadString(d);
    }
  }
  // read the orderings:
  int num_ord; // number of orderings
  num_ord=s_readint(d->f_read);
  rRingOrder_t *ord=(rRingOrder_t *)omAlloc0((num_ord+1)*sizeof(rRingOrder_t));
  int *block0=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int *block1=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int **wvhdl=(int**)omAlloc0((num_ord+1)*sizeof(int*));
  for(int i=0;i<num_ord;i++)
  {
    ord[i]=(rRingOrder_t)s_readint(d->f_read);
    block0[i]=s_readint(d->f_read);
    block1[i]=s_readint(d->f_read);
    switch(ord[i])
    {
      case ringorder_a:
      case ringorder_wp:
      case ringorder_Wp:
      case ringorder_ws:
      case ringorder_Ws:
      case ringorder_aa:
      {
        int s=block1[i]-block0[i]+1; // #vars
        wvhdl[i]=(int*)omAlloc(s*sizeof(int));
        for(int ii=0;ii<s;ii++)
          wvhdl[i][ii]=s_readint(d->f_read);
      }
      break;
      case ringorder_M:
      {
        int s=block1[i]-block0[i]+1; // #vars
        wvhdl[i]=(int*)omAlloc(s*s*sizeof(int));
        for(int ii=0;ii<s*s;ii++)
        {
          wvhdl[i][ii]=s_readint(d->f_read);
        }
      }
      break;
      case ringorder_a64:
      case ringorder_L:
      case ringorder_IS:
        Werror("ring order not implemented for ssi:%d",ord[i]);
        break;

      default: break;
    }
  }
  if (N==0)
  {
    omFree(ord);
    omFree(block0);
    omFree(block1);
    omFree(wvhdl);
    return NULL;
  }
  else
  {
    ring r=NULL;
    if (ch>=0) /* Q, Z/p */
      r=rDefault(ch,N,names,num_ord,ord,block0,block1,wvhdl);
    else if (ch==-1) /* trans ext. */
    {
      TransExtInfo T;
      T.r=ssiReadRing(d);
      if (T.r==NULL) return NULL;
      cf=nInitChar(n_transExt,&T);
      r=rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
    }
    else if (ch==-2) /* alg ext. */
    {
      TransExtInfo T;
      T.r=ssiReadRing(d); /* includes qideal */
      if (T.r==NULL) return NULL;
      cf=nInitChar(n_algExt,&T);
      r=rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
    }
    else if (ch==-3)
    {
      r=rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
    }
    else
    {
      Werror("ssi: read unknown coeffs type (%d)",ch);
      for(int i=0;i<N;i++)
      {
        omFree(names[i]);
      }
      omFreeSize(names,N*sizeof(char*));
      return NULL;
    }
    ideal q=ssiReadIdeal_R(d,r);
    if (IDELEMS(q)==0) omFreeBin(q,sip_sideal_bin);
    else r->qideal=q;
    for(int i=0;i<N;i++)
    {
      omFree(names[i]);
    }
    omFreeSize(names,N*sizeof(char*));
    rIncRefCnt(r);
    // check if such ring already exist as ssiRing*
    char name[20];
    int nr=0;
    idhdl h=NULL;
    loop
    {
      snprintf(name,20,"ssiRing%d",nr); nr++;
      h=IDROOT->get(name, 0);
      if (h==NULL)
      {
        break;
      }
      else if ((IDTYP(h)==RING_CMD)
      && (r!=IDRING(h))
      && (rEqual(r,IDRING(h),1)))
      {
        rDelete(r);
        r=rIncRefCnt(IDRING(h));
        break;
      }
    }
    if (new_ref!=-1)
    {
      d->rings[new_ref]=r;
      rIncRefCnt(r);
    }
    return r;
  }
}
static ideal ssiReadIdeal_R_S(char** s,const ring r);
static ring ssiReadRing_R_S(char **s)
{
/* syntax is <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... <Q-ideal> */
  int ch;
  int new_ref=-1;
  ch=s_readint_S(s);
  if (ch==-4)
    return NULL;
  int N=s_readint_S(s);
  char **names;
  coeffs cf=NULL;
  if (ch==-3)
  {
    char *cf_name=ssiReadString_S(s);
    cf=nFindCoeffByName(cf_name);
    if (cf==NULL)
    {
      Werror("cannot find cf:%s",cf_name);
      omFreeBinAddr(cf_name);
      return NULL;
    }
  }
  if (N!=0)
  {
    names=(char**)omAlloc(N*sizeof(char*));
    for(int i=0;i<N;i++)
    {
      names[i]=ssiReadString_S(s);
    }
  }
  // read the orderings:
  int num_ord; // number of orderings
  num_ord=s_readint_S(s);
  rRingOrder_t *ord=(rRingOrder_t *)omAlloc0((num_ord+1)*sizeof(rRingOrder_t));
  int *block0=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int *block1=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int **wvhdl=(int**)omAlloc0((num_ord+1)*sizeof(int*));
  for(int i=0;i<num_ord;i++)
  {
    ord[i]=(rRingOrder_t)s_readint_S(s);
    block0[i]=s_readint_S(s);
    block1[i]=s_readint_S(s);
    switch(ord[i])
    {
      case ringorder_a:
      case ringorder_wp:
      case ringorder_Wp:
      case ringorder_ws:
      case ringorder_Ws:
      case ringorder_aa:
      {
        int ss=block1[i]-block0[i]+1; // #vars
        wvhdl[i]=(int*)omAlloc(ss*sizeof(int));
        for(int ii=0;ii<ss;ii++)
          wvhdl[i][ii]=s_readint_S(s);
      }
      break;
      case ringorder_M:
      {
        int ss=block1[i]-block0[i]+1; // #vars
        wvhdl[i]=(int*)omAlloc(ss*ss*sizeof(int));
        for(int ii=0;ii<ss*ss;ii++)
        {
          wvhdl[i][ii]=s_readint_S(s);
        }
      }
      break;
      case ringorder_a64:
      case ringorder_L:
      case ringorder_IS:
        Werror("ring order not implemented for ssi:%d",ord[i]);
        break;

      default: break;
    }
  }
  if (N==0)
  {
    omFree(ord);
    omFree(block0);
    omFree(block1);
    omFree(wvhdl);
    return NULL;
  }
  else
  {
    ring r=NULL;
    if (ch>=0) /* Q, Z/p */
      r=rDefault(ch,N,names,num_ord,ord,block0,block1,wvhdl);
    else if (ch==-1) /* trans ext. */
    {
      TransExtInfo T;
      T.r=ssiReadRing_R_S(s);
      if (T.r==NULL) return NULL;
      cf=nInitChar(n_transExt,&T);
      r=rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
    }
    else if (ch==-2) /* alg ext. */
    {
      TransExtInfo T;
      T.r=ssiReadRing_R_S(s); /* includes qideal */
      if (T.r==NULL) return NULL;
      cf=nInitChar(n_algExt,&T);
      r=rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
    }
    else if (ch==-3)
    {
      r=rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
    }
    else
    {
      Werror("ssi: read unknown coeffs type (%d)",ch);
      for(int i=0;i<N;i++)
      {
        omFree(names[i]);
      }
      omFreeSize(names,N*sizeof(char*));
      return NULL;
    }
    ideal q=ssiReadIdeal_R_S(s,r);
    if (IDELEMS(q)==0) omFreeBin(q,sip_sideal_bin);
    else r->qideal=q;
    for(int i=0;i<N;i++)
    {
      omFree(names[i]);
    }
    omFreeSize(names,N*sizeof(char*));
    return r;
  }
}
ring ssiReadRing_S(char *s)
{
  ring r=ssiReadRing_R_S(&s);
  // check if such ring already exist as ssiRing*
  char name[20];
  int nr=0;
  idhdl h=NULL;
  loop // already defined?
  {
    snprintf(name,20,"ssiRing%d",nr); nr++;
    h=IDROOT->get(name, 0);
    if (h==NULL)
    {
      break;
    }
    else if ((IDTYP(h)==RING_CMD)
    && (r!=IDRING(h))
    && (rEqual(r,IDRING(h),1)))
    {
      rDelete(r);
      r=rIncRefCnt(IDRING(h));
      break;
    }
  }
  if ((h==NULL) && ((currRing==NULL) || (!rEqual(r,currRing,1))))
  {
    char name[20];
    int nr=0;
    idhdl hh=NULL;
    loop
    {
      snprintf(name,20,"ssiRing%d",nr); nr++;
      hh=IDROOT->get(name, 0);
      if (hh==NULL)
      {
        hh=enterid(name,0,RING_CMD,&IDROOT,FALSE);
        break;
      }
      else if ((IDTYP(hh)==RING_CMD)
      && (rEqual(r,IDRING(hh),1)))
      {
        break;
      }
    }
    rSetHdl(hh);
  }
  else
    rSetHdl(h);
  return r;
}

static poly ssiReadPoly_R(const ssiInfo *d, const ring r)
{
// < # of terms> < term1> < .....
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_POLY, "ssi")) return NULL;
  int n,i,l;
  n=ssiReadInt(d); // # of terms
  //Print("poly: terms:%d\n",n);
  poly p;
  poly ret=NULL;
  poly prev=NULL;
  for(l=0;l<n;l++) // read n terms
  {
// coef,comp.exp1,..exp N
    p=p_Init(r,r->PolyBin);
    pSetCoeff0(p,ssiReadNumber_CF(d,r->cf));
    int D;
    D=s_readint(d->f_read);
    p_SetComp(p,D,r);
    for(i=1;i<=rVar(r);i++)
    {
      D=s_readint(d->f_read);
      p_SetExp(p,i,D,r);
    }
    p_Setm(p,r);
    p_Test(p,r);
    if (ret==NULL) ret=p;
    else           pNext(prev)=p;
    prev=p;
 }
 return ret;
}

static poly ssiReadPoly_R_S(char **s, const ring r)
{
// < # of terms> < term1> < .....
  int n,i,l;
  char* c=*s;
  n=s_readint_S(&c); // # of terms
  poly p;
  poly ret=NULL;
  poly prev=NULL;
  for(l=0;l<n;l++) // read n terms
  {
// coef,comp.exp1,..exp N
    p=p_Init(r,r->PolyBin);
    number cf=ssiReadNumber_CF_S(&c,r->cf);
    pSetCoeff0(p,cf);
    int D;
    D=s_readint_S(&c);
    p_SetComp(p,D,r);
    for(i=1;i<=rVar(r);i++)
    {
      D=s_readint_S(&c);
      p_SetExp(p,i,D,r);
    }
    p_Setm(p,r);
    p_Test(p,r);
    if (ret==NULL) ret=p;
    else           pNext(prev)=p;
    prev=p;
 }
 *s=c;
 return ret;
}

static poly ssiReadPoly(ssiInfo *d)
{
  return ssiReadPoly_R(d,d->r);
}

poly ssiReadPoly_S(char *s, const ring r)
{
  return ssiReadPoly_R_S(&s,r);
}
static ideal ssiReadIdeal_R(const ssiInfo *d,const ring r)
{
// < # of terms> < term1> < .....
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_IDEAL, "ssi")) return NULL;
  int n,i;
  ideal I;
  n=s_readint(d->f_read);
  I=idInit(n,1); // will be fixed later for module/smatrix
  for(i=0;i<IDELEMS(I);i++) // read n terms
  {
    I->m [i]=ssiReadPoly_R(d,r);
  }
  return I;
}
static ideal ssiReadIdeal_R_S(char** s,const ring r)
{
// < # of terms> < term1> < .....
  int n,i;
  ideal I;
  n=s_readint_S(s);
  I=idInit(n,1); // will be fixed later for module/smatrix
  for(i=0;i<IDELEMS(I);i++) // read n terms
  {
    I->m [i]=ssiReadPoly_R_S(s,r);
  }
  return I;
}

ideal ssiReadIdeal(ssiInfo *d)
{
  return ssiReadIdeal_R(d,d->r);
}

ideal ssiReadIdeal_S(char *s, const ring R)
{
  return ssiReadIdeal_R_S(&s,R);
}

static matrix ssiReadMatrix(ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_MATRIX, "ssi")) return NULL;
  int n,m;
  m=s_readint(d->f_read);
  n=s_readint(d->f_read);
  matrix M=mpNew(m,n);
  poly p;
  for(int i=1;i<=MATROWS(M);i++)
    for(int j=1;j<=MATCOLS(M);j++)
    {
      p=ssiReadPoly(d);
      MATELEM(M,i,j)=p;
    }
  return M;
}
static matrix ssiReadMatrix_R_S(char** s, const ring R)
{
  int n,m;
  m=s_readint_S(s);
  n=s_readint_S(s);
  matrix M=mpNew(m,n);
  poly p;
  for(int i=1;i<=MATROWS(M);i++)
    for(int j=1;j<=MATCOLS(M);j++)
    {
      p=ssiReadPoly_R_S(s,R);
      MATELEM(M,i,j)=p;
    }
  return M;
}
matrix ssiReadMatrix_R_S(char* s, const ring R)
{
  return ssiReadMatrix_R_S(&s,R);
}

static command ssiReadCommand(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_COMMAND, "ssi")) return NULL;
  // syntax: <num ops> <operation> <op1> <op2> ....
  command D=(command)omAlloc0(sizeof(*D));
  int argc,op;
  argc=s_readint(d->f_read);
  op=s_readint(d->f_read);
  D->argc=argc; D->op=op;
  leftv v;
  if (argc >0)
  {
    v=ssiRead1(l);
    memcpy(&(D->arg1),v,sizeof(*v));
    omFreeBin(v,sleftv_bin);
  }
  if (argc <4)
  {
    if (D->argc >1)
    {
      v=ssiRead1(l);
      memcpy(&(D->arg2),v,sizeof(*v));
      omFreeBin(v,sleftv_bin);
    }
    if (D->argc >2)
    {
      v=ssiRead1(l);
      memcpy(&(D->arg3),v,sizeof(*v));
      omFreeBin(v,sleftv_bin);
    }
  }
  else
  {
    leftv prev=&(D->arg1);
    argc--;
    while(argc >0)
    {
      v=ssiRead1(l);
      prev->next=v;
      prev=v;
      argc--;
    }
  }
  return D;
}

static procinfov ssiReadProc(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_PROC, "ssi")) return NULL;
  char *s=ssiReadString(d);
  procinfov p=(procinfov)omAlloc0Bin(procinfo_bin);
  p->language=LANG_SINGULAR;
  p->libname=omStrDup("");
  p->procname=omStrDup("");
  p->data.s.body=s;
  return p;
}
static procinfov ssiReadProc_S(char**s)
{
  char *st=ssiReadString_S(s);
  procinfov p=(procinfov)omAlloc0Bin(procinfo_bin);
  p->language=LANG_SINGULAR;
  p->libname=omStrDup("");
  p->procname=omStrDup("");
  p->data.s.body=st;
  return p;
}
static lists ssiReadList(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_LIST, "ssi")) return NULL;
  int nr;
  nr=s_readint(d->f_read);
  lists L=(lists)omAlloc0Bin(slists_bin);
  L->Init(nr);

  int i;
  leftv v;
  for(i=0;i<=L->nr;i++)
  {
    v=ssiRead1(l);
    memcpy(&(L->m[i]),v,sizeof(*v));
    omFreeBin(v,sleftv_bin);
  }
  return L;
}
static lists ssiReadList_S(char**s, const ring R)
{
  int nr;
  nr=s_readint_S(s);
  lists L=(lists)omAlloc0Bin(slists_bin);
  L->Init(nr);

  int i;
  leftv v;
  for(i=0;i<=L->nr;i++)
  {
    v=ssiRead1_S(s,R);
    memcpy(&(L->m[i]),v,sizeof(*v));
    omFreeBin(v,sleftv_bin);
  }
  return L;
}
static intvec* ssiReadIntvec(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_INTVEC, "ssi")) return NULL;
  int nr;
  nr=s_readint(d->f_read);
  intvec *v=new intvec(nr);
  for(int i=0;i<nr;i++)
  {
    (*v)[i]=s_readint(d->f_read);
  }
  return v;
}
static intvec* ssiReadIntvec_S(char**s)
{
  int nr;
  nr=s_readint_S(s);
  intvec *v=new intvec(nr);
  for(int i=0;i<nr;i++)
  {
    (*v)[i]=s_readint_S(s);
  }
  return v;
}
static intvec* ssiReadIntmat(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_INTVEC, "ssi")) return NULL;
  int r,c;
  r=s_readint(d->f_read);
  c=s_readint(d->f_read);
  intvec *v=new intvec(r,c,0);
  for(int i=0;i<r*c;i++)
  {
    (*v)[i]=s_readint(d->f_read);
  }
  return v;
}
static intvec* ssiReadIntmat_S(char**s)
{
  int r,c;
  r=s_readint_S(s);
  c=s_readint_S(s);
  intvec *v=new intvec(r,c,0);
  for(int i=0;i<r*c;i++)
  {
    (*v)[i]=s_readint_S(s);
  }
  return v;
}
static bigintmat* ssiReadBigintmat(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_BIGINTMAT, "ssi")) return NULL;
  int r,c;
  r=s_readint(d->f_read);
  c=s_readint(d->f_read);
  bigintmat *v=new bigintmat(r,c,coeffs_BIGINT);
  for(int i=0;i<r*c;i++)
  {
    (*v)[i]=ssiReadBigInt(d);
  }
  return v;
}
static bigintmat* ssiReadBigintmat_S(char**s)
{
  int r,c;
  r=s_readint_S(s);
  c=s_readint_S(s);
  bigintmat *v=new bigintmat(r,c,coeffs_BIGINT);
  for(int i=0;i<r*c;i++)
  {
    (*v)[i]=ssiReadBigInt_S(s);
  }
  return v;
}
static bigintmat* ssiReadBigintvec(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_BIGINTMAT, "ssi")) return NULL;
  int c;
  c=s_readint(d->f_read);
  bigintmat *v=new bigintmat(1,c,coeffs_BIGINT);
  for(int i=0;i<c;i++)
  {
    (*v)[i]=ssiReadBigInt(d);
  }
  return v;
}
static bigintmat* ssiReadBigintvec_S(char**s)
{
  int c;
  c=s_readint_S(s);
  bigintmat *v=new bigintmat(1,c,coeffs_BIGINT);
  for(int i=0;i<c;i++)
  {
    (*v)[i]=ssiReadBigInt_S(s);
  }
  return v;
}

static void ssiReadBlackbox(leftv res, si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_BLACKBOX, "ssi")) return;
  leftv lv=ssiRead1(l);
  char *name=(char*)lv->data;
  omFreeBin(lv,sleftv_bin);
  int tok;
  blackboxIsCmd(name,tok);
  if (tok>MAX_TOK)
  {
    ring save_ring=currRing;
    idhdl save_hdl=currRingHdl;
    blackbox *b=getBlackboxStuff(tok);
    res->rtyp=tok;
    b->blackbox_deserialize(&b,&(res->data),l);
    if (save_ring!=currRing)
    {
      rChangeCurrRing(save_ring);
      if (save_hdl!=NULL) rSetHdl(save_hdl);
      else currRingHdl=NULL;
    }
  }
  else
  {
    Werror("blackbox %s not found",name);
  }
  omFree(name);
}

static void ssiReadAttrib(leftv res, si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_ATTRIBUTES, "ssi")) return;
  BITSET fl=(BITSET)s_readint(d->f_read);
  int nr_of_attr=s_readint(d->f_read);
  if (nr_of_attr>0)
  {
    for(int i=1;i<nr_of_attr;i++)
    {
    }
  }
  leftv tmp=ssiRead1(l);
  memcpy(res,tmp,sizeof(sleftv));
  memset(tmp,0,sizeof(sleftv));
  omFreeBin(tmp,sleftv_bin);
  if (nr_of_attr>0)
  {
  }
  res->flag=fl;
}
static void ssiReadRingProperties(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_RING_PROPERTIES, "ssi")) return;
  int what=s_readint(d->f_read);
  switch(what)
  {
    case 0: // bitmask
    {
      int lb=s_readint(d->f_read);
      unsigned long bm=~0L;
      bm=bm<<lb;
      bm=~bm;
      rUnComplete(d->r);
      d->r->bitmask=bm;
      rComplete(d->r);
      break;
    }
    case 1: // LPRing
    {
      int lb=s_readint(d->f_read);
      int isLPring=s_readint(d->f_read);
      unsigned long bm=~0L;
      bm=bm<<lb;
      bm=~bm;
      rUnComplete(d->r);
      d->r->bitmask=bm;
      d->r->isLPring=isLPring;
      rComplete(d->r);
      break;
    }
    case 2: // Plural rings
    {
      matrix C=ssiReadMatrix(d);
      matrix D=ssiReadMatrix(d);
      nc_CallPlural(C,D,NULL,NULL,d->r,true,true,false,d->r,false);
      break;
    }
  }
}

/* #ssi2 start */
#define SSI2_VERSION 1

static BOOLEAN ssi2Write(si_link l, leftv data);
static leftv ssi2Read1(si_link l);
static void ssi2WriteRing(ssiInfo *d, const ring r);
static void ssi2WriteRing_R(ssiInfo *d, const ring r);
static ring ssi2ReadRing(ssiInfo *d);
static void ssi2WritePoly_R(const ssiInfo *d, poly p, const ring r);
static poly ssi2ReadPoly_R(const ssiInfo *d, const ring r);
static void ssi2WriteIdeal_R(const ssiInfo *d, int typ, const ideal I, const ring r);
static ideal ssi2ReadIdeal_R(const ssiInfo *d, const ring r);
static matrix ssi2ReadMatrix(ssiInfo *d);

enum ssi2Compression
{
  SSI2_COMP_NONE,
  SSI2_COMP_GZIP,
  SSI2_COMP_ZSTD,
  SSI2_COMP_LZ4,
  SSI2_COMP_INVALID
};

static BOOLEAN ssi2ModeTokenEquals(const char *s, int len, const char *token)
{
  return ((int)strlen(token)==len) && (strncmp(s, token, len)==0);
}

static char ssi2ModeBase(const char *mode)
{
  if ((mode==NULL) || (mode[0]=='\0')) return '\0';
  if (((mode[0]=='r') || (mode[0]=='w') || (mode[0]=='a'))
  && ((mode[1]=='\0') || (mode[1]==',')))
    return mode[0];
  return '?';
}

static BOOLEAN ssi2EndsWith(const char *s, const char *suffix)
{
  size_t slen=strlen(s);
  size_t tlen=strlen(suffix);
  return (slen>=tlen) && (strcmp(s+slen-tlen, suffix)==0);
}

static ssi2Compression ssi2CompressionFromFilename(const char *filename)
{
  if (filename==NULL) return SSI2_COMP_NONE;
  if (ssi2EndsWith(filename, ".gz") || ssi2EndsWith(filename, ".gzip"))
    return SSI2_COMP_GZIP;
  if (ssi2EndsWith(filename, ".zst") || ssi2EndsWith(filename, ".zstd"))
    return SSI2_COMP_ZSTD;
  if (ssi2EndsWith(filename, ".lz4"))
    return SSI2_COMP_LZ4;
  return SSI2_COMP_NONE;
}

static ssi2Compression ssi2ParseModeOptions(const char *mode, char *zstd_long,
                                            int zstd_long_size,
                                            BOOLEAN *has_long,
                                            BOOLEAN *has_compression)
{
  ssi2Compression comp=SSI2_COMP_NONE;
  if (has_long!=NULL) *has_long=FALSE;
  if (has_compression!=NULL) *has_compression=FALSE;
  if (zstd_long!=NULL) strncpy(zstd_long, "--long=23", zstd_long_size);
  char base=ssi2ModeBase(mode);
  if (base=='?')
  {
    Werror("ssi2: invalid mode `%s'", mode);
    return SSI2_COMP_INVALID;
  }
  if ((mode==NULL) || (mode[0]=='\0')) return comp;
  const char *p=strchr(mode, ',');
  while (p!=NULL)
  {
    const char *start=p+1;
    const char *end=strchr(start, ',');
    int len=(end==NULL) ? (int)strlen(start) : (int)(end-start);
    if (len==0)
    {
      Werror("ssi2: empty mode option in `%s'", mode);
      return SSI2_COMP_INVALID;
    }
    ssi2Compression next=SSI2_COMP_NONE;
    if (ssi2ModeTokenEquals(start, len, "gzip")) next=SSI2_COMP_GZIP;
    else if (ssi2ModeTokenEquals(start, len, "zstd")) next=SSI2_COMP_ZSTD;
    else if (ssi2ModeTokenEquals(start, len, "lz4")) next=SSI2_COMP_LZ4;
    else if (ssi2ModeTokenEquals(start, len, "plain")
          || ssi2ModeTokenEquals(start, len, "none"))
    {
      if (has_compression!=NULL) *has_compression=TRUE;
    }
    else if ((len>5) && (strncmp(start, "long=", 5)==0))
    {
      for (int i=5; i<len; i++)
      {
        if (!isdigit((unsigned char)start[i]))
        {
          Werror("ssi2: invalid zstd long window option `%.*s'", len, start);
          return SSI2_COMP_INVALID;
        }
      }
      if (has_long!=NULL) *has_long=TRUE;
      if ((zstd_long!=NULL) && (zstd_long_size>0))
      {
        int n=(len+2<zstd_long_size) ? len : zstd_long_size-3;
        zstd_long[0]='-';
        zstd_long[1]='-';
        strncpy(zstd_long+2, start, n);
        zstd_long[n+2]='\0';
      }
    }
    else
    {
      Werror("ssi2: unknown mode option `%.*s'", len, start);
      return SSI2_COMP_INVALID;
    }
    if (next!=SSI2_COMP_NONE)
    {
      if (has_compression!=NULL) *has_compression=TRUE;
      if ((comp!=SSI2_COMP_NONE) && (comp!=next))
      {
        Werror("ssi2: multiple compression options in mode `%s'", mode);
        return SSI2_COMP_INVALID;
      }
      comp=next;
    }
    p=end;
  }
  return comp;
}

static BOOLEAN ssi2CompressedOpenByCompression(si_link l, short flag,
                                               ssi2Compression comp,
                                               const char *zstd_long);

static void ssi2FlushWriteBuffer(const ssiInfo *d)
{
  ssiInfo *dd=(ssiInfo*)d;
  if ((dd==NULL) || (dd->f_write==NULL) || (dd->write_buff_pos<=0)) return;
  if (fwrite(dd->write_buff, 1, dd->write_buff_pos, dd->f_write)!=(size_t)dd->write_buff_pos)
    WerrorS("ssi2: write failed");
  dd->write_buff_pos=0;
}

static void ssi2FreeWriteBuffer(ssiInfo *d)
{
  if ((d!=NULL) && (d->write_buff!=NULL))
  {
    ssi2FlushWriteBuffer(d);
    omFreeSize(d->write_buff, d->write_buff_size);
    d->write_buff=NULL;
    d->write_buff_pos=0;
    d->write_buff_size=0;
  }
}

static void ssi2WriteRaw(const ssiInfo *d, const void *buf, size_t len)
{
  if (len==0) return;
  ssiInfo *dd=(ssiInfo*)d;
  if (dd->write_buff==NULL)
  {
    dd->write_buff_size=1<<20;
    dd->write_buff=(char*)omAlloc(dd->write_buff_size);
    dd->write_buff_pos=0;
  }
  if (len>=(size_t)dd->write_buff_size)
  {
    ssi2FlushWriteBuffer(d);
    if (fwrite(buf, 1, len, dd->f_write)!=len)
      WerrorS("ssi2: write failed");
    return;
  }
  if (dd->write_buff_pos+(int)len>dd->write_buff_size)
    ssi2FlushWriteBuffer(d);
  memcpy(dd->write_buff+dd->write_buff_pos, buf, len);
  dd->write_buff_pos+=(int)len;
}

static void ssi2Fflush(const ssiInfo *d)
{
  ssi2FlushWriteBuffer(d);
  if ((d!=NULL) && (d->f_write!=NULL))
  {
    fflush(d->f_write);
  }
}

static BOOLEAN ssi2ReadRaw(const ssiInfo *d, void *buf, size_t len)
{
  char *p=(char*)buf;
  while (len>0)
  {
    int chunk=(len>(size_t)INT_MAX) ? INT_MAX : (int)len;
    int got=s_readbytes(p, chunk, d->f_read);
    if (got!=chunk)
    {
      WerrorS("ssi2: unexpected end of input");
      return TRUE;
    }
    p+=chunk;
    len-=chunk;
  }
  return FALSE;
}

static int ssi2ReadByte(const ssiInfo *d)
{
  unsigned char b=0;
  if (s_readbytes((char*)&b, 1, d->f_read)!=1)
  {
    WerrorS("ssi2: unexpected end of input");
    return -1;
  }
  return (int)b;
}

static void ssi2WriteTag(const ssiInfo *d, unsigned char tag)
{
  ssi2WriteRaw(d, &tag, 1);
}

static int ssi2ReadTag(const ssiInfo *d)
{
  int c=ssi2ReadByte(d);
  if (c<0) return -1;
  return c;
}

static void ssi2WriteU64(const ssiInfo *d, uint64_t v)
{
  unsigned char buf[10];
  size_t len=0;
  do
  {
    unsigned char b=(unsigned char)(v & 0x7f);
    v >>= 7;
    if (v!=0) b |= 0x80;
    buf[len++]=b;
  }
  while (v!=0);
  ssi2WriteRaw(d, buf, len);
}

static uint64_t ssi2ReadU64(const ssiInfo *d)
{
  uint64_t v=0;
  int shift=0;
  loop
  {
    int c=ssi2ReadByte(d);
    if (c<0)
    {
      return 0;
    }
    v |= ((uint64_t)(c & 0x7f)) << shift;
    if ((c & 0x80)==0) return v;
    shift += 7;
    if (shift>=64)
    {
      WerrorS("ssi2: integer is too large");
      return 0;
    }
  }
}

static uint64_t ssi2EncodeI64(int64_t v)
{
  return (((uint64_t)v) << 1) ^ (uint64_t)(v >> 63);
}

static int64_t ssi2DecodeI64(uint64_t v)
{
  return (int64_t)((v >> 1) ^ (uint64_t)(-(int64_t)(v & 1)));
}

static void ssi2WriteI64(const ssiInfo *d, int64_t v)
{
  ssi2WriteU64(d, ssi2EncodeI64(v));
}

static int64_t ssi2ReadI64(const ssiInfo *d)
{
  return ssi2DecodeI64(ssi2ReadU64(d));
}

static void ssi2WriteSchemaTable(const ssiInfo *d)
{
  ssi2WriteTag(d, SSI_SCHEMA_TOKEN);
  ssi2WriteU64(d, SSI_SCHEMA_TABLE_VERSION);
  ssi2WriteU64(d, ssiSchemaVersionCount);
  for (int i=0; i<ssiSchemaVersionCount; i++)
  {
    int id=ssiSchemaVersions[i].id;
    ssi2WriteU64(d, id);
    ssi2WriteU64(d, ssiSchemaVersion(d, id));
  }
}

static void ssi2ReadSchemaTable(ssiInfo *d)
{
  int table_version=(int)ssi2ReadU64(d);
  int count=(int)ssi2ReadU64(d);
  if (table_version>SSI_SCHEMA_TABLE_VERSION)
  {
    Print("ssi2: schema table version %d is newer than supported version %d\n",
          table_version, SSI_SCHEMA_TABLE_VERSION);
  }
  for (int i=0; i<count; i++)
  {
    int id=(int)ssi2ReadU64(d);
    int version=(int)ssi2ReadU64(d);
    ssiSetSchemaVersion(d, id, version);
  }
}

static void ssi2WriteString(const ssiInfo *d, const char *s)
{
  size_t l=strlen(s);
  ssi2WriteU64(d, (uint64_t)l);
  ssi2WriteRaw(d, s, l);
}

static char *ssi2ReadString(const ssiInfo *d)
{
  uint64_t l64=ssi2ReadU64(d);
  if (l64>(uint64_t)INT_MAX)
  {
    WerrorS("ssi2: string too large");
    return omStrDup("");
  }
  size_t l=(size_t)l64;
  char *buf=(char*)omAlloc0(l+1);
  if (ssi2ReadRaw(d, buf, l))
  {
    omFree(buf);
    return omStrDup("");
  }
  buf[l]='\0';
  return buf;
}

static void ssi2WriteMpz(const ssiInfo *d, const mpz_t z)
{
  int sign=mpz_sgn(z);
  ssi2WriteI64(d, sign);
  if (sign==0)
  {
    ssi2WriteU64(d, 0);
    return;
  }
  size_t len=(mpz_sizeinbase(z, 2)+7)/8;
  char *buf=(char*)omAlloc(len);
  size_t written=0;
  mpz_export(buf, &written, 1, 1, 1, 0, z);
  ssi2WriteU64(d, (uint64_t)written);
  ssi2WriteRaw(d, buf, written);
  omFreeSize(buf, len);
}

static void ssi2WriteLongAsMpz(const ssiInfo *d, long v)
{
  int sign=(v>0) - (v<0);
  ssi2WriteI64(d, sign);
  if (sign==0)
  {
    ssi2WriteU64(d, 0);
    return;
  }

  unsigned long a=(sign<0) ? (0UL - (unsigned long)v) : (unsigned long)v;
  unsigned char buf[sizeof(unsigned long)];
  size_t len=0;
  while (a!=0)
  {
    buf[sizeof(buf)-1-len]=(unsigned char)(a & 0xff);
    a >>= 8;
    len++;
  }
  ssi2WriteU64(d, len);
  ssi2WriteRaw(d, buf+sizeof(buf)-len, len);
}

static void ssi2ReadMpz(const ssiInfo *d, mpz_t z)
{
  int sign=(int)ssi2ReadI64(d);
  uint64_t len64=ssi2ReadU64(d);
  if (len64>(uint64_t)INT_MAX)
  {
    WerrorS("ssi2: mpz payload too large");
    mpz_set_ui(z, 0);
    return;
  }
  size_t len=(size_t)len64;
  if (len==0)
  {
    mpz_set_ui(z, 0);
    return;
  }
  char *buf=(char*)omAlloc(len);
  if (ssi2ReadRaw(d, buf, len))
  {
    omFreeSize(buf, len);
    mpz_set_ui(z, 0);
    return;
  }
  mpz_import(z, len, 1, 1, 1, 0, buf);
  if (sign<0) mpz_neg(z, z);
  omFreeSize(buf, len);
}

static void ssi2WriteNumberAsMpz(const ssiInfo *d, number n, const coeffs cf)
{
  mpz_t z;
  number tmp=n;
  n_MPZ(z, tmp, cf);
  ssi2WriteMpz(d, z);
  mpz_clear(z);
}

static void ssi2WriteQIntegerNumber(const ssiInfo *d, number n, const coeffs cf)
{
  number tmp=n;
  n_Normalize(tmp, cf);
  if (SR_HDL(tmp) & SR_INT)
    ssi2WriteLongAsMpz(d, SR_TO_INT(tmp));
  else
    ssi2WriteMpz(d, tmp->z);
}

static void ssi2WriteQQNumber(const ssiInfo *d, number n, const coeffs cf)
{
  number tmp=n;
  n_Normalize(tmp, cf);
  if (SR_HDL(tmp) & SR_INT)
  {
    ssi2WriteLongAsMpz(d, SR_TO_INT(tmp));
    ssi2WriteLongAsMpz(d, 1);
  }
  else
  {
    ssi2WriteMpz(d, tmp->z);
    if (tmp->s==3)
      ssi2WriteLongAsMpz(d, 1);
    else
      ssi2WriteMpz(d, tmp->n);
  }
}
/* #ssi2 end */

/* #ssi2 start */
static void ssi2WriteNumber_CF(const ssiInfo *d, number n, const coeffs cf)
{
  switch (getCoeffType(cf))
  {
    case n_transExt:
    {
      fraction f=(fraction)n;
      ssi2WritePoly_R(d, NUM(f), cf->extRing);
      ssi2WritePoly_R(d, DEN(f), cf->extRing);
      break;
    }
    case n_algExt:
      ssi2WritePoly_R(d, (poly)n, cf->extRing);
      break;
    case n_Q:
      if (cf->is_field)
      {
        ssi2WriteQQNumber(d, n, cf);
      }
      else
      {
        ssi2WriteQIntegerNumber(d, n, cf);
      }
      break;
    case n_Z:
      {
        ssi2WriteNumberAsMpz(d, n, cf);
      }
      break;
    case n_Zp:
    case n_GF:
      {
        number tmp=n;
        ssi2WriteI64(d, n_Int(tmp, cf));
      }
      break;
    default:
      Werror("ssi2: coeff type %d not implemented", (int)getCoeffType(cf));
      break;
  }
}

static number ssi2ReadNumber_CF(const ssiInfo *d, const coeffs cf)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_NUMBER, "ssi2")) return NULL;
  switch (getCoeffType(cf))
  {
    case n_transExt:
    {
      fraction f=(fraction)n_Init(1, cf);
      p_Delete(&NUM(f), cf->extRing);
      NUM(f)=ssi2ReadPoly_R(d, cf->extRing);
      DEN(f)=ssi2ReadPoly_R(d, cf->extRing);
      return (number)f;
    }
    case n_algExt:
      return (number)ssi2ReadPoly_R(d, cf->extRing);
    case n_Q:
      if (cf->is_field)
      {
        mpz_t num;
        mpz_t den;
        mpz_init(num);
        mpz_init(den);
        ssi2ReadMpz(d, num);
        ssi2ReadMpz(d, den);
        number n=n_InitMPZ(num, cf);
        number dnum=n_InitMPZ(den, cf);
        number res=n_Div(n, dnum, cf);
        n_Delete(&n, cf);
        n_Delete(&dnum, cf);
        mpz_clear(den);
        mpz_clear(num);
        return res;
      }
      else
      {
        mpz_t z;
        mpz_init(z);
        ssi2ReadMpz(d, z);
        number res=n_InitMPZ(z, cf);
        mpz_clear(z);
        return res;
      }
    case n_Z:
      {
        mpz_t z;
        mpz_init(z);
        ssi2ReadMpz(d, z);
        number res=n_InitMPZ(z, cf);
        mpz_clear(z);
        return res;
      }
    case n_Zp:
    case n_GF:
      return n_Init((long)ssi2ReadI64(d), cf);
    default:
      Werror("ssi2: coeff type %d not implemented", (int)getCoeffType(cf));
      return n_Init(0, cf);
  }
}

static void ssi2WriteNumber(const ssiInfo *d, number n)
{
  ssi2WriteNumber_CF(d, n, d->r->cf);
}

static number ssi2ReadNumber(ssiInfo *d)
{
  return ssi2ReadNumber_CF(d, d->r->cf);
}

static void ssi2WriteBigInt(const ssiInfo *d, number n)
{
  ssi2WriteQIntegerNumber(d, n, coeffs_BIGINT);
}

static number ssi2ReadBigInt(const ssiInfo *d)
{
  mpz_t z;
  mpz_init(z);
  ssi2ReadMpz(d, z);
  number res=n_InitMPZ(z, coeffs_BIGINT);
  mpz_clear(z);
  return res;
}
/* #ssi2 end */

/* #ssi2 start */
static void ssi2WriteRing_R(ssiInfo *d, const ring r)
{
  if (r!=NULL)
  {
    for (int i=0; i<SI_RING_CACHE; i++)
    {
      if (d->rings[i]==r)
      {
        ssi2WriteI64(d, -5);
        ssi2WriteU64(d, i);
        return;
      }
    }
    for (int i=0; i<SI_RING_CACHE; i++)
    {
      if (d->rings[i]==NULL)
      {
        d->rings[i]=rIncRefCnt(r);
        ssi2WriteI64(d, -6);
        ssi2WriteU64(d, i);
        break;
      }
    }
    if (rField_is_Q(r) || rField_is_Zp(r))
      ssi2WriteI64(d, n_GetChar(r->cf));
    else if (rFieldType(r)==n_transExt)
      ssi2WriteI64(d, -1);
    else if (rFieldType(r)==n_algExt)
      ssi2WriteI64(d, -2);
    else
    {
      ssi2WriteI64(d, -3);
      ssi2WriteString(d, nCoeffName(r->cf));
    }
    ssi2WriteU64(d, r->N);
    for (int i=0; i<r->N; i++)
      ssi2WriteString(d, r->names[i]);
    int n_ord=0;
    if (r->order!=NULL) while (r->order[n_ord]!=0) n_ord++;
    ssi2WriteU64(d, n_ord);
    for (int i=0; i<n_ord; i++)
    {
      ssi2WriteI64(d, r->order[i]);
      ssi2WriteI64(d, r->block0[i]);
      ssi2WriteI64(d, r->block1[i]);
      switch (r->order[i])
      {
        case ringorder_a:
        case ringorder_wp:
        case ringorder_Wp:
        case ringorder_ws:
        case ringorder_Ws:
        case ringorder_aa:
        {
          int s=r->block1[i]-r->block0[i]+1;
          for (int j=0; j<s; j++) ssi2WriteI64(d, r->wvhdl[i][j]);
          break;
        }
        case ringorder_M:
        {
          int s=r->block1[i]-r->block0[i]+1;
          for (int j=0; j<s*s; j++) ssi2WriteI64(d, r->wvhdl[i][j]);
          break;
        }
        case ringorder_a64:
        case ringorder_L:
        case ringorder_IS:
          Werror("ring order not implemented for ssi2:%d", r->order[i]);
          break;
        default:
          break;
      }
    }
    if ((rFieldType(r)==n_transExt) || (rFieldType(r)==n_algExt))
      ssi2WriteRing_R(d, r->cf->extRing);
    if (r->qideal!=NULL)
      ssi2WriteIdeal_R(d, IDEAL_CMD, r->qideal, r);
    else
      ssi2WriteU64(d, 0);
  }
  else
  {
    ssi2WriteI64(d, 0);
    ssi2WriteU64(d, 0);
    ssi2WriteU64(d, 0);
    ssi2WriteU64(d, 0);
  }
  if ((r!=NULL) && rIsLPRing(r))
  {
    ssi2WriteTag(d, 23);
    ssi2WriteU64(d, 1);
    ssi2WriteU64(d, SI_LOG2(r->bitmask));
    ssi2WriteI64(d, r->isLPring);
  }
  else if (r!=NULL)
  {
    unsigned long bm=0;
    int b=0;
    bm=rGetExpSize(bm, b, r->N);
    if (r->bitmask!=bm)
    {
      ssi2WriteTag(d, 23);
      ssi2WriteU64(d, 0);
      ssi2WriteU64(d, SI_LOG2(r->bitmask));
    }
    if (rIsPluralRing(r))
    {
      ssi2WriteTag(d, 23);
      ssi2WriteU64(d, 2);
      ssi2WriteIdeal_R(d, MATRIX_CMD, (ideal)r->GetNC()->C, r);
      ssi2WriteIdeal_R(d, MATRIX_CMD, (ideal)r->GetNC()->D, r);
    }
  }
}

static void ssi2WriteRing(ssiInfo *d, const ring r)
{
  if ((r==NULL) || (r->cf==NULL))
  {
    ssi2WriteI64(d, -4);
    return;
  }
  if (r==currRing)
  {
    if (d->r!=NULL) rKill(d->r);
    d->r=r;
  }
  rIncRefCnt(r);
  ssi2WriteRing_R(d, r);
}

static ring ssi2ReadRing(ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_RING, "ssi2")) return NULL;
  int ch=(int)ssi2ReadI64(d);
  int new_ref=-1;
  if (ch==-6)
  {
    new_ref=(int)ssi2ReadU64(d);
    ch=(int)ssi2ReadI64(d);
  }
  if (ch==-5)
  {
    int index=(int)ssi2ReadU64(d);
    ring r=d->rings[index];
    rIncRefCnt(r);
    return r;
  }
  if (ch==-4) return NULL;
  int N=(int)ssi2ReadU64(d);
  char **names=NULL;
  coeffs cf=NULL;
  if (ch==-3)
  {
    char *cf_name=ssi2ReadString(d);
    cf=nFindCoeffByName(cf_name);
    if (cf==NULL)
    {
      Werror("cannot find cf:%s", cf_name);
      omFree(cf_name);
      return NULL;
    }
    omFree(cf_name);
  }
  if (N!=0)
  {
    names=(char**)omAlloc(N*sizeof(char*));
    for (int i=0; i<N; i++) names[i]=ssi2ReadString(d);
  }
  int num_ord=(int)ssi2ReadU64(d);
  rRingOrder_t *ord=(rRingOrder_t*)omAlloc0((num_ord+1)*sizeof(rRingOrder_t));
  int *block0=(int*)omAlloc0((num_ord+1)*sizeof(int));
  int *block1=(int*)omAlloc0((num_ord+1)*sizeof(int));
  int **wvhdl=(int**)omAlloc0((num_ord+1)*sizeof(int*));
  for (int i=0; i<num_ord; i++)
  {
    ord[i]=(rRingOrder_t)ssi2ReadI64(d);
    block0[i]=(int)ssi2ReadI64(d);
    block1[i]=(int)ssi2ReadI64(d);
    switch (ord[i])
    {
      case ringorder_a:
      case ringorder_wp:
      case ringorder_Wp:
      case ringorder_ws:
      case ringorder_Ws:
      case ringorder_aa:
      {
        int s=block1[i]-block0[i]+1;
        wvhdl[i]=(int*)omAlloc(s*sizeof(int));
        for (int j=0; j<s; j++) wvhdl[i][j]=(int)ssi2ReadI64(d);
        break;
      }
      case ringorder_M:
      {
        int s=block1[i]-block0[i]+1;
        wvhdl[i]=(int*)omAlloc(s*s*sizeof(int));
        for (int j=0; j<s*s; j++) wvhdl[i][j]=(int)ssi2ReadI64(d);
        break;
      }
      case ringorder_a64:
      case ringorder_L:
      case ringorder_IS:
        Werror("ring order not implemented for ssi2:%d", ord[i]);
        break;
      default:
        break;
    }
  }
  if (N==0)
  {
    omFree(ord);
    omFree(block0);
    omFree(block1);
    omFree(wvhdl);
    return NULL;
  }
  ring r=NULL;
  if (ch>=0)
    r=rDefault(ch, N, names, num_ord, ord, block0, block1, wvhdl);
  else if (ch==-1)
  {
    TransExtInfo T;
    T.r=ssi2ReadRing(d);
    if (T.r==NULL) return NULL;
    cf=nInitChar(n_transExt, &T);
    r=rDefault(cf, N, names, num_ord, ord, block0, block1, wvhdl);
  }
  else if (ch==-2)
  {
    TransExtInfo T;
    T.r=ssi2ReadRing(d);
    if (T.r==NULL) return NULL;
    cf=nInitChar(n_algExt, &T);
    r=rDefault(cf, N, names, num_ord, ord, block0, block1, wvhdl);
  }
  else if (ch==-3)
    r=rDefault(cf, N, names, num_ord, ord, block0, block1, wvhdl);
  else
  {
    Werror("ssi2: read unknown coeffs type (%d)", ch);
    for (int i=0; i<N; i++) omFree(names[i]);
    omFreeSize(names, N*sizeof(char*));
    return NULL;
  }
  ideal q=ssi2ReadIdeal_R(d, r);
  if (IDELEMS(q)==0) omFreeBin(q, sip_sideal_bin);
  else r->qideal=q;
  for (int i=0; i<N; i++) omFree(names[i]);
  omFreeSize(names, N*sizeof(char*));
  rIncRefCnt(r);
  char name[20];
  int nr=0;
  idhdl h=NULL;
  loop
  {
    snprintf(name, 20, "ssiRing%d", nr); nr++;
    h=IDROOT->get(name, 0);
    if (h==NULL) break;
    else if ((IDTYP(h)==RING_CMD) && (r!=IDRING(h)) && (rEqual(r, IDRING(h), 1)))
    {
      rDelete(r);
      r=rIncRefCnt(IDRING(h));
      break;
    }
  }
  if (new_ref!=-1)
  {
    d->rings[new_ref]=r;
    rIncRefCnt(r);
  }
  return r;
}
/* #ssi2 end */

/* #ssi2 start */
static void ssi2WritePoly_R(const ssiInfo *d, poly p, const ring r)
{
  ssi2WriteU64(d, pLength(p));
  while (p!=NULL)
  {
    ssi2WriteNumber_CF(d, pGetCoeff(p), r->cf);
    ssi2WriteI64(d, p_GetComp(p, r));
    for (int j=1; j<=rVar(r); j++)
      ssi2WriteI64(d, p_GetExp(p, j, r));
    pIter(p);
  }
}

static void ssi2WritePoly(const ssiInfo *d, poly p)
{
  ssi2WritePoly_R(d, p, d->r);
}

static poly ssi2ReadPoly_R(const ssiInfo *d, const ring r)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_POLY, "ssi2")) return NULL;
  int n=(int)ssi2ReadU64(d);
  poly ret=NULL;
  poly prev=NULL;
  for (int l=0; l<n; l++)
  {
    poly p=p_Init(r, r->PolyBin);
    pSetCoeff0(p, ssi2ReadNumber_CF(d, r->cf));
    p_SetComp(p, (int)ssi2ReadI64(d), r);
    for (int i=1; i<=rVar(r); i++)
      p_SetExp(p, i, (long)ssi2ReadI64(d), r);
    p_Setm(p, r);
    p_Test(p, r);
    if (ret==NULL) ret=p;
    else pNext(prev)=p;
    prev=p;
  }
  return ret;
}

static poly ssi2ReadPoly(ssiInfo *d)
{
  return ssi2ReadPoly_R(d, d->r);
}

static void ssi2WriteIdeal_R(const ssiInfo *d, int typ, const ideal I, const ring R)
{
  matrix M=(matrix)I;
  int mn;
  if (typ==MATRIX_CMD)
  {
    mn=MATROWS(M)*MATCOLS(M);
    ssi2WriteU64(d, MATROWS(M));
    ssi2WriteU64(d, MATCOLS(M));
  }
  else
  {
    mn=IDELEMS(I);
    ssi2WriteU64(d, IDELEMS(I));
  }
  for (int i=0; i<mn; i++) ssi2WritePoly_R(d, I->m[i], R);
}

static void ssi2WriteIdeal(const ssiInfo *d, int typ, const ideal I)
{
  ssi2WriteIdeal_R(d, typ, I, d->r);
}

static ideal ssi2ReadIdeal_R(const ssiInfo *d, const ring r)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_IDEAL, "ssi2")) return NULL;
  int n=(int)ssi2ReadU64(d);
  ideal I=idInit(n, 1);
  for (int i=0; i<IDELEMS(I); i++) I->m[i]=ssi2ReadPoly_R(d, r);
  return I;
}

static ideal ssi2ReadIdeal(ssiInfo *d)
{
  return ssi2ReadIdeal_R(d, d->r);
}

static matrix ssi2ReadMatrix(ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_MATRIX, "ssi2")) return NULL;
  int m=(int)ssi2ReadU64(d);
  int n=(int)ssi2ReadU64(d);
  matrix M=mpNew(m, n);
  for (int i=1; i<=MATROWS(M); i++)
    for (int j=1; j<=MATCOLS(M); j++)
      MATELEM(M, i, j)=ssi2ReadPoly(d);
  return M;
}
/* #ssi2 end */

/* #ssi2 start */
static void ssi2WriteCommand(si_link l, command D)
{
  ssiInfo *d=(ssiInfo*)l->data;
  ssi2WriteU64(d, D->argc);
  ssi2WriteI64(d, D->op);
  if (D->argc>0) ssi2Write(l, &(D->arg1));
  if (D->argc<4)
  {
    if (D->argc>1) ssi2Write(l, &(D->arg2));
    if (D->argc>2) ssi2Write(l, &(D->arg3));
  }
}

static command ssi2ReadCommand(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_COMMAND, "ssi2")) return NULL;
  command D=(command)omAlloc0(sizeof(*D));
  int argc=(int)ssi2ReadU64(d);
  int op=(int)ssi2ReadI64(d);
  D->argc=argc;
  D->op=op;
  leftv v;
  if (argc>0)
  {
    v=ssi2Read1(l);
    memcpy(&(D->arg1), v, sizeof(*v));
    omFreeBin(v, sleftv_bin);
  }
  if (argc<4)
  {
    if (D->argc>1)
    {
      v=ssi2Read1(l);
      memcpy(&(D->arg2), v, sizeof(*v));
      omFreeBin(v, sleftv_bin);
    }
    if (D->argc>2)
    {
      v=ssi2Read1(l);
      memcpy(&(D->arg3), v, sizeof(*v));
      omFreeBin(v, sleftv_bin);
    }
  }
  else
  {
    leftv prev=&(D->arg1);
    argc--;
    while (argc>0)
    {
      v=ssi2Read1(l);
      prev->next=v;
      prev=v;
      argc--;
    }
  }
  return D;
}

static void ssi2WriteProc(const ssiInfo *d, procinfov p)
{
  if (p->data.s.body==NULL) iiGetLibProcBuffer(p);
  if (p->data.s.body!=NULL) ssi2WriteString(d, p->data.s.body);
  else ssi2WriteString(d, "");
}

static procinfov ssi2ReadProc(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_PROC, "ssi2")) return NULL;
  char *s=ssi2ReadString(d);
  procinfov p=(procinfov)omAlloc0Bin(procinfo_bin);
  p->language=LANG_SINGULAR;
  p->libname=omStrDup("");
  p->procname=omStrDup("");
  p->data.s.body=s;
  return p;
}

static void ssi2WriteList(si_link l, lists dd)
{
  ssiInfo *d=(ssiInfo*)l->data;
  int Ll=dd->nr;
  ssi2WriteU64(d, Ll+1);
  for (int i=0; i<=Ll; i++) ssi2Write(l, &(dd->m[i]));
}

static lists ssi2ReadList(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_LIST, "ssi2")) return NULL;
  int nr=(int)ssi2ReadU64(d);
  lists L=(lists)omAlloc0Bin(slists_bin);
  L->Init(nr);
  for (int i=0; i<=L->nr; i++)
  {
    leftv v=ssi2Read1(l);
    memcpy(&(L->m[i]), v, sizeof(*v));
    omFreeBin(v, sleftv_bin);
  }
  return L;
}

static void ssi2WriteIntvec(const ssiInfo *d, intvec *v)
{
  ssi2WriteU64(d, v->length());
  for (int i=0; i<v->length(); i++) ssi2WriteI64(d, (*v)[i]);
}

static intvec *ssi2ReadIntvec(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_INTVEC, "ssi2")) return NULL;
  int nr=(int)ssi2ReadU64(d);
  intvec *v=new intvec(nr);
  for (int i=0; i<nr; i++) (*v)[i]=(int)ssi2ReadI64(d);
  return v;
}

static void ssi2WriteIntmat(const ssiInfo *d, intvec *v)
{
  ssi2WriteU64(d, v->rows());
  ssi2WriteU64(d, v->cols());
  for (int i=0; i<v->length(); i++) ssi2WriteI64(d, (*v)[i]);
}

static intvec *ssi2ReadIntmat(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_INTVEC, "ssi2")) return NULL;
  int r=(int)ssi2ReadU64(d);
  int c=(int)ssi2ReadU64(d);
  intvec *v=new intvec(r, c, 0);
  for (int i=0; i<r*c; i++) (*v)[i]=(int)ssi2ReadI64(d);
  return v;
}

static void ssi2WriteBigintmat(const ssiInfo *d, bigintmat *v)
{
  ssi2WriteU64(d, v->rows());
  ssi2WriteU64(d, v->cols());
  for (int i=0; i<v->length(); i++) ssi2WriteBigInt(d, (*v)[i]);
}

static bigintmat *ssi2ReadBigintmat(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_BIGINTMAT, "ssi2")) return NULL;
  int r=(int)ssi2ReadU64(d);
  int c=(int)ssi2ReadU64(d);
  bigintmat *v=new bigintmat(r, c, coeffs_BIGINT);
  for (int i=0; i<r*c; i++) (*v)[i]=ssi2ReadBigInt(d);
  return v;
}

static void ssi2WriteBigintvec(const ssiInfo *d, bigintmat *v)
{
  ssi2WriteU64(d, v->cols());
  for (int i=0; i<v->length(); i++) ssi2WriteBigInt(d, (*v)[i]);
}

static bigintmat *ssi2ReadBigintvec(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_BIGINTMAT, "ssi2")) return NULL;
  int c=(int)ssi2ReadU64(d);
  bigintmat *v=new bigintmat(1, c, coeffs_BIGINT);
  for (int i=0; i<c; i++) (*v)[i]=ssi2ReadBigInt(d);
  return v;
}

static void ssi2ReadBlackbox(leftv, si_link)
{
  /* If this is implemented later, dispatch by SSI_SCHEMA_BLACKBOX here. */
  WerrorS("ssi2: blackbox serialization is not implemented");
}

static void ssi2ReadAttrib(leftv res, si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_ATTRIBUTES, "ssi2")) return;
  BITSET fl=(BITSET)ssi2ReadU64(d);
  int nr_of_attr=(int)ssi2ReadU64(d);
  if (nr_of_attr>0)
  {
    for (int i=1; i<nr_of_attr; i++) {}
  }
  leftv tmp=ssi2Read1(l);
  memcpy(res, tmp, sizeof(sleftv));
  memset(tmp, 0, sizeof(sleftv));
  omFreeBin(tmp, sleftv_bin);
  res->flag=fl;
}

static void ssi2ReadRingProperties(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_RING_PROPERTIES, "ssi2")) return;
  int what=(int)ssi2ReadU64(d);
  switch (what)
  {
    case 0:
    {
      int lb=(int)ssi2ReadU64(d);
      unsigned long bm=~0L;
      bm=bm<<lb;
      bm=~bm;
      rUnComplete(d->r);
      d->r->bitmask=bm;
      rComplete(d->r);
      break;
    }
    case 1:
    {
      int lb=(int)ssi2ReadU64(d);
      int isLPring=(int)ssi2ReadI64(d);
      unsigned long bm=~0L;
      bm=bm<<lb;
      bm=~bm;
      rUnComplete(d->r);
      d->r->bitmask=bm;
      d->r->isLPring=isLPring;
      rComplete(d->r);
      break;
    }
    case 2:
    {
      matrix C=ssi2ReadMatrix(d);
      matrix D=ssi2ReadMatrix(d);
      nc_CallPlural(C, D, NULL, NULL, d->r, true, true, false, d->r, false);
      break;
    }
  }
}
/* #ssi2 end */

/* #ssi2 start */
static void ssi2WriteHeader(const ssiInfo *d)
{
  ssi2WriteTag(d, 98);
  ssi2WriteU64(d, SSI2_VERSION);
  ssi2WriteU64(d, MAX_TOK);
  ssi2WriteU64(d, si_opt_1);
  ssi2WriteU64(d, si_opt_2);
}

static leftv ssi2Read1(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  leftv res=(leftv)omAlloc0Bin(sleftv_bin);
  int t=ssi2ReadTag(d);
  switch (t)
  {
    case 1:
      res->rtyp=INT_CMD;
      res->data=(char*)(long)ssi2ReadI64(d);
      break;
    case 2:
      res->rtyp=STRING_CMD;
      res->data=(char*)ssi2ReadString(d);
      break;
    case 3:
      res->rtyp=NUMBER_CMD;
      if (d->r==NULL) goto no_ring;
      ssiCheckCurrRing(d->r);
      res->data=(char*)ssi2ReadNumber(d);
      break;
    case 4:
      res->rtyp=BIGINT_CMD;
      res->data=(char*)ssi2ReadBigInt(d);
      break;
    case 15:
    case 5:
    {
      d->r=ssi2ReadRing(d);
      if (errorreported) return NULL;
      res->data=(char*)d->r;
      if (d->r!=NULL) rIncRefCnt(d->r);
      res->rtyp=RING_CMD;
      if (t==15)
      {
        if (ssiSetCurrRing(d->r)) d->r=currRing;
        omFreeBin(res, sleftv_bin);
        return ssi2Read1(l);
      }
      break;
    }
    case 6:
      res->rtyp=POLY_CMD;
      if (d->r==NULL) goto no_ring;
      ssiCheckCurrRing(d->r);
      res->data=(char*)ssi2ReadPoly(d);
      break;
    case 7:
      res->rtyp=IDEAL_CMD;
      if (d->r==NULL) goto no_ring;
      ssiCheckCurrRing(d->r);
      res->data=(char*)ssi2ReadIdeal(d);
      break;
    case 8:
      res->rtyp=MATRIX_CMD;
      if (d->r==NULL) goto no_ring;
      ssiCheckCurrRing(d->r);
      res->data=(char*)ssi2ReadMatrix(d);
      break;
    case 9:
      res->rtyp=VECTOR_CMD;
      if (d->r==NULL) goto no_ring;
      ssiCheckCurrRing(d->r);
      res->data=(char*)ssi2ReadPoly(d);
      break;
    case 10:
    case 22:
      res->rtyp=(t==22) ? SMATRIX_CMD : MODUL_CMD;
      if (d->r==NULL) goto no_ring;
      ssiCheckCurrRing(d->r);
      if (ssiRequireSchemaVersion(d, SSI_SCHEMA_MODULE, "ssi2"))
      {
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      {
        int rk=(int)ssi2ReadI64(d);
        ideal M=ssi2ReadIdeal(d);
        M->rank=rk;
        res->data=(char*)M;
      }
      break;
    case 11:
      res->rtyp=COMMAND;
      res->data=ssi2ReadCommand(l);
      if (res->data==NULL)
      {
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      if (res->Eval()) WerrorS("error in eval");
      break;
    case 12:
      res->rtyp=0;
      res->name=(char*)ssi2ReadString(d);
      if (res->Eval()) WerrorS("error in name lookup");
      break;
    case 13:
      res->rtyp=PROC_CMD;
      res->data=ssi2ReadProc(d);
      break;
    case 14:
      res->rtyp=LIST_CMD;
      res->data=ssi2ReadList(l);
      break;
    case 16:
      res->rtyp=NONE;
      res->data=NULL;
      break;
    case 17:
      res->rtyp=INTVEC_CMD;
      res->data=ssi2ReadIntvec(d);
      break;
    case 18:
      res->rtyp=INTMAT_CMD;
      res->data=ssi2ReadIntmat(d);
      break;
    case 19:
      res->rtyp=BIGINTMAT_CMD;
      res->data=ssi2ReadBigintmat(d);
      break;
    case 20:
      ssi2ReadBlackbox(res, l);
      break;
    case 21:
      ssi2ReadAttrib(res, l);
      break;
    case 23:
      ssi2ReadRingProperties(l);
      omFreeBin(res, sleftv_bin);
      return ssi2Read1(l);
    case 24:
      res->rtyp=BIGINTVEC_CMD;
      res->data=ssi2ReadBigintvec(d);
      break;
    case SSI_SCHEMA_TOKEN:
      ssi2ReadSchemaTable(d);
      omFreeBin(res, sleftv_bin);
      return ssi2Read1(l);
    case 98:
    {
      int n98_v=(int)ssi2ReadU64(d);
      int n98_m=(int)ssi2ReadU64(d);
      BITSET n98_o1=(BITSET)ssi2ReadU64(d);
      BITSET n98_o2=(BITSET)ssi2ReadU64(d);
      if ((n98_v>SSI2_VERSION) || (n98_m!=MAX_TOK))
      {
        Print("incompatible versions of ssi2: %d/%d vs %d/%d\n",
              SSI2_VERSION, MAX_TOK, n98_v, n98_m);
      }
      si_opt_1=n98_o1;
      si_opt_2=n98_o2;
      omFreeBin(res, sleftv_bin);
      return ssi2Read1(l);
    }
    case 99:
      omFreeBin(res, sleftv_bin);
      ssiClose(l);
      m2_end(-1);
      break;
    case -1:
      ssiClose(l);
      res->rtyp=DEF_CMD;
      break;
    default:
      Werror("ssi2: not implemented (t:%d)", t);
      omFreeBin(res, sleftv_bin);
      res=NULL;
      break;
  }
  if ((d->r!=NULL) && (currRing!=d->r) && (res!=NULL) && (res->RingDependend()))
  {
    if (ssiSetCurrRing(d->r)) d->r=currRing;
  }
  return res;
no_ring:
  WerrorS("no ring");
  omFreeBin(res, sleftv_bin);
  return NULL;
}

static BOOLEAN ssi2Write(si_link l, leftv data)
{
  if (SI_LINK_W_OPEN_P(l)==0)
    if (slOpen(l, SI_LINK_OPEN|SI_LINK_WRITE, NULL)) return TRUE;
  ssiInfo *d=(ssiInfo*)l->data;
  d->level++;
  while (data!=NULL)
  {
    int tt=data->Typ();
    void *dd=data->Data();
    attr *aa=data->Attribute();
    if ((aa!=NULL) && ((*aa)!=NULL))
    {
      attr a=*aa;
      int n=0;
      while (a!=NULL) { n++; a=a->next; }
      ssi2WriteTag(d, 21);
      ssi2WriteU64(d, data->flag);
      ssi2WriteU64(d, n);
    }
    else if (data->flag!=0)
    {
      ssi2WriteTag(d, 21);
      ssi2WriteU64(d, data->flag);
      ssi2WriteU64(d, 0);
    }
    if ((dd==NULL) && (data->name!=NULL) && (tt==0)) tt=DEF_CMD;
    switch (tt)
    {
      case 0:
      case NONE:
        ssi2WriteTag(d, 16);
        break;
      case STRING_CMD:
        ssi2WriteTag(d, 2);
        ssi2WriteString(d, (char*)dd);
        break;
      case INT_CMD:
        ssi2WriteTag(d, 1);
        ssi2WriteI64(d, (long)dd);
        break;
      case BIGINT_CMD:
        ssi2WriteTag(d, 4);
        ssi2WriteBigInt(d, (number)dd);
        break;
      case NUMBER_CMD:
        if (d->r!=currRing)
        {
          ssi2WriteTag(d, 15);
          ssi2WriteRing(d, currRing);
        }
        ssi2WriteTag(d, 3);
        ssi2WriteNumber(d, (number)dd);
        break;
      case RING_CMD:
        ssi2WriteTag(d, 5);
        ssi2WriteRing(d, (ring)dd);
        break;
      case BUCKET_CMD:
      {
        sBucket_pt b=(sBucket_pt)dd;
        if (d->r!=sBucketGetRing(b))
        {
          ssi2WriteTag(d, 15);
          ssi2WriteRing(d, sBucketGetRing(b));
        }
        ssi2WriteTag(d, 6);
        ssi2WritePoly(d, sBucketPeek(b));
        break;
      }
      case POLY_CMD:
      case VECTOR_CMD:
        if (d->r!=currRing)
        {
          ssi2WriteTag(d, 15);
          ssi2WriteRing(d, currRing);
        }
        ssi2WriteTag(d, (tt==POLY_CMD) ? 6 : 9);
        ssi2WritePoly(d, (poly)dd);
        break;
      case IDEAL_CMD:
      case MODUL_CMD:
      case MATRIX_CMD:
      case SMATRIX_CMD:
        if (d->r!=currRing)
        {
          ssi2WriteTag(d, 15);
          ssi2WriteRing(d, currRing);
        }
        if (tt==IDEAL_CMD) ssi2WriteTag(d, 7);
        else if (tt==MATRIX_CMD) ssi2WriteTag(d, 8);
        else
        {
          ideal M=(ideal)dd;
          ssi2WriteTag(d, (tt==MODUL_CMD) ? 10 : 22);
          ssi2WriteI64(d, M->rank);
        }
        ssi2WriteIdeal(d, tt, (ideal)dd);
        break;
      case COMMAND:
        ssi2WriteTag(d, 11);
        ssi2WriteCommand(l, (command)dd);
        break;
      case DEF_CMD:
        ssi2WriteTag(d, 12);
        ssi2WriteString(d, data->Name());
        break;
      case PROC_CMD:
        ssi2WriteTag(d, 13);
        ssi2WriteProc(d, (procinfov)dd);
        break;
      case LIST_CMD:
        ssi2WriteTag(d, 14);
        ssi2WriteList(l, (lists)dd);
        break;
      case INTVEC_CMD:
        ssi2WriteTag(d, 17);
        ssi2WriteIntvec(d, (intvec*)dd);
        break;
      case INTMAT_CMD:
        ssi2WriteTag(d, 18);
        ssi2WriteIntmat(d, (intvec*)dd);
        break;
      case BIGINTMAT_CMD:
        ssi2WriteTag(d, 19);
        ssi2WriteBigintmat(d, (bigintmat*)dd);
        break;
      case BIGINTVEC_CMD:
        ssi2WriteTag(d, 24);
        ssi2WriteBigintvec(d, (bigintmat*)dd);
        break;
      default:
        Werror("ssi2: not implemented (t:%d, rtyp:%d)", tt, data->rtyp);
        d->level=0;
        return TRUE;
    }
    if (d->level<=1) ssi2Fflush(d);
    data=data->next;
  }
  d->level--;
  return FALSE;
}
/* #ssi2 end */
//**************************************************************************/

BOOLEAN ssiOpen(si_link l, short flag, leftv u)
{
  if (l!=NULL)
  {
    const char *mode;
    if (flag & SI_LINK_OPEN)
    {
      if (strcmp(l->mode, "r") == 0)
        flag = SI_LINK_READ;
      else if (strcmp(l->mode,"string")==0)
      {
        SI_LINK_SET_RW_OPEN_P(l);
        return FALSE;
      }
      else flag = SI_LINK_WRITE;
    }
    if (((flag == SI_LINK_READ)
      || (flag == SI_LINK_WRITE))
    && (strcmp(l->mode,"string")==0))
    {
      SI_LINK_SET_RW_OPEN_P(l);
      return FALSE;
    }

    if (flag == SI_LINK_READ) mode = "r";
    else if (strcmp(l->mode, "w") == 0) mode = "w";
    else if (strcmp(l->mode, "fork") == 0) mode = "fork";
    else if (strcmp(l->mode, "tcp") == 0) mode = "tcp";
    else if (strcmp(l->mode, "connect") == 0) mode = "connect";
    else mode = "a";


    SI_LINK_SET_OPEN_P(l, flag);
    if(l->data!=NULL) omFreeSize(l->data,sizeof(ssiInfo));
    omFreeBinAddr(l->mode);
    l->mode = omStrDup(mode);

    ssiInfo *d=(ssiInfo*)omAlloc0(sizeof(ssiInfo));
    l->data=d;
    ssiInitSchemaVersions(d);
    if (l->name[0] == '\0')
    {
      if (strcmp(mode,"fork")==0)
      {
        int cpus = (long) feOptValue(FE_OPT_CPUS);
        if (cpus<1)
        {
          WerrorS("no sub-processes allowed");
          l->flags=0;
          l->data=NULL;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        int pc[2];
        int cp[2];
        int err1=pipe(pc);
        int err2=pipe(cp);
        if (err1 || err2)
        {
          Werror("pipe failed with %d\n",errno);
          l->flags=0;
          l->data=NULL;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        link_list n=(link_list)omAlloc(sizeof(link_struct));
        n->u=u;
        n->l=l;
        n->next=(void *)ssiToBeClosed;
        ssiToBeClosed=n;

        pid_t pid = fork();
        if (pid == -1 && errno == EAGAIN)   // RLIMIT_NPROC too low?
        {
          raise_rlimit_nproc();
          pid = fork();
        }
        if (pid == -1)
        {
          WerrorS("could not fork");
          l->flags=0;
          l->data=NULL;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        if (pid==0) /*fork: child*/
        {
          /* block SIGINT */
          sigset_t sigint;
          sigemptyset(&sigint);
          sigaddset(&sigint, SIGINT);
          sigprocmask(SIG_BLOCK, &sigint, NULL);
          si_set_signal(SIGTERM,sig_term_hdl);
          /* set #cpu to 1 for the child:*/
          feSetOptValue(FE_OPT_CPUS,1);

          link_list hh=(link_list)ssiToBeClosed->next;
          /* we know: l is the first entry in ssiToBeClosed-list */
          while(hh!=NULL)
          {
            SI_LINK_SET_CLOSE_P(hh->l);
            ssiInfo *dd=(ssiInfo*)hh->l->data;
            s_close(dd->f_read);
            fclose(dd->f_write);
            if (dd->r!=NULL) rKill(dd->r);
            omFreeSize((ADDRESS)dd,(sizeof *dd));
            hh->l->data=NULL;
            link_list nn=(link_list)hh->next;
            omFree(hh);
            hh=nn;
          }
          ssiToBeClosed->next=NULL;
#ifdef HAVE_SIMPLEIPC
          memset(sem_acquired, 0, SIPC_MAX_SEMAPHORES*sizeof(sem_acquired[0]));
#endif   // HAVE_SIMPLEIPC
          si_close(pc[1]); si_close(cp[0]);
          d->f_write=fdopen(cp[1],"w");
          d->f_read=s_open(pc[0]);
          d->fd_read=pc[0];
          d->fd_write=cp[1];
          //d->r=currRing;
          //if (d->r!=NULL) d->r->ref++;
          l->data=d;
          omFreeBinAddr(l->mode);
          l->mode = omStrDup(mode);
          singular_in_batchmode=TRUE;
          SI_LINK_SET_RW_OPEN_P(l);
          //myynest=0;
          fe_fgets_stdin=fe_fgets_dummy;
          if ((u!=NULL)&&(u->rtyp==IDHDL))
          {
            idhdl h=(idhdl)u->data;
            h->lev=0;
          }
          loop
          {
            if (!SI_LINK_OPEN_P(l)) m2_end(-1);
            if(d->f_read->is_eof) m2_end(-1);
            leftv h=ssiRead1(l); /*contains an exit.... */
            if (feErrors != NULL && *feErrors != '\0')
            {
              // handle errors:
              PrintS(feErrors); /* currently quite simple */
              *feErrors = '\0';
            }
            ssiWrite(l,h);
            h->CleanUp();
            omFreeBin(h, sleftv_bin);
          }
          /* never reached*/
        }
        else if (pid>0) /*fork: parent*/
        {
          d->pid=pid;
          si_close(pc[0]); si_close(cp[1]);
          d->f_write=fdopen(pc[1],"w");
          d->f_read=s_open(cp[0]);
          d->fd_read=cp[0];
          d->fd_write=pc[1];
          SI_LINK_SET_RW_OPEN_P(l);
          d->send_quit_at_exit=1;
          //d->r=currRing;
          //if (d->r!=NULL) d->r->ref++;
        }
        else
        {
          Werror("fork failed (%d)",errno);
          l->data=NULL;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
      }
      // ---------------------------------------------------------------------
      else if (strcmp(mode,"tcp")==0)
      {
        int sockfd, newsockfd, portno, clilen;
        struct sockaddr_in serv_addr, cli_addr;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0)
        {
          WerrorS("ERROR opening socket");
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        memset((char *) &serv_addr,0, sizeof(serv_addr));
        portno = 1025;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        do
        {
          portno++;
          serv_addr.sin_port = htons(portno);
          if(portno > 50000)
          {
            WerrorS("ERROR on binding (no free port available?)");
            l->data=NULL;
            l->flags=0;
            omFreeSize(d,sizeof(ssiInfo));
            return TRUE;
          }
        }
        while(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0);
        Print("waiting on port %d\n", portno);mflush();
        listen(sockfd,1);
        newsockfd = si_accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if(newsockfd < 0)
        {
          WerrorS("ERROR on accept");
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        PrintS("client accepted\n");
        d->fd_read = newsockfd;
        d->fd_write = newsockfd;
        d->f_read = s_open(newsockfd);
        d->f_write = fdopen(newsockfd, "w");
        SI_LINK_SET_RW_OPEN_P(l);
        si_close(sockfd);
      }
      // no ssi-Link on stdin or stdout
      else if (strcmp(mode,"string")==0)
      {
        SI_LINK_SET_RW_OPEN_P(l);
      }
      else
      {
        Werror("invalid mode >>%s<< for ssi",mode);
        l->data=NULL;
        l->flags=0;
        omFreeSize(d,sizeof(ssiInfo));
        return TRUE;
      }
    }
    // =========================================================================
    else /*now l->name!=NULL*/
    {
      // tcp mode
      if(strcmp(mode,"tcp")==0)
      {
        int sockfd, newsockfd, portno, clilen;
        struct sockaddr_in serv_addr, cli_addr;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0)
        {
          WerrorS("ERROR opening socket");
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        memset((char *) &serv_addr,0, sizeof(serv_addr));
        portno = 1025;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        do
        {
          portno++;
          serv_addr.sin_port = htons(portno);
          if(portno > 50000)
          {
            WerrorS("ERROR on binding (no free port available?)");
            l->data=NULL;
            l->flags=0;
            omFreeSize(d,sizeof(ssiInfo));
            return TRUE;
          }
        }
        while(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0);
        //Print("waiting on port %d\n", portno);mflush();
        listen(sockfd,1);
        char* cli_host = (char*)omAlloc(256);
        char* path = (char*)omAlloc(1024);
        int r = si_sscanf(l->name,"%255[^:]:%s",cli_host,path);
        if(r == 0)
        {
          WerrorS("ERROR: no host specified");
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          omFree(path);
          omFree(cli_host);
          return TRUE;
        }
        else if(r == 1)
        {
          WarnS("program not specified, using /usr/local/bin/Singular");
          Warn("in line >>%s<<",my_yylinebuf);
          strcpy(path,"/usr/local/bin/Singular");
        }
        char* ssh_command = (char*)omAlloc(256);
        char* ser_host = (char*)omAlloc(64);
        if(strcmp(cli_host,"localhost")==0)
          strcpy(ser_host,"localhost");
        else
          gethostname(ser_host,64);
        if (strcmp(cli_host,"localhost")==0) /*avoid "ssh localhost" as key may change*/
          snprintf(ssh_command,256,"%s -q --batch --link=ssi --MPhost=%s --MPport=%d &",path,ser_host,portno);
        else
          snprintf(ssh_command,256,"ssh %s %s -q --batch --link=ssi --MPhost=%s --MPport=%d &",cli_host,path,ser_host,portno);
        //Print("client on %s started:%s\n",cli_host,path);
        omFree(path);
        omFree(cli_host);
        if (TEST_OPT_PROT) { Print("running >>%s<<\n",ssh_command); }
        int re=system(ssh_command);
        if (re<0)
        {
          Werror("ERROR running `%s` (%d)",ssh_command,re);
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        omFree(ssh_command);
        omFree(ser_host);
        clilen = sizeof(cli_addr);
        newsockfd = si_accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if(newsockfd < 0)
        {
          WerrorS("ERROR on accept");
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        //PrintS("client accepted\n");
        d->fd_read = newsockfd;
        d->fd_write = newsockfd;
        d->f_read = s_open(newsockfd);
        d->f_write = fdopen(newsockfd, "w");
        si_close(sockfd);
        SI_LINK_SET_RW_OPEN_P(l);
        d->send_quit_at_exit=1;
        link_list newlink=(link_list)omAlloc(sizeof(link_struct));
        newlink->u=u;
        newlink->l=l;
        newlink->next=(void *)ssiToBeClosed;
        ssiToBeClosed=newlink;
        fprintf(d->f_write,"98 %d %d %u %u\n",SSI_VERSION,MAX_TOK,si_opt_1,si_opt_2);
        ssiWriteSchemaTable(d);
      }
      // ----------------------------------------------------------------------
      else if(strcmp(mode,"connect")==0)
      {
        char* host = (char*)omAlloc(256);
        int sockfd, portno;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        si_sscanf(l->name,"%255[^:]:%d",host,&portno);
        //Print("connect to host %s, port %d\n",host,portno);mflush();
        if (portno!=0)
        {
          sockfd = socket(AF_INET, SOCK_STREAM, 0);
          if (sockfd < 0)
          {
            WerrorS("ERROR opening socket");
            l->flags=0;
            l->data=NULL;
            omFreeSize(d,sizeof(ssiInfo));
            return TRUE;
          }
          server = gethostbyname(host);
          if (server == NULL)
          {
            WerrorS("ERROR, no such host");
            l->flags=0;
            l->data=NULL;
            omFreeSize(d,sizeof(ssiInfo));
            return TRUE;
          }
          memset((char *) &serv_addr, 0, sizeof(serv_addr));
          serv_addr.sin_family = AF_INET;
          memcpy((char *)&serv_addr.sin_addr.s_addr,
                (char *)server->h_addr,
                server->h_length);
          serv_addr.sin_port = htons(portno);
          if (si_connect(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
          {
            Werror("ERROR connecting(errno=%d)",errno);
            l->flags=0;
            l->data=NULL;
            omFreeSize(d,sizeof(ssiInfo));
            return TRUE;
          }
          //PrintS("connected\n");mflush();
          d->f_read=s_open(sockfd);
          d->fd_read=sockfd;
          d->f_write=fdopen(sockfd,"w");
          d->fd_write=sockfd;
          SI_LINK_SET_RW_OPEN_P(l);
          omFree(host);
        }
        else
        {
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
      }
      // ======================================================================
      else
      {
        // normal link to a file
        if (FE_OPT_NO_SHELL_FLAG) {WerrorS("no links allowed");return TRUE;}
        FILE *outfile;
        char *filename=l->name;

        if(filename[0]=='>')
        {
          if (filename[1]=='>')
          {
            filename+=2;
            mode = "a";
          }
          else
          {
            filename++;
            mode="w";
          }
        }
        outfile=myfopen(filename,mode);
        if (outfile!=NULL)
        {
          if (strcmp(l->mode,"r")==0)
          {
            fclose(outfile);
            d->f_read=s_open_by_name(filename);
          }
          else
          {
            d->f_write = outfile;
            fprintf(d->f_write,"98 %d %d %u %u\n",SSI_VERSION,MAX_TOK,si_opt_1,si_opt_2);
            ssiWriteSchemaTable(d);
          }
        }
        else
        {
          omFree(d);
          l->data=NULL;
          l->flags=0;
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

/* #ssi2 start */
BOOLEAN ssi2Open(si_link l, short flag, leftv)
{
  if (l==NULL) return TRUE;
  const char *link_mode=(l->mode!=NULL) ? l->mode : "";
  char base=ssi2ModeBase(link_mode);
  if (base=='?')
  {
    Werror("ssi2: invalid mode `%s'", link_mode);
    return TRUE;
  }
  char zstd_long[32];
  BOOLEAN has_long=FALSE;
  BOOLEAN has_compression=FALSE;
  ssi2Compression comp=ssi2ParseModeOptions(link_mode, zstd_long, sizeof(zstd_long),
                                            &has_long, &has_compression);
  if (comp==SSI2_COMP_INVALID) return TRUE;
  const char *mode;
  if (flag & SI_LINK_OPEN)
  {
    if (base=='r') flag=SI_LINK_READ;
    else flag=SI_LINK_WRITE;
  }
  if ((comp==SSI2_COMP_NONE) && (!has_compression))
    comp=ssi2CompressionFromFilename(l->name);
  if ((has_long) && (comp!=SSI2_COMP_ZSTD))
  {
    WerrorS("ssi2: long=N is only valid with zstd compression");
    return TRUE;
  }
  if (comp!=SSI2_COMP_NONE)
    return ssi2CompressedOpenByCompression(l, flag, comp, zstd_long);
  if (flag==SI_LINK_READ) mode="r";
  else if (base=='a') mode="a";
  else mode="w";

  SI_LINK_SET_OPEN_P(l, flag);
  if (l->data!=NULL) omFreeSize(l->data, sizeof(ssiInfo));
  omFreeBinAddr(l->mode);
  l->mode=omStrDup(mode);

  ssiInfo *d=(ssiInfo*)omAlloc0(sizeof(ssiInfo));
  l->data=d;
  ssiInitSchemaVersions(d);
  d->ssi2_format=1;
  if ((l->name==NULL) || (l->name[0]=='\0'))
  {
    WerrorS("ssi2: file name required");
    l->data=NULL;
    l->flags=0;
    omFreeSize(d, sizeof(ssiInfo));
    return TRUE;
  }

  if (flag==SI_LINK_READ)
  {
    d->f_read=s_open_by_name(l->name);
    if (d->f_read==NULL)
    {
      l->data=NULL;
      l->flags=0;
      omFreeSize(d, sizeof(ssiInfo));
      return TRUE;
    }
    SI_LINK_SET_R_OPEN_P(l);
  }
  else
  {
    char *filename=l->name;
    if (filename[0]=='>')
    {
      if (filename[1]=='>')
      {
        filename+=2;
        mode="a";
      }
      else
      {
        filename++;
        mode="w";
      }
    }
    d->f_write=myfopen(filename, mode);
    if (d->f_write==NULL)
    {
      l->data=NULL;
      l->flags=0;
      omFreeSize(d, sizeof(ssiInfo));
      return TRUE;
    }
    ssi2WriteHeader(d);
    ssi2WriteSchemaTable(d);
    ssi2Fflush(d);
    SI_LINK_SET_W_OPEN_P(l);
  }
  return FALSE;
}

static BOOLEAN ssi2CompressedOpen(si_link l, short flag,
                                  const char *link_type,
                                  const char *program,
                                  char *const read_argv[],
                                  char *const write_argv[])
{
  if (l==NULL) return TRUE;
  char base=ssi2ModeBase(l->mode);
  if (base=='?')
  {
    Werror("%s: invalid mode `%s'", link_type, l->mode);
    return TRUE;
  }
  const char *mode;
  if (flag & SI_LINK_OPEN)
  {
    if (base=='r') flag=SI_LINK_READ;
    else flag=SI_LINK_WRITE;
  }
  if (flag==SI_LINK_READ) mode="r";
  else if (base=='a') mode="a";
  else mode="w";

  SI_LINK_SET_OPEN_P(l, flag);
  if (l->data!=NULL) omFreeSize(l->data, sizeof(ssiInfo));
  omFreeBinAddr(l->mode);
  l->mode=omStrDup(mode);

  ssiInfo *d=(ssiInfo*)omAlloc0(sizeof(ssiInfo));
  l->data=d;
  ssiInitSchemaVersions(d);
  d->ssi2_format=1;
  d->compressor_name=link_type;
  if ((l->name==NULL) || (l->name[0]=='\0'))
  {
    Werror("%s: file name required", link_type);
    l->data=NULL;
    l->flags=0;
    omFreeSize(d, sizeof(ssiInfo));
    return TRUE;
  }

  char *filename=l->name;
  if (flag!=SI_LINK_READ && filename[0]=='>')
  {
    if (filename[1]=='>')
    {
      filename+=2;
      mode="a";
    }
    else
    {
      filename++;
      mode="w";
    }
  }

  int pc[2];
  if (pipe(pc)!=0)
  {
    Werror("%s: pipe failed with %d", link_type, errno);
    l->data=NULL;
    l->flags=0;
    omFreeSize(d, sizeof(ssiInfo));
    return TRUE;
  }

  pid_t pid=fork();
  if (pid==-1 && errno==EAGAIN)
  {
    raise_rlimit_nproc();
    pid=fork();
  }
  if (pid==-1)
  {
    Werror("%s: could not fork %s", link_type, program);
    si_close(pc[0]);
    si_close(pc[1]);
    l->data=NULL;
    l->flags=0;
    omFreeSize(d, sizeof(ssiInfo));
    return TRUE;
  }

  if (pid==0)
  {
    if (flag==SI_LINK_READ)
    {
      int fd=si_open(filename, O_RDONLY);
      if (fd<0) _exit(126);
      si_close(pc[0]);
      si_dup2(fd, STDIN_FILENO);
      si_dup2(pc[1], STDOUT_FILENO);
      si_close(fd);
      si_close(pc[1]);
      execvp(program, read_argv);
      _exit(127);
    }
    else
    {
      int open_flags=O_WRONLY | O_CREAT;
      open_flags |= (strcmp(mode, "a")==0) ? O_APPEND : O_TRUNC;
      int fd=si_open(filename, open_flags, 0666);
      if (fd<0) _exit(126);
      si_close(pc[1]);
      si_dup2(pc[0], STDIN_FILENO);
      si_dup2(fd, STDOUT_FILENO);
      si_close(pc[0]);
      si_close(fd);
      execvp(program, write_argv);
      _exit(127);
    }
  }

  d->pid=pid;
  if (flag==SI_LINK_READ)
  {
    si_close(pc[1]);
    d->fd_read=pc[0];
    d->f_read=s_open(pc[0]);
    SI_LINK_SET_R_OPEN_P(l);
  }
  else
  {
    si_close(pc[0]);
    d->fd_write=pc[1];
    d->f_write=fdopen(pc[1], "w");
    if (d->f_write==NULL)
    {
      si_close(pc[1]);
      kill(pid, SIGTERM);
      si_waitpid(pid, NULL, 0);
      l->data=NULL;
      l->flags=0;
      omFreeSize(d, sizeof(ssiInfo));
      return TRUE;
    }
    ssi2WriteHeader(d);
    ssi2WriteSchemaTable(d);
    ssi2Fflush(d);
    SI_LINK_SET_W_OPEN_P(l);
  }
  return FALSE;
}

static BOOLEAN ssi2CompressedOpenByCompression(si_link l, short flag,
                                               ssi2Compression comp,
                                               const char *zstd_long)
{
  switch (comp)
  {
    case SSI2_COMP_GZIP:
    {
      char *const read_argv[]={(char*)"gzip", (char*)"-cd", NULL};
      char *const write_argv[]={(char*)"gzip", (char*)"-c", NULL};
      return ssi2CompressedOpen(l, flag, "ssi2:gzip", "gzip", read_argv, write_argv);
    }
    case SSI2_COMP_ZSTD:
    {
      char *const read_argv[]={(char*)"zstd", (char*)"-q", (char*)"-d", (char*)"-c", NULL};
      char *const write_argv[]={(char*)"zstd", (char*)"-q", (char*)"-3",
        (char*)((zstd_long!=NULL) ? zstd_long : "--long=23"), (char*)"-c", NULL};
      return ssi2CompressedOpen(l, flag, "ssi2:zstd", "zstd", read_argv, write_argv);
    }
    case SSI2_COMP_LZ4:
    {
      char *const read_argv[]={(char*)"lz4", (char*)"-q", (char*)"-d", (char*)"-c", NULL};
      char *const write_argv[]={(char*)"lz4", (char*)"-q", (char*)"-1", (char*)"-c", NULL};
      return ssi2CompressedOpen(l, flag, "ssi2:lz4", "lz4", read_argv, write_argv);
    }
    default:
      WerrorS("ssi2: invalid compression option");
      return TRUE;
  }
}

static BOOLEAN ssi2zOpen(si_link l, short flag, leftv u)
{
  return ssi2CompressedOpenByCompression(l, flag, SSI2_COMP_GZIP, NULL);
}

static BOOLEAN ssi2zstdOpen(si_link l, short flag, leftv u)
{
  return ssi2CompressedOpenByCompression(l, flag, SSI2_COMP_ZSTD, "--long=23");
}

static BOOLEAN ssi2lz4Open(si_link l, short flag, leftv u)
{
  return ssi2CompressedOpenByCompression(l, flag, SSI2_COMP_LZ4, NULL);
}

static BOOLEAN ssi2zClose(si_link l)
{
  BOOLEAN res=FALSE;
  if (l!=NULL)
  {
    SI_LINK_SET_CLOSE_P(l);
    ssiInfo *d=(ssiInfo*)l->data;
    if (d!=NULL)
    {
      if (d->r!=NULL) rKill(d->r);
      for (int i=0; i<SI_RING_CACHE; i++)
      {
        if (d->rings[i]!=NULL) rKill(d->rings[i]);
        d->rings[i]=NULL;
      }
      BOOLEAN was_read=(d->f_read!=NULL);
      if (d->f_read!=NULL) { s_close(d->f_read); d->f_read=NULL; }
      ssi2FreeWriteBuffer(d);
      if (d->f_write!=NULL) { if (fclose(d->f_write)!=0) res=TRUE; d->f_write=NULL; }
      if (d->pid>1)
      {
        const char *compressor_name=(d->compressor_name!=NULL) ? d->compressor_name : "ssi2z";
        int status=0;
        if (si_waitpid(d->pid, &status, 0)!=d->pid)
          res=TRUE;
        else if ((status!=0)
        && !(was_read && WIFSIGNALED(status) && (WTERMSIG(status)==SIGPIPE)))
        {
          Werror("%s: compressor exited with status %d", compressor_name, status);
          res=TRUE;
        }
      }
      l->data=NULL;
      omFreeSize(d, sizeof(ssiInfo));
    }
  }
  return res;
}
/* #ssi2 end */

//**************************************************************************/
#if 0
static BOOLEAN ssiPrepClose(si_link l)
{
  if (l!=NULL)
  {
    SI_LINK_SET_CLOSE_P(l);
    ssiInfo *d = (ssiInfo *)l->data;
    if (d!=NULL)
    {
      if (d->send_quit_at_exit)
      {
        fputs("99\n",d->f_write);
        fflush(d->f_write);
      }
      d->quit_sent=1;
    }
  }
  return FALSE;
}
#endif

BOOLEAN ssiClose(si_link l)
{
  if (l!=NULL)
  {
    SI_LINK_SET_CLOSE_P(l);
    ssiInfo *d = (ssiInfo *)l->data;
    if (d!=NULL)
    {
      if ((d->ssi2_format) && (d->compressor_name!=NULL))
        return ssi2zClose(l);
      // send quit signal
      if ((d->send_quit_at_exit)
      && (d->quit_sent==0))
      {
        fputs("99\n",d->f_write);
        fflush(d->f_write);
        d->quit_sent=1;
      }
      // clean ring
      if (d->r!=NULL) rKill(d->r);
      for(int i=0;i<SI_RING_CACHE;i++)
      {
        if (d->rings[i]!=NULL)  rKill(d->rings[i]);
        d->rings[i]=NULL;
      }
      if (d->f_read!=NULL) { s_close(d->f_read);d->f_read=NULL;}
      ssi2FreeWriteBuffer(d);
      if (d->f_write!=NULL) { fclose(d->f_write); d->f_write=NULL; }
      if (((strcmp(l->mode,"tcp")==0)
      || (strcmp(l->mode,"fork")==0))
      && (d->pid>1))
      {
        // did the child stop ?
        int pid=si_waitpid(d->pid,NULL,WNOHANG);
        if ((pid==0) /* no status change for child*/
        && (kill(d->pid,0)==0)) // child is still running
        {
          struct timespec t;
          struct timespec rem;
          // wait 60 sec
          for(int i=0;i<50;i++)
          {
            // wait till signal or 100ms:
            t.tv_sec=0;
            t.tv_nsec=100000000; // <=100 ms
            nanosleep(&t, &rem);
            // child finished ?
            if (si_waitpid(d->pid,NULL,WNOHANG) == d->pid) break;
          }
          if (kill(d->pid,0)==0) // child still exists
          {
            kill(d->pid,SIGTERM);
            t.tv_sec=1;
            t.tv_nsec=0; // <=1000 ms
            nanosleep(&t, &rem);
            si_waitpid(d->pid,NULL,WNOHANG);
          }
        }
      }
      if ((strcmp(l->mode,"tcp")==0)
      || (strcmp(l->mode,"fork")==0))
      {
        link_list hh=ssiToBeClosed;
        if (hh!=NULL)
        {
          if (hh->l==l)
          {
             ssiToBeClosed=(link_list)hh->next;
             omFreeSize(hh,sizeof(link_struct));
          }
          else while(hh->next!=NULL)
          {
            link_list hhh=(link_list)hh->next;
            if (hhh->l==l)
            {
              hh->next=hhh->next;
              omFreeSize(hhh,sizeof(link_struct));
              break;
            }
            else
              hh=(link_list)hh->next;
          }
        }
      }
      omFreeSize((ADDRESS)d,(sizeof *d));
    }
    l->data=NULL;
  }
  return FALSE;
}

//**************************************************************************/
leftv ssiRead1(si_link l)
{
  ssiInfo *d = (ssiInfo *)l->data;
  leftv res=(leftv)omAlloc0Bin(sleftv_bin);
  int t=0;
  t=s_readint(d->f_read);
  //Print("got type %d\n",t);
  switch(t)
  {
    case 1:res->rtyp=INT_CMD;
           res->data=(char *)(long)ssiReadInt(d);
           //Print("int: %d\n",(int)(long)res->data);
           break;
    case 2:res->rtyp=STRING_CMD;
           res->data=(char *)ssiReadString(d);
           //Print("str: %s\n",(char*)res->data);
           break;
    case 3:res->rtyp=NUMBER_CMD;
           if (d->r==NULL) goto no_ring;
           ssiCheckCurrRing(d->r);
           res->data=(char *)ssiReadNumber(d);
           //Print("number\n");
           break;
    case 4:res->rtyp=BIGINT_CMD;
           res->data=(char *)ssiReadBigInt(d);
           //Print("bigint\n");
           break;
    case 15:
    case 5:{
           //Print("ring %d\n",t);
             d->r=ssiReadRing(d);
             if (errorreported) return NULL;
             res->data=(char*)d->r;
             if (d->r!=NULL) rIncRefCnt(d->r);
             res->rtyp=RING_CMD;
             if (t==15) // setring
             {
               if(ssiSetCurrRing(d->r)) { d->r=currRing; }
               omFreeBin(res,sleftv_bin);
               return ssiRead1(l);
             }
           }
           break;
    case 6:res->rtyp=POLY_CMD;
           //Print("poly\n");
           if (d->r==NULL) goto no_ring;
           ssiCheckCurrRing(d->r);
           res->data=(char*)ssiReadPoly(d);
           break;
    case 7:res->rtyp=IDEAL_CMD;
           //Print("ideal\n");
           if (d->r==NULL) goto no_ring;
           ssiCheckCurrRing(d->r);
           res->data=(char*)ssiReadIdeal(d);
           break;
    case 8:res->rtyp=MATRIX_CMD;
           //Print("matrix\n");
           if (d->r==NULL) goto no_ring;
           ssiCheckCurrRing(d->r);
           res->data=(char*)ssiReadMatrix(d);
           break;
    case 9:res->rtyp=VECTOR_CMD;
           //Print("vector\n");
           if (d->r==NULL) goto no_ring;
           ssiCheckCurrRing(d->r);
           res->data=(char*)ssiReadPoly(d);
           break;
    case 10:
    case 22:if (t==22) res->rtyp=SMATRIX_CMD;
           else        res->rtyp=MODUL_CMD;
           //Print("module/smatrix %d\n",t);
           if (d->r==NULL) goto no_ring;
           ssiCheckCurrRing(d->r);
           if (ssiRequireSchemaVersion(d, SSI_SCHEMA_MODULE, "ssi"))
           {
             omFreeBin(res,sleftv_bin);
             return NULL;
           }
           {
             int rk=s_readint(d->f_read);
             ideal M=ssiReadIdeal(d);
             M->rank=rk;
             res->data=(char*)M;
           }
           break;
    case 11:
           {
           //Print("cmd\n",t);
             res->rtyp=COMMAND;
             res->data=ssiReadCommand(l);
             if (res->data==NULL)
             {
               omFreeBin(res,sleftv_bin);
               return NULL;
             }
             int nok=res->Eval();
             if (nok) WerrorS("error in eval");
             break;
           }
    case 12: /*DEF_CMD*/
           {
           //Print("def\n",t);
             res->rtyp=0;
             res->name=(char *)ssiReadString(d);
             int nok=res->Eval();
             if (nok) WerrorS("error in name lookup");
             break;
           }
    case 13: res->rtyp=PROC_CMD;
             res->data=ssiReadProc(d);
             break;
    case 14: res->rtyp=LIST_CMD;
             res->data=ssiReadList(l);
             break;
    case 16: res->rtyp=NONE; res->data=NULL;
             break;
    case 17: res->rtyp=INTVEC_CMD;
             res->data=ssiReadIntvec(d);
             break;
    case 18: res->rtyp=INTMAT_CMD;
             res->data=ssiReadIntmat(d);
             break;
    case 19: res->rtyp=BIGINTMAT_CMD;
             res->data=ssiReadBigintmat(d);
             break;
    case 20: ssiReadBlackbox(res,l);
             break;
    case 21: ssiReadAttrib(res,l);
             break;
    case 23: ssiReadRingProperties(l);
             return ssiRead1(l);
             break;
    case 24: res->rtyp=BIGINTVEC_CMD;
             res->data=ssiReadBigintvec(d);
             break;
    case SSI_SCHEMA_TOKEN:
             ssiReadSchemaTable(d);
             omFreeBin(res,sleftv_bin);
             return ssiRead1(l);
    // ------------
    case 98: // version
             {
                int n98_v,n98_m;
                BITSET n98_o1,n98_o2;
                n98_v=s_readint(d->f_read);
                n98_m=s_readint(d->f_read);
                n98_o1=s_readint(d->f_read);
                n98_o2=s_readint(d->f_read);
                if ((n98_v>SSI_VERSION) ||(n98_m!=MAX_TOK))
                {
                  Print("incompatible versions of ssi: %d/%d vs %d/%d\n",
                                  SSI_VERSION,MAX_TOK,n98_v,n98_m);
                }
                #ifndef SING_NDEBUG
                if (TEST_OPT_DEBUG)
                  Print("// opening ssi-%d, MAX_TOK=%d\n",n98_v,n98_m);
                #endif
                si_opt_1=n98_o1;
                si_opt_2=n98_o2;
                omFreeBin(res,sleftv_bin);
                return ssiRead1(l);
             }
    case 99: omFreeBin(res,sleftv_bin); ssiClose(l); m2_end(-1);
             break; /*to make compiler happy*/
    case 0: if (s_iseof(d->f_read))
            {
              ssiClose(l);
            }
            res->rtyp=DEF_CMD;
            break;
    default: Werror("not implemented (t:%d)",t);
             omFreeBin(res,sleftv_bin);
             res=NULL;
             break;
  }
  // if currRing is required for the result, but lost
  // define "ssiRing%d" as currRing:
  if ((d->r!=NULL)
  && (currRing!=d->r)
  && (res->RingDependend()))
  {
    if(ssiSetCurrRing(d->r)) { d->r=currRing; }
  }
  return res;
no_ring: WerrorS("no ring");
  omFreeBin(res,sleftv_bin);
  return NULL;
}
leftv ssiRead1_S(char**s, const ring R)
{
  leftv res=(leftv)omAlloc0Bin(sleftv_bin);
  int t=0;
  t=s_readint_S(s);
  switch(t)
  {
    case 1:res->rtyp=INT_CMD;
           res->data=(char *)(long)ssiReadInt_S(s);
           break;
    case 2:res->rtyp=STRING_CMD;
           res->data=(char *)ssiReadString_S(s);
           break;
    case 3:res->rtyp=NUMBER_CMD;
           res->data=(char *)ssiReadNumber_CF_S(s,R->cf);
           break;
    case 4:res->rtyp=BIGINT_CMD;
           res->data=(char *)ssiReadBigInt_S(s);
           //Print("bigint\n");
           break;
    case 15:
    case 5:{
             //Print("ring %d\n",t);
             ring r=ssiReadRing_R_S(s);
             if (errorreported||(r==NULL)) return NULL;
             res->rtyp=RING_CMD;
             res->data=(char*)r;
             if (/*(t==15)&&*/
             ((currRing==NULL)||(!rSamePolyRep(r,currRing))))
             {
               rChangeCurrRing(r);
             }
             break;
           }
    case 6:res->rtyp=POLY_CMD;
           res->data=(char*)ssiReadPoly_R_S(s,R);
           break;
    case 7:res->rtyp=IDEAL_CMD;
           res->data=(char*)ssiReadIdeal_R_S(s,R);
           break;
    case 8:res->rtyp=MATRIX_CMD;
           res->data=(char*)ssiReadMatrix_R_S(s,R);
           break;
    case 9:res->rtyp=VECTOR_CMD;
           res->data=(char*)ssiReadPoly_R_S(s,R);
           break;
    case 10:
    case 22:if (t==22) res->rtyp=SMATRIX_CMD;
           else        res->rtyp=MODUL_CMD;
           {
             int rk=s_readint_S(s);
             ideal M=ssiReadIdeal_R_S(s,R);
             M->rank=rk;
             res->data=(char*)M;
           }
           break;
    #if 0
    case 11:
           {
             res->rtyp=COMMAND;
             res->data=ssiReadCommand(l);
             int nok=res->Eval();
             if (nok) WerrorS("error in eval");
             break;
           }
    #endif
    case 12: /*DEF_CMD*/
           {
             res->rtyp=0;
             res->name=(char *)ssiReadString_S(s);
             int nok=res->Eval();
             if (nok) WerrorS("error in name lookup");
             break;
           }
    case 13: res->rtyp=PROC_CMD;
             res->data=ssiReadProc_S(s);
             break;
    case 14: res->rtyp=LIST_CMD;
             res->data=ssiReadList_S(s,R);
             break;
    case 16: res->rtyp=NONE; res->data=NULL;
             break;
    case 17: res->rtyp=INTVEC_CMD;
             res->data=ssiReadIntvec_S(s);
             break;
    case 18: res->rtyp=INTMAT_CMD;
             res->data=ssiReadIntmat_S(s);
             break;
    case 19: res->rtyp=BIGINTMAT_CMD;
             res->data=ssiReadBigintmat_S(s);
             break;
    #if 0
    case 20: ssiReadBlackbox(res,l);
             break;
    case 21: ssiReadAttrib(res,l);
             break;
    case 23: ssiReadRingProperties(l);
             return ssiRead1(l);
             break;
    #endif
    case 24: res->rtyp=BIGINTVEC_CMD;
             res->data=ssiReadBigintvec_S(s);
             break;
    // ------------
    case 98: // version
             {
                int n98_v,n98_m;
                BITSET n98_o1,n98_o2;
                n98_v=s_readint_S(s);
                n98_m=s_readint_S(s);
                n98_o1=s_readint_S(s);
                n98_o2=s_readint_S(s);
                Print("// version ssi-%d, MAX_TOK=%d\n",n98_v,n98_m);
                si_opt_1=n98_o1;
                si_opt_2=n98_o2;
                omFreeBin(res,sleftv_bin);
                return ssiRead1_S(s,R);
             }
    #if 0
    case 99: omFreeBin(res,sleftv_bin); ssiClose(l); m2_end(-1);
             break; /*to make compiler happy*/
    #endif
    case 0: res->rtyp=DEF_CMD;
            **s='\0'; /* unkown char?*/
            break;
    default: Werror("not implemented (t:%d)",t);
             omFreeBin(res,sleftv_bin);
             res=NULL;
             break;
  }
  while((**s!='\0') &&(**s<=' ')) (*s)++;
  if (**s>' ') res->next=ssiRead1_S(s,R);
  return res;
}
//**************************************************************************/
static BOOLEAN ssiSetRing(si_link l, ring r, BOOLEAN send)
{
  if(SI_LINK_W_OPEN_P(l)==0)
     if (slOpen(l,SI_LINK_OPEN|SI_LINK_WRITE,NULL)) return TRUE;
  ssiInfo *d = (ssiInfo *)l->data;
  if (d->r!=r)
  {
    if (send)
    {
      fputs("15 ",d->f_write);
      ssiWriteRing(d,r);
    }
    d->r=r;
  }
  if (currRing!=r) rChangeCurrRing(r);
  return FALSE;
}
//**************************************************************************/

BOOLEAN ssiWrite(si_link l, leftv data)
{
  if(SI_LINK_W_OPEN_P(l)==0)
     if (slOpen(l,SI_LINK_OPEN|SI_LINK_WRITE,NULL)) return TRUE;
  if (strcmp(l->mode,"string")==0) return TRUE;
  ssiInfo *d = (ssiInfo *)l->data;
  d->level++;
  //FILE *fich=d->f;
  while (data!=NULL)
  {
    int tt=data->Typ();
    void *dd=data->Data();
    attr *aa=data->Attribute();
    if ((aa!=NULL) && ((*aa)!=NULL)) // n user attributes
    {
      attr a=*aa;
      int n=0;
      while(a!=NULL) { n++; a=a->next;}
      fprintf(d->f_write,"21 %d %d ",data->flag,n);
    }
    else if (data->flag!=0) // only "flag" attributes
    {
      fprintf(d->f_write,"21 %d 0 ",data->flag);
    }
    if ((dd==NULL) && (data->name!=NULL) && (tt==0)) tt=DEF_CMD;
      // return pure undefined names as def

    switch(tt /*data->Typ()*/)
    {
          case 0: /*error*/
          case NONE/* nothing*/:fputs("16 ",d->f_write);
                          break;
          case STRING_CMD: fputs("2 ",d->f_write);
                           ssiWriteString(d,(char *)dd);
                           break;
          case INT_CMD: fputs("1 ",d->f_write);
                        ssiWriteInt(d,(int)(long)dd);
                        break;
          case BIGINT_CMD:fputs("4 ",d->f_write);
                        ssiWriteBigInt(d,(number)dd);
                        break;
          case NUMBER_CMD:
                          if (d->r!=currRing)
                          {
                            fputs("15 ",d->f_write);
                            ssiWriteRing(d,currRing);
                            if (d->level<=1) fputc('\n',d->f_write);
                          }
                          fputs("3 ",d->f_write);
                          ssiWriteNumber(d,(number)dd);
                        break;
          case RING_CMD:fputs("5 ",d->f_write);
                        ssiWriteRing(d,(ring)dd);
                        break;
          case BUCKET_CMD:
                        {
                          sBucket_pt b=(sBucket_pt)dd;
                          if (d->r!=sBucketGetRing(b))
                          {
                            fputs("15 ",d->f_write);
                            ssiWriteRing(d,sBucketGetRing(b));
                            if (d->level<=1) fputc('\n',d->f_write);
                          }
                          fputs("6 ",d->f_write);
                          ssiWritePoly(d,sBucketPeek(b));
                          break;
                        }
          case POLY_CMD:
          case VECTOR_CMD:
                        if (d->r!=currRing)
                        {
                          fputs("15 ",d->f_write);
                          ssiWriteRing(d,currRing);
                          if (d->level<=1) fputc('\n',d->f_write);
                        }
                        if(tt==POLY_CMD) fputs("6 ",d->f_write);
                        else             fputs("9 ",d->f_write);
                        ssiWritePoly(d,(poly)dd);
                        break;
          case IDEAL_CMD:
          case MODUL_CMD:
          case MATRIX_CMD:
          case SMATRIX_CMD:
                        if (d->r!=currRing)
                        {
                          fputs("15 ",d->f_write);
                          ssiWriteRing(d,currRing);
                          if (d->level<=1) fputc('\n',d->f_write);
                        }
                        if(tt==IDEAL_CMD)       fputs("7 ",d->f_write);
                        else if(tt==MATRIX_CMD) fputs("8 ",d->f_write);
                        else /* tt==MODUL_CMD, SMATRIX_CMD*/
                        {
                          ideal M=(ideal)dd;
                          if (tt==MODUL_CMD)
                            fprintf(d->f_write,"10 %d ",(int)M->rank);
                          else /*(tt==SMATRIX_CMD)*/
                            fprintf(d->f_write,"22 %d ",(int)M->rank);
                        }
                        ssiWriteIdeal(d,tt,(ideal)dd);
                        break;
          case COMMAND:
                   fputs("11 ",d->f_write);
                   ssiWriteCommand(l,(command)dd);
                   break;
          case DEF_CMD: /* not evaluated stuff in quotes */
                   fputs("12 ",d->f_write);
                   ssiWriteString(d,data->Name());
                   break;
          case PROC_CMD:
                   fputs("13 ",d->f_write);
                   ssiWriteProc(d,(procinfov)dd);
                   break;
          case LIST_CMD:
                   fputs("14 ",d->f_write);
                   ssiWriteList(l,(lists)dd);
                   break;
          case INTVEC_CMD:
                   fputs("17 ",d->f_write);
                   ssiWriteIntvec(d,(intvec *)dd);
                   break;
          case INTMAT_CMD:
                   fputs("18 ",d->f_write);
                   ssiWriteIntmat(d,(intvec *)dd);
                   break;
          case BIGINTMAT_CMD:
                   fputs("19 ",d->f_write);
                   ssiWriteBigintmat(d,(bigintmat *)dd);
                   break;
          case BIGINTVEC_CMD:
                   fputs("24 ",d->f_write);
                   ssiWriteBigintvec(d,(bigintmat *)dd);
                   break;
          default:
            if (tt>MAX_TOK)
            {
              blackbox *b=getBlackboxStuff(tt);
              fputs("20 ",d->f_write);
              b->blackbox_serialize(b,dd,l);
            }
            else
            {
              Werror("not implemented (t:%d, rtyp:%d)",tt, data->rtyp);
              d->level=0;
              return TRUE;
            }
            break;
    }
    if (d->level<=1) { fputc('\n',d->f_write); fflush(d->f_write); }
    data=data->next;
  }
  d->level--;
  return FALSE;
}
void ssiWrite_S(leftv data,const ring R)
{
  while(data!=NULL)
  {
    int tt=data->Typ();
    void *dd=data->Data();
    switch(tt /*data->Typ()*/)
    {
      case 0: /*error*/
      case NONE/* nothing*/:StringAppendS("16 ");
           break;
      case INT_CMD: StringAppendS("1 ");
                          ssiWriteInt_S((int)(long)dd);
                          break;
      case STRING_CMD: StringAppendS("2 ");
                             ssiWriteString_S((char *)dd);
                             break;
      case BIGINT_CMD:StringAppendS("4 ");
                          ssiWriteBigInt_S((number)dd);
                          break;
      case NUMBER_CMD:
        StringAppendS("3 ");
        ssiWriteNumber_CF_S((number)dd,R->cf);
        break;
      case RING_CMD:StringAppendS("5 ");
                          ssiWriteRing_R_S((ring)dd);
                          break;
      case BUCKET_CMD:
                          {
                            sBucket_pt b=(sBucket_pt)dd;
                            StringAppendS("6 ");
                            ssiWritePoly_R_S(sBucketPeek(b),R);
                            break;
                          }
      case POLY_CMD:
      case VECTOR_CMD:
        if(tt==POLY_CMD) StringAppendS("6 ");
        else             StringAppendS("9 ");
        ssiWritePoly_R_S((poly)dd,R);
        break;
      case IDEAL_CMD:
      case MODUL_CMD:
      case MATRIX_CMD:
      case SMATRIX_CMD:
        if(tt==IDEAL_CMD)       StringAppendS("7 ");
        else if(tt==MATRIX_CMD) StringAppendS("8 ");
        else /* tt==MODUL_CMD, SMATRIX_CMD*/
        {
          ideal M=(ideal)dd;
          if (tt==MODUL_CMD)
          {
            StringAppendS("10 ");StringAppend("%d ",(int)M->rank);
          }
          else /*(tt==SMATRIX_CMD)*/
          {
            StringAppendS("22 ");StringAppend("%d ",(int)M->rank);
          }
        }
        ssiWriteIdeal_R_S(tt,(ideal)dd,R);
        break;
      #if 0
      case COMMAND:
                     fputs("11 ",d->f_write);
                     ssiWriteCommand(l,(command)dd);
                     break;
      #endif
      case DEF_CMD: /* not evaluated stuff in quotes */
                     StringAppendS("12 ");
                     ssiWriteString_S(data->Name());
                     break;
      case PROC_CMD:
                     StringAppendS("13 ");
                     ssiWriteProc_S((procinfov)dd);
                     break;
      case LIST_CMD:
                     StringAppendS("14 ");
                     ssiWriteList_S((lists)dd,R);
                     break;
      case INTVEC_CMD:
                     StringAppendS("17 ");
                     ssiWriteIntvec_S((intvec *)dd);
                     break;
      case INTMAT_CMD:
                     StringAppendS("18 ");
                     ssiWriteIntmat_S((intvec *)dd);
                     break;
      case BIGINTMAT_CMD:
                     StringAppendS("19 ");
                     ssiWriteBigintmat_S((bigintmat *)dd);
                     break;
      case BIGINTVEC_CMD:
                     StringAppendS("24 ");
                     ssiWriteBigintvec_S((bigintmat *)dd);
                     break;
      default:
              #if 0
              if (tt>MAX_TOK)
              {
                blackbox *b=getBlackboxStuff(tt);
                fputs("20 ",d->f_write);
                b->blackbox_serialize(b,dd,l);
              }
              else
              #endif
              {
                Werror("not implemented (t:%d, rtyp:%d)",tt, data->rtyp);
                return;
              }
              break;
    }
    data=data->next;
  }
}

BOOLEAN ssiGetDump(si_link l);
BOOLEAN ssiDump(si_link l);

si_link_extension slInitSsiExtension(si_link_extension s)
{
  s->Open=ssiOpen;
  s->Close=ssiClose;
  s->Kill=ssiClose;
  s->Read=ssiRead1;
  s->Read2=ssiRead2;
  s->Write=ssiWrite;
  s->Dump=ssiDump;
  s->GetDump=ssiGetDump;

  s->Status=slStatusSsi;
  s->SetRing=ssiSetRing;
  s->type="ssi";
  return s;
}

/* #ssi2 start */
static const char* slStatusSsi2(si_link l, const char* request)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (strcmp(request, "read")==0)
  {
    if (SI_LINK_R_OPEN_P(l) && (d!=NULL) && (d->f_read!=NULL) && (!s_iseof(d->f_read)))
      return "ready";
    return "not ready";
  }
  if (strcmp(request, "write")==0)
  {
    if (SI_LINK_W_OPEN_P(l)) return "ready";
    return "not ready";
  }
  return "unknown status request";
}

si_link_extension slInitSsi2Extension(si_link_extension s)
{
  s->Open=ssi2Open;
  s->Close=ssiClose;
  s->Kill=ssiClose;
  s->Read=ssi2Read1;
  s->Read2=NULL;
  s->Write=ssi2Write;
  s->Dump=NULL;
  s->GetDump=NULL;
  s->Status=slStatusSsi2;
  s->SetRing=NULL;
  s->type="ssi2";
  return s;
}

si_link_extension slInitSsi2zExtension(si_link_extension s)
{
  s->Open=ssi2zOpen;
  s->Close=ssi2zClose;
  s->Kill=ssi2zClose;
  s->Read=ssi2Read1;
  s->Read2=NULL;
  s->Write=ssi2Write;
  s->Dump=NULL;
  s->GetDump=NULL;
  s->Status=slStatusSsi2;
  s->SetRing=NULL;
  s->type="ssi2z";
  return s;
}

si_link_extension slInitSsi2zstdExtension(si_link_extension s)
{
  s->Open=ssi2zstdOpen;
  s->Close=ssi2zClose;
  s->Kill=ssi2zClose;
  s->Read=ssi2Read1;
  s->Read2=NULL;
  s->Write=ssi2Write;
  s->Dump=NULL;
  s->GetDump=NULL;
  s->Status=slStatusSsi2;
  s->SetRing=NULL;
  s->type="ssi2zstd";
  return s;
}

si_link_extension slInitSsi2lz4Extension(si_link_extension s)
{
  s->Open=ssi2lz4Open;
  s->Close=ssi2zClose;
  s->Kill=ssi2zClose;
  s->Read=ssi2Read1;
  s->Read2=NULL;
  s->Write=ssi2Write;
  s->Dump=NULL;
  s->GetDump=NULL;
  s->Status=slStatusSsi2;
  s->SetRing=NULL;
  s->type="ssi2lz4";
  return s;
}
/* #ssi2 end */

const char* slStatusSsi(si_link l, const char* request)
{
  if (strcmp(l->mode,"string")==0)
  {
    if (strcmp(request, "read") == 0)
    {
      if (SI_LINK_R_OPEN_P(l)) return "yes";
      else return "no";
    }
    if (strcmp(request, "write") == 0)
    {
      if (SI_LINK_W_OPEN_P(l)) return "yes";
      else return "no";
    }
    return "inavlid";
  }
  ssiInfo *d=(ssiInfo*)l->data;
  if (d==NULL)
    return "no";
  if (((strcmp(l->mode,"fork")==0)
  ||(strcmp(l->mode,"tcp")==0)
  ||(strcmp(l->mode,"connect")==0))
  && (strcmp(request, "read") == 0))
  {
    if (s_isready(d->f_read)) return "ready";
#if defined(HAVE_POLL)
    pollfd pfd;
    loop
    {
      /* Don't block. Return socket status immediately. */
      pfd.fd=d->fd_read;
      pfd.events=POLLIN;
      //Print("test fd %d\n",d->fd_read);
      /* check with select: chars waiting: no -> not ready */
      switch (si_poll(&pfd,1,0))
      {
        case 0: /* not ready */ return "not ready";
        case -1: /*error*/      return "error";
        case 1: /*ready ? */    break;
      }
#else
    fd_set  mask;
    struct timeval wt;
    if (FD_SETSIZE<=d->fd_read)
    {
      Werror("file descriptor number too high (%d)",d->fd_read);
      return "error";
    }

    loop
    {
      /* Don't block. Return socket status immediately. */
      wt.tv_sec  = 0;
      wt.tv_usec = 0;

      FD_ZERO(&mask);
      FD_SET(d->fd_read, &mask);
      //Print("test fd %d\n",d->fd_read);
      /* check with select: chars waiting: no -> not ready */
      switch (si_select(d->fd_read+1, &mask, NULL, NULL, &wt))
      {
        case 0: /* not ready */ return "not ready";
        case -1: /*error*/      return "error";
        case 1: /*ready ? */    break;
      }
#endif
      /* yes: read 1 char*/
      /* if \n, check again with select else ungetc(c), ready*/
      int c=s_getc(d->f_read);
      //Print("try c=%d\n",c);
      if (c== -1) return "eof"; /* eof or error */
      else if (isdigit(c))
      { s_ungetc(c,d->f_read); return "ready"; }
      else if (c>' ')
      {
        Werror("unknown char in ssiLink(%d)",c);
        return "error";
      }
      /* else: next char */
    }
  }
  else if (strcmp(request, "read") == 0)
  {
    if (SI_LINK_R_OPEN_P(l) && (!s_iseof(d->f_read)) && (s_isready(d->f_read))) return "ready";
    else return "not ready";
  }
  else if (strcmp(request, "write") == 0)
  {
    if (SI_LINK_W_OPEN_P(l)) return "ready";
    else return "not ready";
  }
  else return "unknown status request";
}

int slStatusSsiL(lists L, int timeout, BOOLEAN *ignore)
{
// input: L: a list with links of type
//           ssi-connect, ssi-fork, ssi-tcp, MPtcp-fork or MPtcp-launch.
//           Note: Not every entry in L must be set.
//        timeout: timeout for select in milli-seconds
//           or -1 for infinity
//           or 0 for polling
// returns: ERROR (via Werror): L has wrong elements or link not open
//           -2: error in L
//           -1: the read state of all links is eof
//           0:  timeout (or polling): none ready,
//           i>0: (at least) L[i] is ready
#if defined(HAVE_POLL) && !defined(__APPLE__)
// fd is restricted on OsX by ulimit "file descriptors" (256)
  si_link l;
  ssiInfo *d=NULL;
  int d_fd;
  int s;
  int nfd=L->nr+1;
  pollfd *pfd=(pollfd*)omAlloc0(nfd*sizeof(pollfd));
  for(int i=L->nr; i>=0; i--)
  {
    pfd[i].fd=-1;
    if (L->m[i].Typ()!=DEF_CMD)
    {
      if (L->m[i].Typ()!=LINK_CMD)
      { WerrorS("all elements must be of type link"); return -2;}
      l=(si_link)L->m[i].Data();
      if(SI_LINK_OPEN_P(l)==0)
      { WerrorS("all links must be open"); return -2;}
      if (((strcmp(l->m->type,"ssi")!=0) && (strcmp(l->m->type,"MPtcp")!=0))
      || ((strcmp(l->mode,"fork")!=0) && (strcmp(l->mode,"tcp")!=0)
        && (strcmp(l->mode,"launch")!=0) && (strcmp(l->mode,"connect")!=0)))
      {
        WerrorS("all links must be of type ssi:fork, ssi:tcp, ssi:connect");
        return -2;
      }
      if (strcmp(l->m->type,"ssi")==0)
      {
        d=(ssiInfo*)l->data;
        d_fd=d->fd_read;
        if (!s_isready(d->f_read))
        {
          pfd[i].fd=d_fd;
          pfd[i].events=POLLIN;
        }
        else
        {
          return i+1;
        }
      }
      else
      {
        Werror("wrong link type >>%s<<",l->m->type);
        return -2;
      }
    }
    else if (ignore!=NULL)
    {
      ignore[i]=TRUE; // not a link
    }
  }
  s=si_poll(pfd,nfd,timeout);
  if (s==-1)
  {
    Werror("error in poll call (errno:%d)",errno);
    return -2; /*error*/
  }
  if(s==0)
  {
    return 0; /*timeout*/
  }
  for(int i=L->nr; i>=0; i--)
  {
    if ((L->m[i].rtyp==LINK_CMD)
    && ((ignore==NULL)||(ignore[i]==FALSE)))
    {
      // the link type is ssi, that's already tested
      l=(si_link)L->m[i].Data();
      d=(ssiInfo*)l->data;
      d_fd=d->fd_read;
      if (pfd[i].fd==d_fd)
      {
        if (pfd[i].revents &POLLIN)
        {
          omFree(pfd);
          return i+1;
        }
      }
    }
  }
  // no ready
  return 0;
#else
  // fd is restricted to <=1024
  si_link l;
  ssiInfo *d=NULL;
  int d_fd;
  fd_set fdmask;
  FD_ZERO(&fdmask);
  int max_fd=0; /* 1 + max fd in fd_set */

  /* timeout */
  struct timeval wt;
  struct timeval *wt_ptr=&wt;
  int startingtime = getRTimer()/TIMER_RESOLUTION;  // in seconds
  if (timeout== -1)
  {
    wt_ptr=NULL;
  }
  else
  {
    wt.tv_sec  = timeout / 1000;
    wt.tv_usec = (timeout % 1000)*1000;
  }

  /* auxiliary variables */
  int i;
  int j;
  int k;
  int s;
  char fdmaskempty;

  /* check the links and fill in fdmask */
  /* check ssi links for ungetc_buf */
  for(i=L->nr; i>=0; i--)
  {
    if (L->m[i].Typ()!=DEF_CMD)
    {
      if (L->m[i].Typ()!=LINK_CMD)
      { WerrorS("all elements must be of type link"); return -2;}
      l=(si_link)L->m[i].Data();
      if(SI_LINK_OPEN_P(l)==0)
      { WerrorS("all links must be open"); return -2;}
      if (((strcmp(l->m->type,"ssi")!=0) && (strcmp(l->m->type,"MPtcp")!=0))
      || ((strcmp(l->mode,"fork")!=0) && (strcmp(l->mode,"tcp")!=0)
        && (strcmp(l->mode,"launch")!=0) && (strcmp(l->mode,"connect")!=0)))
      {
        WerrorS("all links must be of type ssi:fork, ssi:tcp, ssi:connect");
        return -2;
      }
      if (strcmp(l->m->type,"ssi")==0)
      {
        d=(ssiInfo*)l->data;
        d_fd=d->fd_read;
        if (!s_isready(d->f_read))
        {
          if ((ignore==NULL) || (ignore[i]==FALSE))
          {
            FD_SET(d_fd, &fdmask);
            if (d_fd > max_fd) max_fd=d_fd;
          }
        }
        else
          return i+1;
      }
      else
      {
        Werror("wrong link type >>%s<<",l->m->type);
        return -2;
      }
    }
  }
  max_fd++;
  if (FD_SETSIZE<=max_fd)
  {
    Werror("file descriptor number too high (%d)",max_fd);
    return -2;
  }

  /* check with select: chars waiting: no -> not ready */
  s = si_select(max_fd, &fdmask, NULL, NULL, wt_ptr);
  if (s==-1)
  {
    Werror("error in select call (errno:%d)",errno);
    return -2; /*error*/
  }
  if (s==0)
  {
    return 0; /*poll: not ready */
  }
  else /* s>0, at least one ready  (the number of fd which are ready is s)*/
  {
    j=0;
    while (j<=max_fd) { if (FD_ISSET(j,&fdmask)) break; j++; }
    for(i=L->nr; i>=0; i--)
    {
      if (L->m[i].rtyp==LINK_CMD)
      {
        l=(si_link)L->m[i].Data();
        if (strcmp(l->m->type,"ssi")==0)
        {
          d=(ssiInfo*)l->data;
          d_fd=d->fd_read;
          if(j==d_fd) return i+1;
        }
      }
    }
  }
  return 0;
#endif
}

int ssiBatch(const char *host, const char * port)
/* return 0 on success, >0 else*/
{
  si_link l=(si_link) omAlloc0Bin(sip_link_bin);
  char *buf=(char*)omAlloc(256);
  snprintf(buf,256,"ssi:connect %s:%s",host,port);
  slInit(l, buf);
  omFreeSize(buf,256);
  if (slOpen(l,SI_LINK_OPEN,NULL)) return 1;
  SI_LINK_SET_RW_OPEN_P(l);

  idhdl id = enterid("link_ll", 0, LINK_CMD, &IDROOT, FALSE);
  IDLINK(id) = l;

  loop
  {
    leftv h=ssiRead1(l); /*contains an exit.... */
    if (feErrors != NULL && *feErrors != '\0')
    {
      // handle errors:
      PrintS(feErrors); /* currently quite simple */
      *feErrors = '\0';
    }
    ssiWrite(l,h);
    h->CleanUp();
    omFreeBin(h, sleftv_bin);
  }
  /* never reached*/
  _exit(0);
}

STATIC_VAR int ssiReserved_P=0;
STATIC_VAR int ssiReserved_sockfd;
STATIC_VAR struct sockaddr_in ssiResverd_serv_addr;
STATIC_VAR int  ssiReserved_Clients;
int ssiReservePort(int clients)
{
  if (ssiReserved_P!=0)
  {
    WerrorS("ERROR already a reserved port requested");
    return 0;
  }
  int portno;
  ssiReserved_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(ssiReserved_sockfd < 0)
  {
    WerrorS("ERROR opening socket");
    return 0;
  }
  memset((char *) &ssiResverd_serv_addr,0, sizeof(ssiResverd_serv_addr));
  portno = 1025;
  ssiResverd_serv_addr.sin_family = AF_INET;
  ssiResverd_serv_addr.sin_addr.s_addr = INADDR_ANY;
  do
  {
    portno++;
    ssiResverd_serv_addr.sin_port = htons(portno);
    if(portno > 50000)
    {
      WerrorS("ERROR on binding (no free port available?)");
      return 0;
    }
  }
  while(bind(ssiReserved_sockfd, (struct sockaddr *) &ssiResverd_serv_addr, sizeof(ssiResverd_serv_addr)) < 0);
  ssiReserved_P=portno;
  listen(ssiReserved_sockfd,clients);
  ssiReserved_Clients=clients;
  return portno;
}

EXTERN_VAR si_link_extension si_link_root;
si_link ssiCommandLink()
{
  if (ssiReserved_P==0)
  {
    WerrorS("ERROR no reserved port requested");
    return NULL;
  }
  struct sockaddr_in cli_addr;
  int clilen = sizeof(cli_addr);
  int newsockfd = si_accept(ssiReserved_sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
  if(newsockfd < 0)
  {
    Werror("ERROR on accept (errno=%d)",errno);
    return NULL;
  }
  si_link l=(si_link) omAlloc0Bin(sip_link_bin);
  si_link_extension s = si_link_root;
  si_link_extension prev = s;
  while (strcmp(s->type, "ssi") != 0)
  {
    if (s->next == NULL)
    {
      prev = s;
      s = NULL;
      break;
    }
    else
    {
      s = s->next;
    }
  }
  if (s != NULL)
    l->m = s;
  else
  {
    si_link_extension ns = (si_link_extension)omAlloc0Bin(s_si_link_extension_bin);
    prev->next=slInitSsiExtension(ns);
    l->m = prev->next;
  }
  l->name=omStrDup("");
  l->mode=omStrDup("tcp");
  l->ref=1;
  ssiInfo *d=(ssiInfo*)omAlloc0(sizeof(ssiInfo));
  l->data=d;
  ssiInitSchemaVersions(d);
  d->fd_read = newsockfd;
  d->fd_write = newsockfd;
  d->f_read = s_open(newsockfd);
  d->f_write = fdopen(newsockfd, "w");
  SI_LINK_SET_RW_OPEN_P(l);
  ssiReserved_Clients--;
  if (ssiReserved_Clients<=0)
  {
    ssiReserved_P=0;
    si_close(ssiReserved_sockfd);
  }
  return l;
}
/*---------------------------------------------------------------------*/
/**
 * @brief additional default signal handler

  // some newer Linux version cannot have SIG_IGN for SIGCHLD,
  // so use this nice routine here:
  //  SuSe 9.x reports -1 always
  //  Redhat 9.x/FC x reports sometimes -1
  // see also: hpux_system
  // also needed by getrusage (timer etc.)

 @param[in] sig
**/
/*---------------------------------------------------------------------*/
void sig_chld_hdl(int)
{

#if 0
  pid_t kidpid;
  int status;
  loop
  {
    kidpid = si_waitpid(-1, &status, WNOHANG);
    if (kidpid==-1)
    {
      /* continue on interruption (EINTR): */
      if (errno == EINTR) continue;
      /* break on anything else (EINVAL or ECHILD according to manpage): */
      break;
    }
    else if (kidpid==0) break; /* no more children to process, so break */

    //printf("Child %ld terminated\n", kidpid);
    link_list hh=ssiToBeClosed;
    while((hh!=NULL)&&(ssiToBeClosed_inactive))
    {
      if((hh->l!=NULL) && (hh->l->m->Open==ssiOpen))
      {
        ssiInfo *d = (ssiInfo *)hh->l->data;
        if(d->pid==kidpid)
        {
          if(ssiToBeClosed_inactive)
          {
            ssiToBeClosed_inactive=FALSE;
            slClose(hh->l);
            ssiToBeClosed_inactive=TRUE;
            break;
          }
          else break;
        }
        else hh=(link_list)hh->next;
      }
      else hh=(link_list)hh->next;
    }
  }
#endif
}

static BOOLEAN DumpSsiIdhdl(si_link l, idhdl h)
{
  int type_id = IDTYP(h);

  // C-proc not to be dumped, also LIB-proc not
  if (type_id == PROC_CMD)
  {
    if (IDPROC(h)->language == LANG_C) return FALSE;
    if (IDPROC(h)->libname != NULL) return FALSE;
  }
  // do not dump links
  if (type_id == LINK_CMD) return FALSE;

  // do not dump ssi internal rings: ssiRing*
  if ((type_id == RING_CMD) && (strncmp(IDID(h),"ssiRing",7)==0))
    return FALSE;

  // do not dump default cring:
  if (type_id == CRING_CMD)
  {
    if (strcmp(IDID(h),"ZZ")==0) return FALSE;
    if (strcmp(IDID(h),"QQ")==0) return FALSE;
    #ifdef SINGULAR_4_2
    if (strcmp(IDID(h),"AE")==0) return FALSE;
    if (strcmp(IDID(h),"QAE")==0) return FALSE;
    #endif
  }

  command D=(command)omAlloc0(sizeof(*D));
  sleftv tmp;
  memset(&tmp,0,sizeof(tmp));
  tmp.rtyp=COMMAND;
  tmp.data=D;

  if (type_id == PACKAGE_CMD)
  {
    // do not dump Top, Standard
    if ((strcmp(IDID(h), "Top") == 0)
    || (strcmp(IDID(h), "Standard") == 0))
    {
      omFreeSize(D,sizeof(*D));
      return FALSE;
    }
    package p=(package)IDDATA(h);
    // dump Singular-packages as LIB("...");
    if (p->language==LANG_SINGULAR)
    {
      D->op=LOAD_CMD;
      D->argc=2;
      D->arg1.rtyp=STRING_CMD;
      D->arg1.data=p->libname;
      D->arg2.rtyp=STRING_CMD;
      D->arg2.data=(char*)"with";
      ssiWrite(l,&tmp);
      omFreeSize(D,sizeof(*D));
      return FALSE;
    }
    // dump Singular-packages as load("...");
    else if (p->language==LANG_C)
    {
      D->op=LOAD_CMD;
      D->argc=1;
      D->arg1.rtyp=STRING_CMD;
      D->arg1.data=p->libname;
      ssiWrite(l,&tmp);
      omFreeSize(D,sizeof(*D));
      return FALSE;
    }
  }

  // put type and name
  //Print("generic dump:%s,%s\n",IDID(h),Tok2Cmdname(IDTYP(h)));
  D->op='=';
  D->argc=2;
  D->arg1.rtyp=DEF_CMD;
  D->arg1.name=IDID(h);
  D->arg2.rtyp=IDTYP(h);
  D->arg2.data=IDDATA(h);
  ssiWrite(l,&tmp);
  omFreeSize(D,sizeof(*D));
  return FALSE;
}
static BOOLEAN ssiDumpIter(si_link l, idhdl h)
{
  if (h == NULL) return FALSE;

  if (ssiDumpIter(l, IDNEXT(h))) return TRUE;

  // need to set the ring before writing it, otherwise we get in
  // trouble with minpoly
  if (IDTYP(h) == RING_CMD)
    rSetHdl(h);

  if (DumpSsiIdhdl(l, h)) return TRUE;

  // do not dump ssi internal rings: ssiRing*
  // but dump objects of all other rings
  if ((IDTYP(h) == RING_CMD)
  && (strncmp(IDID(h),"ssiRing",7)!=0))
    return ssiDumpIter(l, IDRING(h)->idroot);
  else
    return FALSE;
}
BOOLEAN ssiDump(si_link l)
{
  if (strcmp(l->mode,"string")==0)
  {
    WerrorS("no dump for ssi:string");
    return TRUE;
  }
  idhdl h = IDROOT, rh = currRingHdl;
  BOOLEAN status = ssiDumpIter(l, h);

  //if (! status ) status = DumpAsciiMaps(fd, h, NULL);

  if (currRingHdl != rh) rSetHdl(rh);
  //fprintf(fd, "option(set, intvec(%d, %d));\n", si_opt_1, si_opt_2);

  return status;
}
BOOLEAN ssiGetDump(si_link l)
{
  if (strcmp(l->mode,"string")==0)
  {
    WerrorS("no dump for ssi:string");
    return TRUE;
  }
  ssiInfo *d=(ssiInfo*)l->data;
  loop
  {
    if (!SI_LINK_OPEN_P(l)) break;
    if (s_iseof(d->f_read)) break;
    leftv h=ssiRead1(l); /*contains an exit.... */
    if (feErrors != NULL && *feErrors != '\0')
    {
      // handle errors:
      PrintS(feErrors); /* currently quite simple */
      return TRUE;
      *feErrors = '\0';
    }
    h->CleanUp();
    omFreeBin(h, sleftv_bin);
  }
  return FALSE;
}

void singular_close_links()
{
  link_list hh=ssiToBeClosed;
  while(hh!=NULL)
  {
    if ((hh->l->m!=NULL)
    && (hh->l->m->Open==ssiOpen)
    && SI_LINK_OPEN_P(hh->l)
    && (strcmp(hh->l->mode, "fork")==0))
    {
      SI_LINK_SET_CLOSE_P(hh->l);
      ssiInfo *d = (ssiInfo *)hh->l->data;
      if (d->f_read!=NULL) { s_close(d->f_read);d->f_read=NULL;}
      if (d->f_write!=NULL) { fclose(d->f_write); d->f_write=NULL; }
    }
    hh=(link_list)hh->next;
  }
  ssiToBeClosed=NULL;
}

BOOLEAN ssiWrite2(si_link l, leftv res, leftv u)
{
  if((strcmp(l->mode,"string")==0)
  &&(u->Typ()==STRING_CMD))
  {
    StringSetS("");
    ssiWrite_S(u, currRing);
    res->data=(void*)StringEndS();
    res->rtyp=STRING_CMD;
    return res->data==NULL;
  }
  return TRUE;
}
leftv ssiRead2(si_link l, leftv u)
{
  if((strcmp(l->mode,"string")==0)
  &&(u->Typ()==STRING_CMD))
  {
    char *s=(char*)u->Data();
    return ssiRead1_S(&s,currRing);
  }
 return NULL;
}
// ----------------------------------------------------------------
// format
// 1 int %d
// 2 string <len> %s
// 3 number
// 4 bigint 4 %d or 3 <mpz_t>
// 5 ring
// 6 poly
// 7 ideal
// 8 matrix
// 9 vector
// 10 module
// 11 command
// 12 def <len> %s
// 13 proc <len> %s
// 14 list %d <elem1> ....
// 15 setring .......
// 16 nothing
// 17 intvec <len> ...
// 18 intmat
// 19 bigintmat <r> <c> ...
// 20 blackbox <name> <len> ...
// 21 attrib <bit-attrib> <len> <a-name1> <val1>... <data>
// 22 smatrix
// 23 0 <log(bitmask)> ring properties: max.exp.
// 23 1 <log(bitmask)> <r->IsLPRing> ring properties:LPRing
// 23 2 <matrix C> <matrix D> ring properties: PLuralRing
// 24 bigintvec <c>
// 25 schema table: <table-version> <count> (<schema-id> <schema-version>)...
//
// 98: verify version: <ssi-version> <MAX_TOK> <OPT1> <OPT2>
// 99: quit Singular
