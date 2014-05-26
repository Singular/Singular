/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - the mapping of polynomials to other rings
*/
#define TRANSEXT_PRIVATES




#include <kernel/mod2.h>
#include <omalloc/omalloc.h>

#include <coeffs/numbers.h>
#include <coeffs/coeffs.h>

#include <polys/monomials/ring.h>
#include <polys/monomials/maps.h>
#include <polys/matpol.h>
#include <polys/prCopy.h>
#include <polys/ext_fields/transext.h>

//#include <libpolys/polys/ext_fields/longtrans.h>
// #include <kernel/longalg.h>

#include <kernel/GBEngine/kstd1.h>

#include "maps_ip.h"
#include "ipid.h"


#include "lists.h"
#include "tok.h"

/* debug output: Tok2Cmdname in maApplyFetch*/
#include "ipshell.h"

// define this if you want to use the fast_map routine for mapping ideals
//#define FAST_MAP

#ifdef FAST_MAP
#include <polys/monomials/maps.h>
#endif


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
//        WerrorS("Sorry 'napPermNumber' was lost in the refactoring process (due to Frank): needs to be fixed");
//        return TRUE;
#if 1
// poly n_PermNumber(const number z, const int *par_perm, const int OldPar, const ring src, const ring dst);
        res->data= (void *) n_PermNumber((number)data, par_perm, P, preimage_r, currRing);
#endif
        res->rtyp=POLY_CMD;
        if (nCoeff_is_Extension(currRing->cf))
          res->data=(void *)p_MinPolyNormalize((poly)res->data, currRing);
        pTest((poly) res->data);
      }
      else
      {
        assume( nMap != NULL );

        number a = nMap((number)data, preimage_r->cf, currRing->cf);


        if (nCoeff_is_Extension(currRing->cf))
        {
          n_Normalize(a, currRing->cf); // ???
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
    case POLY_CMD:
    case VECTOR_CMD:
      if ((what==FETCH_CMD)&& (preimage_r->cf==currRing->cf))
        res->data=(void *)prCopyR( (poly)data, preimage_r, currRing);
      else
	if ( (what==IMAP_CMD) || /*(*/ (what==FETCH_CMD) /*)*/) /* && (nMap!=nCopy)*/
        res->data=(void *)p_PermPoly((poly)data,perm,preimage_r,currRing, nMap,par_perm,P);
      else /*if (what==MAP_CMD)*/
      {
        p_Test((poly)data,preimage_r);
        matrix s=mpNew(N,maMaxDeg_P((poly)data, preimage_r));
        res->data=(void *)maEval(theMap, (poly)data, preimage_r, nMap, (ideal)s, currRing);
        idDelete((ideal *)&s);
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
      else
	if ( (what==IMAP_CMD) || /*(*/ (what==FETCH_CMD) /*)*/) /* && (nMap!=nCopy)*/
      {
        for (i=R*C-1;i>=0;i--)
        {
          m->m[i]=p_PermPoly(((ideal)data)->m[i],perm,preimage_r,currRing,
                          nMap,par_perm,P);
          pTest(m->m[i]);
        }
      }
      else /* if(what==MAP_CMD) */
      {
        matrix s=mpNew(N,maMaxDeg_Ma((ideal)data,preimage_r));
        for (i=R*C-1;i>=0;i--)
        {
          m->m[i]=maEval(theMap, ((ideal)data)->m[i], preimage_r, nMap, (ideal)s, currRing);
          pTest(m->m[i]);
        }
        idDelete((ideal *)&s);
      }
      if (nCoeff_is_Extension(currRing->cf))
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
  assume( nCoeff_is_transExt(currRing->cf) ); // nCoeff_is_Extension???
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
  while (p!=NULL)
  {
    memset(v,0,sizeof(sleftv));

    number d = n_GetDenom(p_GetCoeff(p, currRing), currRing);
    assume( p_Test((poly)NUM(d), R) );

    if ( n_IsOne (d, currRing->cf) )
    {
      n_Delete(&d, currRing); d = NULL;
    }
    else if (!p_IsConstant((poly)NUM(d), R))
    {
      WarnS("ignoring denominators of coefficients...");
      n_Delete(&d, currRing); d = NULL;
    }

    number num = n_GetNumerator(p_GetCoeff(p, currRing), currRing);
    assume( p_Test((poly)NUM(num), R) );

    memset(&tmpW,0,sizeof(sleftv));
    tmpW.rtyp = POLY_CMD;
    tmpW.data = NUM (num); // a copy of this poly will be used

    p_Normalize(NUM(num),R);
    if (maApplyFetch(MAP_CMD,theMap,v,&tmpW,R,NULL,NULL,0,nMap))
    {
      WerrorS("map failed");
      v->data=NULL;
    }
    n_Delete(&num, currRing);

    //TODO check for memory leaks
    poly pp = pHead(p);
    //PrintS("map:");pWrite(pp);
    if( d != NULL )
    {
      pSetCoeff(pp, n_Invers(d, currRing->cf));
      n_Delete(&d, currRing); // d = NULL;
    }
    else
      pSetCoeff(pp, nInit(1));

    //PrintS("->");pWrite((poly)(v->data));
    poly ppp = pMult((poly)(v->data),pp);
    //PrintS("->");pWrite(ppp);
    res=pAdd(res,ppp);
    pIter(p);
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
  map theMap=(map)idMaxIdeal(1);
  theMap->preimage=NULL;
  pDelete(&(theMap->m[var-1]));
  theMap->m[var-1]=pCopy(image);

  poly res=NULL;
#ifdef FAST_MAP
  if (pGetComp(p)==0)
  {
    ideal src_id=idInit(1,1);
    src_id->m[0]=p;
    ideal res_id=fast_map(src_id,currRing,(ideal)theMap,currRing);
    res=res_id->m[0];
    res_id->m[0]=NULL; idDelete(&res_id);
    src_id->m[0]=NULL; idDelete(&src_id);
  }
  else
#endif
  {
    sleftv tmpW;
    memset(&tmpW,0,sizeof(sleftv));
    tmpW.rtyp=POLY_CMD;
    tmpW.data=p;
    leftv v=(leftv)omAlloc0Bin(sleftv_bin);
    if (maApplyFetch(MAP_CMD,theMap,v,&tmpW,currRing,NULL,NULL,0,
                            n_SetMap(currRing->cf, currRing->cf)))
    {
      WerrorS("map failed");
      v->data=NULL;
    }
    res=(poly)(v->data);
    omFreeBin((ADDRESS)v, sleftv_bin);
  }
  idDelete((ideal *)(&theMap));
  return res;
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
  map theMap=(map)idMaxIdeal(1);
  theMap->preimage=NULL;
  pDelete(&(theMap->m[n-1]));
  theMap->m[n-1]=pCopy(e);

  leftv v=(leftv)omAlloc0Bin(sleftv_bin);
  sleftv tmpW;
  memset(&tmpW,0,sizeof(sleftv));
  tmpW.rtyp=IDEAL_CMD;
  tmpW.data=id;
  if (maApplyFetch(MAP_CMD,theMap,v,&tmpW,currRing,NULL,NULL,0,
                          n_SetMap(currRing->cf, currRing->cf)))
  {
    WerrorS("map failed");
    v->data=NULL;
  }
  ideal res=(ideal)(v->data);
  idDelete((ideal *)(&theMap));
  omFreeBin((ADDRESS)v, sleftv_bin);
  return res;
}
