/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipconv.cc,v 1.12 1998-10-21 10:25:31 krueger Exp $ */
/*
* ABSTRACT: automatic type conversions
*/

#include "mod2.h"
#include "tok.h"
#include "ipid.h"
#include "intvec.h"
#include "mmemory.h"
#include "febase.h"
#include "polys.h"
#include "ideals.h"
#include "subexpr.h"
#include "numbers.h"
#include "matpol.h"
#include "silink.h"
#include "syz.h"
#include "ipconv.h"

typedef void *   (*iiConvertProc)(void * data);
struct sConvertTypes
{
  int i_typ;
  int o_typ;
  iiConvertProc p;
};

static void * iiI2P(void *data)
{
  poly p=pISet((int)data);
  return (void *)p;
}

static void * iiI2V(void *data)
{
  poly p=pISet((int)data);
  if (p!=NULL) pSetComp(p,1);
  return (void *)p;
}

static void * iiI2Id(void *data)
{
  ideal I=idInit(1,1);
  I->m[0]=pISet((int)data);
  return (void *)I;
}

static void * iiP2V(void *data)
{
  poly p=(poly)data;
  if (p!=NULL) pSetCompP(p,1);
  return (void *)p;
}

static void * iiP2Id(void *data)
{
  ideal I=idInit(1,1);

  I->m[0]=(poly)data;
  if (data!=NULL)
  {
    poly p=(poly)data;
    if (pGetComp(p)!=0) I->rank=pMaxComp(p);
#ifdef DRING
    else if (pDRING)
    {
      while (p!=NULL)
      {
        pdSetDFlag(p,1);
        pSetm(p);
        pIter(p);
      }
    }
#endif
  }
  return (void *)I;
}

static void * iiV2Ma(void *data)
{
  matrix m=(matrix)idVec2Ideal((poly)data);
  int h=MATCOLS(m);
  MATCOLS(m)=MATROWS(m);
  MATROWS(m)=h;
  m->rank=h;
  return (void *)m;
}

static void * iiDummy(void *data)
{
  return data;
}

static void * iiMo2Ma(void *data)
{
  void *res=idModule2Matrix((ideal)data);
  idDelete((ideal *)&data);
  return res;
}

static void * iiMa2Mo(void *data)
{
  void *res=idMatrix2Module((matrix)data);
  idDelete((ideal *)&data);
  return res;
}

static void * iiI2Iv(void *data)
{
  intvec *iv=new intvec((int)data,(int)data);
  return (void *)iv;
}

static void * iiI2N(void *data)
{
  number n=nInit((int)data);
  return (void *)n;
}

static void * iiIm2Ma(void *data)
{
  int i, j;
  intvec *iv = (intvec *)data;
  matrix m = mpNew(iv->rows(), iv->cols());

  for (i=iv->rows(); i>0; i--)
  {
    for (j=iv->cols(); j>0; j--)
    {
      MATELEM(m, i, j) = pISet(IMATELEM(*iv, i, j));
    }
  }
  return (void *)m;
}

static void * iiN2P(void *data)
{
  poly p=NULL;
  if (!nIsZero((number)data))
  {
    p=pOne();
    pSetCoeff(p,(number)data);
  }
  return (void *)p;
}

static void * iiN2Ma(void *data)
{
  ideal I=idInit(1,1);
  if (!nIsZero((number)data))
  {
    poly p=pOne();
    pSetCoeff(p,(number)data);
    I->m[0]=p;
  }
  return (void *)I;
}

static void * iiS2Link(void *data)
{
  si_link l=(si_link)Alloc0(sizeof(ip_link));
  slInit(l, (char *) data);
  return (void *)l;
}

#if defined(INIT_BUG) || defined(PROC_BUG)
static void * iiR2L(void * data)
{
  return (void *)syConvRes((syStrategy)data);
}

static void * iiL2R(void * data)
{
  return (void *)syConvList((lists)data);
}
#else
#define iiR2L (iiConvertProc)syConvRes
#define iiL2R (iiConvertProc)syConvList
#endif

//
// automatic conversions:
//
struct sConvertTypes dConvertTypes[] =
{
//   input type       output type     convert procedure
//  int -> number
   { INT_CMD,         NUMBER_CMD,     iiI2N },
//  int -> poly
   { INT_CMD,         POLY_CMD,       iiI2P },
//  int -> vector
   { INT_CMD,         VECTOR_CMD,     iiI2V },
//  int -> ideal
   { INT_CMD,         IDEAL_CMD,      iiI2Id },
//  int -> matrix
   { INT_CMD,         MATRIX_CMD,     iiI2Id },
//  int -> intvec
   { INT_CMD,         INTVEC_CMD,     iiI2Iv },
//  intvec -> intmat
   { INTVEC_CMD,      INTMAT_CMD,     iiDummy},
//  intvec -> matrix
   { INTVEC_CMD,      MATRIX_CMD,     iiIm2Ma },
//  intmat -> matrix
   { INTMAT_CMD,      MATRIX_CMD,     iiIm2Ma },
//  number -> poly
   { NUMBER_CMD,      POLY_CMD,       iiN2P  },
//  number -> matrix
   { NUMBER_CMD,      MATRIX_CMD,     iiN2Ma  },
//  number -> ideal
//  number -> vector
//  number -> module
//  poly -> number
//  poly -> ideal
   { POLY_CMD,        IDEAL_CMD,      iiP2Id },
//  poly -> vector
   { POLY_CMD,        VECTOR_CMD,     iiP2V },
//  poly -> matrix
   { POLY_CMD,        MATRIX_CMD,     iiP2Id },
//  vector -> module
   { VECTOR_CMD,      MODUL_CMD,      iiP2Id },
//  vector -> matrix
   { VECTOR_CMD,      MATRIX_CMD,     iiV2Ma },
//  ideal -> module
   { IDEAL_CMD,       MODUL_CMD,      iiMa2Mo },
//  ideal -> matrix
   { IDEAL_CMD,       MATRIX_CMD,     iiDummy },
//  module -> matrix
   { MODUL_CMD,       MATRIX_CMD,     iiMo2Ma },
//  matrix -> ideal
//  matrix -> module
   { MATRIX_CMD,      MODUL_CMD,      iiMa2Mo },
//  intvec
//  link
   { STRING_CMD,      LINK_CMD,       iiS2Link },
// resolution -> list
   { RESOLUTION_CMD,  LIST_CMD,       iiR2L },
// list -> resolution
   { LIST_CMD,        RESOLUTION_CMD, iiL2R },
//  end of list
   { 0,               0,              NULL }
};

