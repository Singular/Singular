/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: automatic type conversions
*/

#include <kernel/mod2.h>
#include <Singular/tok.h>
#include <Singular/ipid.h>
#include <kernel/intvec.h>
#include <kernel/options.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <Singular/subexpr.h>
#include <kernel/numbers.h>
#include <kernel/modulop.h>
#include <kernel/longrat.h>
#include <kernel/longalg.h>
#ifdef HAVE_RINGS
#include <kernel/rmodulon.h>
#include <kernel/rmodulo2m.h>
#include <kernel/rintegers.h>
#endif
#include <kernel/matpol.h>
#include <Singular/silink.h>
#include <kernel/syz.h>
#include <Singular/attrib.h>
#include <kernel/ring.h>
#include <Singular/ipshell.h>
#include <Singular/ipconv.h>

typedef void *   (*iiConvertProc)(void * data);
typedef void    (*iiConvertProcL)(leftv out,leftv in);
struct sConvertTypes
{
  int i_typ;
  int o_typ;
  iiConvertProc p;
  iiConvertProcL pl;
};

// all of these static conversion routines work destructive on their input

static void * iiI2P(void *data)
{
  poly p=pISet((int)(long)data);
  return (void *)p;
}

static void * iiBI2P(void *data)
{
  number n=nInit_bigint((number)data);
  nlDelete((number *)&data,NULL);
  poly p=pNSet(n);
  return (void *)p;
}

static void * iiI2V(void *data)
{
  poly p=pISet((int)(long)data);
  if (p!=NULL) pSetComp(p,1);
  return (void *)p;
}

static void * iiBI2V(void *data)
{
  number n=nInit_bigint((number)data);
  nlDelete((number *)&data,NULL);
  poly p=pNSet(n);
  if (p!=NULL) pSetComp(p,1);
  return (void *)p;
}

static void * iiI2Id(void *data)
{
  ideal I=idInit(1,1);
  I->m[0]=pISet((int)(long)data);
  return (void *)I;
}

static void * iiBI2Id(void *data)
{
  ideal I=idInit(1,1);
  number n=nInit_bigint((number)data);
  nlDelete((number *)&data,NULL);
  poly p=pNSet(n);
  I->m[0]=p;
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
  pDelete((poly *)&data);
  return (void *)m;
}

static void * iiN2P(void *data);

static void * iiDummy(void *data)
{
  return data;
}

static void * iiMo2Ma(void *data)
{
  void *res=idModule2Matrix((ideal)data);
  return res;
}

static void * iiMa2Mo(void *data)
{
  void *res=idMatrix2Module((matrix)data);
  return res;
}

static void * iiI2Iv(void *data)
{
  int s=(int)(long)data;
  intvec *iv=new intvec(s,s);
  return (void *)iv;
}

static void * iiI2N(void *data)
{
  number n=nInit((int)(long)data);
  return (void *)n;
}

static void * iiI2BI(void *data)
{
  number n=nlInit((int)(long)data, NULL /*dummy for nlInit*/);
  return (void *)n;
}

static void * iiBI2N(void *data)
{
  if (currRing==NULL) return NULL;
  // a bigint is really a number from char 0, with diffrent operations...
  return (void*)nInit_bigint((number)data);
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
  delete iv;
  return (void *)m;
}

static void * iiN2P(void *data)
{
  poly p=NULL;
  if (!nIsZero((number)data))
  {
    p=pNSet((number)data);
  }
  //else
  //{
  //  nDelete((number *)&data);
  //}
  return (void *)p;
}

static void * iiN2Ma(void *data)
{
  ideal I=idInit(1,1);
  if (!nIsZero((number)data))
  {
    poly p=pNSet((number)data);
    I->m[0]=p;
  }
  //else
  //{
  //  nDelete((number *)&data);
  //}
  return (void *)I;
}

static void * iiS2Link(void *data)
{
  si_link l=(si_link)omAlloc0Bin(ip_link_bin);
  slInit(l, (char *) data);
  omFree((ADDRESS)data);
  return (void *)l;
}

/*
static void * iiR2L(void * data)
{
  syStrategy tmp=(syStrategy)data;
  return  (void *)syConvRes(tmp,TRUE);
}
*/
static void iiR2L_l(leftv out, leftv in)
{
  int add_row_shift = 0;
  intvec *weights=(intvec*)atGet(in,"isHomog",INTVEC_CMD);
  if (weights!=NULL)  add_row_shift=weights->min_in();

  syStrategy tmp=(syStrategy)in->CopyD();
  out->data=(void *)syConvRes(tmp,TRUE,add_row_shift);
}

static void * iiL2R(void * data)
{
  return (void *)syConvList((lists)data,TRUE);
}

