/* compile:
 * SINGUNAME=`./singuane`
 *  g++  -I$SINGUNAME/include -o libsing-test1 libsing-test1.cc -L./Singular -lsingular
 */
#include <libsingular.h>
main()
{
  // init path names etc.: the string should point to an existing
  //            executable in a directory $B
  //            LIBs are searched in $B/LIB, $B/../LIB,
  //            see kernel/feRessource.cc
  siInit((char *)"./Singular/libsingular.so");

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

  // create tthe polynomial 2*x^3*z^2
  poly p2=p_ISet(2,R);
  pSetExp(p2,1,3);
  pSetExp(p2,3,2);
  pSetm(p2);

  // print p1 + p2
  pWrite(p1); printf(" + \n"); pWrite(p2); printf("\n");

  // compute p1+p2
  p1=p_Add_q(p1,p2,R); p2=NULL;
  pWrite(p1); 

  // clean up:
  pDelete(&p1);
  rKill(R);

  currentVoice=feInitStdin(NULL);
  // hook for error handling:
  // WerrorS_callback=......; of type p(const char *)
  // start in level 1, for better error handling:
  myynest=1;
  // the return is needed to stop the interpreter
  // - otherwise it would continue with input fron stdin / tty
  int err=iiAllStart(NULL,"int ver=system(\"version\");return();\n",BT_proc,0);
  if (err) errorreported = 0; // reset error handling
  printf("interpreter returns %d\n",err);
  idhdl h=ggetid("ver");
  if (h!=NULL)
    printf("singular variable ver of type %d contains %d\n",h->typ,(int)(long)IDDATA(h));
  else
    printf("variable ver does not exist\n");

  // calling a singular-library function
  idhdl datetime=ggetid("datetime");
  if (datetime==NULL)
    printf("datetime not found\n");
  else
  {
    leftv res=iiMake_proc(datetime,NULL,NULL);
    if (res==NULL) { printf("datetime return an error\n"); errorreported = 0; }
    else           printf("datetime returned type %d, >>%s<<\n",res->Typ(),(char *)res->Data());
  }

  // changing a ring for the interpreter
  // re-using n and R from above
  R=rDefault(32003,3,n);
  idhdl newRingHdl=enterid("R" /* ring name*/,
                           0, /*nesting level, 0=global*/
                           RING_CMD,
                           &IDROOT,
                           FALSE);
   IDRING(newRingHdl)=R;
   // make R the default ring (include rChangeCurrRing):
   rSetHdl(newRingHdl);
   err=iiAllStart(NULL,"poly p=x;listvar();return();\n",BT_proc,0);

  // calling a kernel function via the interpreter interface
  sleftv r1; memset(&r1,0,sizeof(r1));
  sleftv arg; memset(&arg,0,sizeof(arg));
  arg.rtyp=STRING_CMD;
  arg.data=omStrDup("huhu");
  err=iiExprArith1(&r1,&arg,TYPEOF_CMD);
  printf("interpreter returns %d\n",err);
  if (err) errorreported = 0; // reset error handling
  else
  // here we know that r1 is of type STRING_CMD, we can cast it to char *:
  printf("typeof returned type %d, >>%s<<\n",r1.Typ(),(char*)r1.Data());
  // clean up r1:
  r1.CleanUp();
}
