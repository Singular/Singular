#include "Singular/libsingular.h"

static void makepartition(int vars,int actvar,int deg,int monomdeg, lists L, int &idpowerpoint)
{
  poly p;
  int i=0;

  if ((idpowerpoint == 0) && (actvar ==1))
  {
    L->m[idpowerpoint].rtyp=INTVEC_CMD;
    L->m[idpowerpoint].data=(char *)new intvec(vars);
    monomdeg = 0;
  }
  while (i<=deg)
  {
    if (deg == monomdeg)
    {
      idpowerpoint++;
      return;
    }
    if (actvar == vars)
    {
      intvec *v=(intvec*)L->m[idpowerpoint].data;
      (*v)[actvar-1]=deg-monomdeg;
      idpowerpoint++;
      return;
    }
    else
    {
      intvec *v=(intvec*)L->m[idpowerpoint].data;
      intvec *vv=ivCopy(v);
      makepartition(vars,actvar+1,deg,monomdeg,L,idpowerpoint);
      L->m[idpowerpoint].data=(char *)vv;
      L->m[idpowerpoint].rtyp=INTVEC_CMD;
    }
    monomdeg++;
    intvec *v=(intvec*)L->m[idpowerpoint].data;
    (*v)[actvar-1]=(*v)[actvar-1]+1;
    i++;
  }
}
lists lPartition(int sum, int elem)
{
  lists L=(lists)omAlloc0Bin(slists_bin);
  if (sum < 0)
  {
    WarnS("partition: power must be non-negative");
  }
  int i = binom(elem+sum-1,sum);
  if ((sum < 1)||(i<1))
  {
    L->Init();
    return L;
  }
  L->Init(i);
  int idpowerpoint = 0;
  makepartition(elem,1,sum,0,L,idpowerpoint);
  return L;
}

static BOOLEAN partition(leftv res, leftv arg)
{
  if ((arg!=NULL)&&(arg->Typ()==INT_CMD)
  && (arg->next!=NULL) &&(arg->next->Typ()==INT_CMD)
  && (arg->next->next==NULL))
  {
   res->data=(void*)lPartition((int)(long)arg->Data(),(int)(long)arg->next->Data());
   res->rtyp=LIST_CMD;
   return FALSE;
  }
  return TRUE;
}
extern "C" int mod_init(SModulFunctions* psModulFunctions)
{
  // this is the initialization routine of the module
  // adding the routine hello:
  psModulFunctions->iiAddCproc(
    (currPack->libname? currPack->libname: ""),// the library name,
            // rely on the loader to set it in currPack->libname
    "partition",// the name for the singular interpreter
    FALSE,  // should not be static
    partition); // the C/C++ routine
  const char* partition_help="USAGE: partition(int a, int b)\n"
                       "RETURN: list of intvec with b entries and sum a";
  module_help_proc(
    (currPack->libname? currPack->libname: ""),// the library name,
    "partition", // the name of the procedure
    partition_help); // the help string
  module_help_main(
     (currPack->libname? currPack->libname: ""),// the library name,
    "partition"); // the help string for the module
  VAR return MAX_TOK;
}