//
// automatic conversions:
//
struct sConvertTypes dConvertTypes[] =
{
//   input type       output type     convert procedure
//  int -> bigint
   { INT_CMD,         BIGINT_CMD,     iiI2BI , NULL },
//  int -> number
   { INT_CMD,         NUMBER_CMD,     iiI2N , NULL },
   { BIGINT_CMD,      NUMBER_CMD,     iiBI2N , NULL },
//  int -> poly
   { INT_CMD,         POLY_CMD,       iiI2P , NULL },
   { BIGINT_CMD,      POLY_CMD,       iiBI2P , NULL },
//  int -> vector
   { INT_CMD,         VECTOR_CMD,     iiI2V , NULL },
   { BIGINT_CMD,      VECTOR_CMD,     iiBI2V , NULL },
//  int -> ideal
   { INT_CMD,         IDEAL_CMD,      iiI2Id , NULL },
   { BIGINT_CMD,      IDEAL_CMD,      iiBI2Id , NULL },
//  int -> matrix
   { INT_CMD,         MATRIX_CMD,     iiI2Id , NULL },
   { BIGINT_CMD,      MATRIX_CMD,     iiBI2Id , NULL },
//  int -> intvec
   { INT_CMD,         INTVEC_CMD,     iiI2Iv , NULL },
//  intvec -> intmat
   { INTVEC_CMD,      INTMAT_CMD,     iiDummy, NULL },
//  intvec -> matrix
   { INTVEC_CMD,      MATRIX_CMD,     iiIm2Ma , NULL },
//  intmat -> matrix
   { INTMAT_CMD,      MATRIX_CMD,     iiIm2Ma , NULL },
//  number -> poly
   { NUMBER_CMD,      POLY_CMD,       iiN2P  , NULL },
//  number -> matrix
   { NUMBER_CMD,      MATRIX_CMD,     iiN2Ma  , NULL },
//  number -> ideal
//  number -> vector
//  number -> module
//  poly -> number
//  poly -> ideal
   { POLY_CMD,        IDEAL_CMD,      iiP2Id , NULL },
//  poly -> vector
   { POLY_CMD,        VECTOR_CMD,     iiP2V , NULL },
//  poly -> matrix
   { POLY_CMD,        MATRIX_CMD,     iiP2Id , NULL },
//  vector -> module
   { VECTOR_CMD,      MODUL_CMD,      iiP2Id , NULL },
//  vector -> matrix
   { VECTOR_CMD,      MATRIX_CMD,     iiV2Ma , NULL },
//  ideal -> module
   { IDEAL_CMD,       MODUL_CMD,      iiMa2Mo , NULL },
//  ideal -> matrix
   { IDEAL_CMD,       MATRIX_CMD,     iiDummy , NULL },
//  module -> matrix
   { MODUL_CMD,       MATRIX_CMD,     iiMo2Ma , NULL },
//  matrix -> ideal
//  matrix -> module
   { MATRIX_CMD,      MODUL_CMD,      iiMa2Mo , NULL },
//  intvec
//  link
   { STRING_CMD,      LINK_CMD,       iiS2Link , NULL },
// resolution -> list
   { RESOLUTION_CMD,  LIST_CMD,       NULL /*iiR2L*/ , iiR2L_l },
// list -> resolution
   { LIST_CMD,        RESOLUTION_CMD, iiL2R , NULL },
//  end of list
   { 0,               0,              NULL , NULL }
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
        output->name=omStrDup(IDID((idhdl)(input->data)));
      else if (input->name!=NULL)
      {
        output->name=input->name;
        input->name=NULL;
      }
      else if ((input->rtyp==POLY_CMD) && (input->name==NULL))
      {
        if (input->data!=NULL)
        {
          int nr=pIsPurePower((poly)input->data);
          if (nr!=0)
          {
            if (pGetExp((poly)input->data,nr)==1)
            {
              output->name=omStrDup(currRing->names[nr-1]);
            }
            else
            {
              char *tmp=(char *)omAlloc(4);
              sprintf(tmp,"%c%d",*(currRing->names[nr-1]),
                (int)pGetExp((poly)input->data,nr));
              output->name=tmp;
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
            output->name=omStrDup("?");
          }
#endif
        }
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
      if(TEST_V_ALLWARN)
      {
        Print("automatic  conversion %s -> %s\n",
        Tok2Cmdname(inputType),Tok2Cmdname(outputType));
      }
      if ((currRing==NULL) && (outputType>BEGIN_RING) && (outputType<END_RING))
        return TRUE;
      output->rtyp=outputType;
      if (dConvertTypes[index].p!=NULL)
      {
        output->data=dConvertTypes[index].p(input->CopyD());
      }
      else
      {
        dConvertTypes[index].pl(output,input);
      }
      if ((output->data==NULL)
      && ((outputType!=INT_CMD)
        &&(outputType!=POLY_CMD)
        &&(outputType!=VECTOR_CMD)
        &&(outputType!=NUMBER_CMD)))
      {
        return TRUE;
      }
      output->next=input->next;
      input->next=NULL;
  //if (outputType==MATRIX_CMD) Print("convert %d -> matrix\n",inputType);
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