/*2
* try to convert 'input' of type 'inputType' to 'output' of type 'outputType'
* return FALSE on success
*/
BOOLEAN iiConvert (int inputType, int outputType, int index, leftv input, leftv output)
{
  memset(output,0,sizeof(sleftv));
  if ((inputType==outputType)
  || (outputType==DEF_CMD)
  || ((outputType==IDHDL)&&(input->rtyp==IDHDL)))
  {
    memcpy(output,input,sizeof(*output));
    memset(input,0,sizeof(*input));
    return FALSE;
  }
  else if (outputType==ANY_TYPE)
  {
    output->rtyp=ANY_TYPE;
    output->data=(char *)input->Typ();
    /* the name of the object:*/
    if (input->e==NULL)
    {
      if (input->rtyp==IDHDL)
      /* preserve name: copy it */
        output->name=mstrdup(IDID((idhdl)(input->data)));
      else if (input->name!=NULL)
      {
        output->name=input->name;
        input->name=NULL;
      }
      else if ((input->rtyp==POLY_CMD) && (input->name==NULL))
      {
        int nr=pIsPurePower((poly)input->data);
        if (nr!=0)
        {
          if (pGetExp((poly)input->data,nr)==1)
          {
            output->name=mstrdup(currRing->names[nr-1]);
          }
          else
          {
            output->name=(char *)AllocL(4);
            sprintf(output->name,"%c%d",*(currRing->names[nr-1]),
              pGetExp((poly)input->data,nr));
          }
        }
        else if(pIsConstant((poly)input->data))
        {
          output->name=nName(pGetCoeff((poly)input->data));
        }
#ifdef TEST
        else
        {
          WerrorS("wrong name, should not happen");
          output->name=mstrdup("?");
        }
#endif
      }
      else if ((input->rtyp==NUMBER_CMD) && (input->name==NULL))
      {
        output->name=nName((number)input->data);
      }
      else
      {
        /* no need to preserve name: use it */
        output->name=input->name;
        memset(input,0,sizeof(*input));
      }
    }
#ifdef HAVE_NAMESPACES
    output->packhdl = input->packhdl;
    output->req_packhdl = input->req_packhdl;
#endif /* HAVE_NAMESPACES */
    output->next=input->next;
    input->next=NULL;
    return FALSE;
  }
  if (index!=0) /* iiTestConvert does not returned 'failure' */
  {
    index--;

    if((dConvertTypes[index].i_typ==inputType)
    &&(dConvertTypes[index].o_typ==outputType))
    {
      if ((currRing==NULL) && (outputType>BEGIN_RING) && (outputType<END_RING))
        return TRUE;
      output->rtyp=outputType;
      output->data=dConvertTypes[index].p(input->CopyD());
      if ((output->data==NULL)
      && ((outputType==IDEAL_CMD)
        ||(outputType==MODUL_CMD)
        ||(outputType==MATRIX_CMD)
        ||(outputType==INTVEC_CMD)))
      {
        return TRUE;
      }
#ifdef HAVE_NAMESPACES
      //output->packhdl = input->packhdl;
      //output->req_packhdl = input->req_packhdl;
#endif /* HAVE_NAMESPACES */
      output->next=input->next;
      input->next=NULL;
      return FALSE;
    }
  }
  return TRUE;
}

/*2
* try to convert 'inputType' in 'outputType'
* return 0 on failure, an index (<>0) on success
*/
int iiTestConvert (int inputType, int outputType)
{
  if ((inputType==outputType)
  || (outputType==DEF_CMD)
  || (outputType==IDHDL)
  || (outputType==ANY_TYPE))
  {
    return -1;
  }

  if ((currRing==NULL) && (outputType>BEGIN_RING) && (outputType<END_RING))
    return 0;

  // search the list
  int i=0;
  while (dConvertTypes[i].i_typ!=0)
  {
    if((dConvertTypes[i].i_typ==inputType)
    &&(dConvertTypes[i].o_typ==outputType))
    {
      //Print("test convert %d to %d (%s -> %s):%d\n",inputType,outputType,
      //Tok2Cmdname(inputType), Tok2Cmdname(outputType),i+1);
      return i+1;
    }
    i++;
  }
  //Print("test convert %d to %d (%s -> %s):0\n",inputType,outputType,
  // Tok2Cmdname(inputType), Tok2Cmdname(outputType));
  return 0;
}
