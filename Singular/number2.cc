#include "kernel/mod2.h" // general settings/macros
#include"kernel/febase.h"  // for Print, WerrorS
#include"Singular/ipid.h" // for SModulFunctions, leftv
#include"coeffs/numbers.h" // nRegister, coeffs.h
#include"coeffs/rmodulon.h" // ZnmInfo
#include"coeffs/longrat.h" // nlGMP
#include"Singular/blackbox.h" // blackbox type
#include"Singular/ipshell.h" // IsPrime

#include <Singular/number2.h>

char *crString(coeffs c)
{
  StringSetS("");
  if (c==NULL) StringAppendS("oo");
  else StringAppend("Coeff(%d):%s",c->type,c->cfCoeffString(c));
  return omStrDup(StringEndS());
}
void crPrint(coeffs c)
{
  char *s=crString(c);
  PrintS(s);
  omFree(s);
}

BOOLEAN jjCRING_Zp(leftv res, leftv a, leftv b)
{
  coeffs c1=(coeffs)a->Data();
  int    i2=(int)(long)b->Data();
  if (c1->type==n_Z)
  {
    if (i2==IsPrime(i2))
    {
      res->data=(void *)nInitChar(n_Zp,(void*)(long)i2);
    }
    else
    {
      ZnmInfo info;
      mpz_ptr modBase= (int_number) omAlloc(sizeof(mpz_t));
      mpz_init_set_ui(modBase,i2);
      info.base= modBase;
      info.exp= 1;
      res->data=(void *)nInitChar(n_Zn,&info);
    }
    return FALSE;
  }
  return TRUE;
}
BOOLEAN jjCRING_Zm(leftv res, leftv a, leftv b)
{
  coeffs c1=(coeffs)a->Data();
  number i2=(number)b->Data();
  if (c1->type==n_Z)
  {
    ZnmInfo info;
    number modBase= (number) omAlloc(sizeof(mpz_t));
    nlGMP(i2,modBase,coeffs_BIGINT);
    info.base= (mpz_ptr)modBase;
    info.exp= 1;
    res->data=(void *)nInitChar(n_Zn,&info);
    return FALSE;
  }
  return TRUE;
}

