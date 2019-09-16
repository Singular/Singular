/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - the mapping of polynomials to other rings
*/
#define TRANSEXT_PRIVATES

#include "kernel/mod2.h"

#include "coeffs/numbers.h"
#include "coeffs/coeffs.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/maps.h"
#include "polys/matpol.h"
#include "polys/prCopy.h"
#include "polys/ext_fields/transext.h"

//#include "polys/ext_fields/longtrans.h"
// #include "kernel/longalg.h"

#include "misc/options.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/maps/gen_maps.h"

#include "maps_ip.h"
#include "ipid.h"


#include "lists.h"
#include "tok.h"

/* debug output: Tok2Cmdname in maApplyFetch*/
#include "ipshell.h"

/*2
* maps the expression w to res,
* switch what: MAP_CMD: use theMap for mapping, N for preimage ring
*              //FETCH_CMD: use pOrdPoly for mapping
*              IMAP_CMD: use perm for mapping, N for preimage ring
*              default: map only poly-structures,
*                       use perm and par_perm, N and P,
*/
BOOLEAN maApplyFetch(int what,map theMap,leftv res, leftv w, ring preimage_r,
                     int *perm, int *par_perm, int P, nMapFunc nMap)
{
  BOOLEAN use_mult=FALSE;
#ifdef HAVE_PLURAL
  if ((what==IMAP_CMD)
  && rIsPluralRing(currRing)
  && rIsPluralRing(preimage_r))
  {
    assume(perm!=NULL);
    int i=1;
    while((i<currRing->N)&&(perm[i]==0)) i++;
    if (i<currRing->N)
    {
      int prev_nonnull=i;
      i++;
      for(;i<=currRing->N;i++)
      {
        if (perm[prev_nonnull] > perm[i])
        {
          if (TEST_V_ALLWARN)
          {
            Warn("imap not usable for permuting variables, use map (%s <-> %s)",currRing->names[prev_nonnull-1],currRing->names[i-1]);
          }
          use_mult=TRUE;
          break;
        }
        else
          prev_nonnull=i;
      }
    }
  }
#endif
  int i;
  int N = preimage_r->N;
#if 0
  Print("N=%d what=%s ",N,Tok2Cmdname(what));
  if (perm!=NULL) for(i=1;i<=N;i++) Print("%d -> %d ",i,perm[i]);
  PrintS("\n");
  Print("P=%d ",P);
  if (par_perm!=NULL) for(i=0;i<P;i++) Print("%d -> %d ",i,par_perm[i]);
  PrintS("\n");
#endif

  void *data=w->Data();
  res->rtyp = w->rtyp;
  switch (w->rtyp)
  {
    case NUMBER_CMD:
      if (P!=0)
      {
// poly n_PermNumber(const number z, const int *par_perm, const int OldPar, const ring src, const ring dst);
        res->data= (void *) n_PermNumber((number)data, par_perm, P, preimage_r, currRing);
        res->rtyp=POLY_CMD;
        if (nCoeff_is_algExt(currRing->cf))
          res->data=(void *)p_MinPolyNormalize((poly)res->data, currRing);
        pTest((poly) res->data);
      }
      else
      {
        assume( nMap != NULL );
        number a = nMap((number)data, preimage_r->cf, currRing->cf);
        if (nCoeff_is_Extension(currRing->cf))
        {
          n_Normalize(a, currRing->cf);
/*
          number a = (number)res->data;
          number one = nInit(1);
          number product = nMult(a, one );
          nDelete(&one);
          nDelete(&a);
          res->data=(void *)product;
 */
        }
        #ifdef LDEBUG
        n_Test(a, currRing->cf);
        #endif
        res->data=(void *)a;

      }
      break;
    case BUCKET_CMD:
      if ((what==FETCH_CMD)&& (preimage_r->cf==currRing->cf))
        res->data=(void *)prCopyR(sBucketPeek((sBucket_pt)data), preimage_r, currRing);
      else
        if ( (what==IMAP_CMD) || /*(*/ (what==FETCH_CMD) /*)*/) /* && (nMap!=nCopy)*/
        res->data=(void *)p_PermPoly(sBucketPeek((sBucket_pt)data),perm,preimage_r,currRing, nMap,par_perm,P,use_mult);
      else /*if (what==MAP_CMD)*/
      {
        res->data=(void*)maMapPoly(sBucketPeek((sBucket_pt)data),preimage_r,(ideal)theMap,currRing,nMap);
      }
      if (nCoeff_is_Extension(currRing->cf))
        res->data=(void *)p_MinPolyNormalize(sBucketPeek((sBucket_pt)data), currRing);
      break;
    case POLY_CMD:
    case VECTOR_CMD:
      if ((what==FETCH_CMD)&& (preimage_r->cf==currRing->cf))
        res->data=(void *)prCopyR( (poly)data, preimage_r, currRing);
      else
        if ( (what==IMAP_CMD) || /*(*/ (what==FETCH_CMD) /*)*/) /* && (nMap!=nCopy)*/
        res->data=(void *)p_PermPoly((poly)data,perm,preimage_r,currRing, nMap,par_perm,P,use_mult);
      else /*if (what==MAP_CMD)*/
      {
        p_Test((poly)data,preimage_r);
        res->data=(void*)maMapPoly((poly)data,preimage_r,(ideal)theMap,currRing,nMap);
      }
      if (nCoeff_is_Extension(currRing->cf))
        res->data=(void *)p_MinPolyNormalize((poly)res->data, currRing);
      pTest((poly)res->data);
      break;
    case MODUL_CMD:
    case MATRIX_CMD:
    case IDEAL_CMD:
    case MAP_CMD:
    {
      int C=((matrix)data)->cols();
      int R;
      if (w->rtyp==MAP_CMD) R=1;
      else R=((matrix)data)->rows();
      matrix m=mpNew(R,C);
      char *tmpR=NULL;
      if(w->rtyp==MAP_CMD)
      {
        tmpR=((map)data)->preimage;
        ((matrix)data)->rank=((matrix)data)->rows();
      }
      if ((what==FETCH_CMD)&& (preimage_r->cf == currRing->cf))
      {
        for (i=R*C-1;i>=0;i--)
        {
          m->m[i]=prCopyR(((ideal)data)->m[i], preimage_r, currRing);
          pTest(m->m[i]);
        }
      }
      else if ((what==IMAP_CMD) || (what==FETCH_CMD))
      {
        for (i=R*C-1;i>=0;i--)
        {
          m->m[i]=p_PermPoly(((ideal)data)->m[i],perm,preimage_r,currRing,
                          nMap,par_perm,P,use_mult);
          pTest(m->m[i]);
        }
      }
      else /* (what==MAP_CMD) */
      {
        assume(what==MAP_CMD);
        matrix s=mpNew(N,maMaxDeg_Ma((ideal)data,preimage_r));
        for (i=R*C-1;i>=0;i--)
        {
          m->m[i]=maEval(theMap, ((ideal)data)->m[i], preimage_r, nMap, (ideal)s, currRing);
          pTest(m->m[i]);
        }
        idDelete((ideal *)&s);
      }
      if (nCoeff_is_algExt(currRing->cf))
      {
        for (i=R*C-1;i>=0;i--)
        {
          m->m[i]=p_MinPolyNormalize(m->m[i], currRing);
          pTest(m->m[i]);
        }
      }
      if(w->rtyp==MAP_CMD)
      {
        ((map)data)->preimage=tmpR;
        ((map)m)->preimage=omStrDup(tmpR);
      }
      else
      {
        m->rank=((matrix)data)->rank;
      }
      res->data=(char *)m;
      idTest((ideal) m);
      break;
    }

    case LIST_CMD:
    {
      lists l=(lists)data;
      lists ml=(lists)omAllocBin(slists_bin);
      ml->Init(l->nr+1);
      for(i=0;i<=l->nr;i++)
      {
        if (((l->m[i].rtyp>BEGIN_RING)&&(l->m[i].rtyp<END_RING))
        ||(l->m[i].rtyp==LIST_CMD))
        {
          if (maApplyFetch(what,theMap,&ml->m[i],&l->m[i],
                           preimage_r,perm,par_perm,P,nMap))
          {
            ml->Clean();
            omFreeBin((ADDRESS)ml, slists_bin);
            res->rtyp=0;
            return TRUE;
          }
        }
        else
        {
          ml->m[i].Copy(&l->m[i]);
        }
      }
      res->data=(char *)ml;
      break;
    }
    default:
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*2
* substitutes the parameter par (from 1..N) by image,
* does not destroy p and  image
*/
poly pSubstPar(poly p, int par, poly image)
{
  const ring R = currRing->cf->extRing;
  ideal theMapI = idInit(rPar(currRing),1);
  nMapFunc nMap = n_SetMap(R->cf, currRing->cf);
  int i;
  for(i = rPar(currRing);i>0;i--)
  {
    if (i != par)
      theMapI->m[i-1]= p_NSet(n_Param(i, currRing), currRing);
    else
      theMapI->m[i-1] = p_Copy(image, currRing);
    p_Test(theMapI->m[i-1],currRing);
  }
  //iiWriteMatrix((matrix)theMapI,"map:",1,currRing,0);

  map theMap=(map)theMapI;
  theMap->preimage=NULL;

  leftv v=(leftv)omAllocBin(sleftv_bin);
  sleftv tmpW;
  poly res=NULL;

  p_Normalize(p,currRing);
  if (currRing->cf->rep==n_rep_rat_fct )
  {
    while (p!=NULL)
    {
      memset(v,0,sizeof(sleftv));

      number d = n_GetDenom(pGetCoeff(p), currRing->cf);
      p_Test((poly)NUM((fraction)d), R);

      if ( n_IsOne (d, currRing->cf) )
      {
        n_Delete(&d, currRing->cf); d = NULL;
      }
      else if (!p_IsConstant((poly)NUM((fraction)d), R))
      {
        WarnS("ignoring denominators of coefficients...");
        n_Delete(&d, currRing->cf); d = NULL;
      }

      number num = n_GetNumerator(pGetCoeff(p), currRing->cf);
      memset(&tmpW,0,sizeof(sleftv));
      tmpW.rtyp = POLY_CMD;
      p_Test((poly)NUM((fraction)num), R);

      tmpW.data = NUM ((fraction)num); // a copy of this poly will be used

      p_Normalize(NUM((fraction)num),R);
      if (maApplyFetch(MAP_CMD,theMap,v,&tmpW,R,NULL,NULL,0,nMap))
      {
        WerrorS("map failed");
        v->data=NULL;
      }
      n_Delete(&num, currRing->cf);
      //TODO check for memory leaks
      poly pp = pHead(p);
      //PrintS("map:");pWrite(pp);
      if( d != NULL )
      {
        pSetCoeff(pp, n_Invers(d, currRing->cf));
        n_Delete(&d, currRing->cf); // d = NULL;
      }
      else
        pSetCoeff(pp, nInit(1));

      //PrintS("->");pWrite((poly)(v->data));
      poly ppp = pMult((poly)(v->data),pp);
      //PrintS("->");pWrite(ppp);
      res=pAdd(res,ppp);
      pIter(p);
    }
  }
  else if (currRing->cf->rep==n_rep_poly )
  {
    while (p!=NULL)
    {
      memset(v,0,sizeof(sleftv));

      number num = n_GetNumerator(pGetCoeff(p), currRing->cf);
      memset(&tmpW,0,sizeof(sleftv));
      tmpW.rtyp = POLY_CMD;
      p_Test((poly)num, R);


      p_Normalize((poly)num,R);
      if (num==NULL) num=(number)R->qideal->m[0];
      tmpW.data = num; // a copy of this poly will be used
      if (maApplyFetch(MAP_CMD,theMap,v,&tmpW,R,NULL,NULL,0,nMap))
      {
        WerrorS("map failed");
        v->data=NULL;
      }
      if (num!=(number)R->qideal->m[0]) n_Delete(&num, currRing->cf);
      //TODO check for memory leaks
      poly pp = pHead(p);
      //PrintS("map:");pWrite(pp);
      pSetCoeff(pp,n_Init(1,currRing->cf));
      //PrintS("cf->");pWrite((poly)(v->data));
      poly ppp = pMult((poly)(v->data),pp);
      //PrintS("->");pWrite(ppp);
      res=pAdd(res,ppp);
      pIter(p);
    }
  }
  else
  {
    WerrorS("cannot apply subst for these coeffcients");
  }
  idDelete((ideal *)(&theMap));
  omFreeBin((ADDRESS)v, sleftv_bin);
  return res;
}

/*2
* substitute the n-th parameter by the poly e in id
* does not destroy id and e
*/
ideal  idSubstPar(ideal id, int n, poly e)
{
  int k=MATROWS((matrix)id)*MATCOLS((matrix)id);
  ideal res=(ideal)mpNew(MATROWS((matrix)id),MATCOLS((matrix)id));

  res->rank = id->rank;
  for(k--;k>=0;k--)
  {
    res->m[k]=pSubstPar(id->m[k],n,e);
  }
  return res;
}

/*2
* substitutes the variable var (from 1..N) by image,
* does not destroy p and  image
*/
poly pSubstPoly(poly p, int var, poly image)
{
  if (p==NULL) return NULL;
#ifdef HAVE_PLURAL
  if (rIsPluralRing(currRing))
  {
    return pSubst(pCopy(p),var,image);
  }
#endif
#ifdef HAVE_SHIFTBBA
  if (rIsLPRing(currRing))
  {
    return pSubst(pCopy(p),var,image);
  }
#endif
  return p_SubstPoly(p,var,image,currRing,currRing,ndCopyMap);
}

/*2
* substitute the n-th variable by the poly e in id
* does not destroy id and e
*/
ideal  idSubstPoly(ideal id, int n, poly e)
{

#ifdef HAVE_PLURAL
  if (rIsPluralRing(currRing))
  {
    int k=MATROWS((matrix)id)*MATCOLS((matrix)id);
    ideal res=(ideal)mpNew(MATROWS((matrix)id),MATCOLS((matrix)id));
    res->rank = id->rank;
    for(k--;k>=0;k--)
    {
      res->m[k]=pSubst(pCopy(id->m[k]),n,e);
    }
    return res;
  }
#endif
#ifdef HAVE_SHIFTBBA
  if (rIsLPRing(currRing))
  {
    int k=MATROWS((matrix)id)*MATCOLS((matrix)id);
    ideal res=(ideal)mpNew(MATROWS((matrix)id),MATCOLS((matrix)id));
    res->rank = id->rank;
    for(k--;k>=0;k--)
    {
      res->m[k]=pSubst(pCopy(id->m[k]),n,e);
    }
    return res;
  }
#endif
  return id_SubstPoly(id,n,e,currRing,currRing,ndCopyMap);
}
