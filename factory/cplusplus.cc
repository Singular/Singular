#include <stdio.h>

#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)

int main(void)
{
   int ret;
#  ifdef __cplusplus
   printf("#ifndef __cplusplus   \n" );
   printf("/*BEWARE: this fix can lead to problems if cf_gmp.h is publicly installed, while mixing different (versions of) compilers!!!*/\n" );
   printf("#define __cplusplus %s\n", QUOTEME( __cplusplus )); // System current compiler DEPENDENT!!!
   printf("#endif /*__cplusplus*/\n" );
   ret = 0;
#  else
   ret = 1;
#  endif

   return ret;
/*   
 #define __cplusplus_backup __cplusplus

#ifdef __cplusplus_backup
  printf("__cplusplus_backup: %s\n", QUOTEME(__cplusplus_backup) );
#else
  printf("undefined __cplusplus_backup!\n");
#endif

#undef __cplusplus
   
#ifdef __cplusplus_backup
  printf("__cplusplus_backup: %s\n", QUOTEME(__cplusplus_backup) );
#else
  printf("undefined __cplusplus_backup!\n");
#endif
*/
}
