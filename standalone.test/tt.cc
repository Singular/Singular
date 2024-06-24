// Example program how to use libsingular

#include <Singular/libsingular.h>
#include <unistd.h>


int main( int, char *argv[] )
{
  feInitResources(argv[0]);

  StringSetS("ressources in use (as reported by feStringAppendResources(0):\n");
  feStringAppendResources(0);

  { StringAppendS("\n"); char* s = StringEndS(); PrintS(s); omFree(s); }

//  // init path names etc.
//  siInit(argv[0]);


  // construct the ring Z/32003[x,y,z]
  // the variable names
  char **n=(char**)omalloc(3*sizeof(char*));
  n[0]=omStrDup("x");
  n[1]=omStrDup("y");
  n[2]=omStrDup("z2");

  ring R=rDefault(32003,3,n);
  // make R the default ring:
  rChangeCurrRing(R);

  // create the polynomial 1
  poly p1=p_ISet(1,R);

  // create the polynomial 2*x^3*z^2
  poly p2=p_ISet(2,R);
  pSetExp(p2,1,3);
  pSetExp(p2,3,2);
  pSetm(p2);

  // print p1 + p2
  pWrite(p1); printf(" + \n"); pWrite(p2); printf("\n");

  // compute p1+p2
  p1=p_Add_q(p1,p2,R); p2=NULL;
  pWrite(p1);

//   sleep(120);

  // cleanup:
  pDelete(&p1);
  rKill(R);
}
