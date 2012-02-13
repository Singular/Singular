/* compile:
 * SINGUNAME=`./singunane`
 *  g++  -I$SINGUNAME/include -o libsing-test2 libsing-test2.cc -L./Singular -lsingular
 */
//---------------------------
//  how to call a library function: Primdec::primdecGTZ
//---------------------------
#include <libsingular.h>
main()
{
  // init path names etc.: the string should point to an existing
  //            executable in a directory $B
  //            LIBs are searched in $B/LIB, $B/../LIB,
  //            see kernel/feRessource.cc
  siInit((char *)"./Singular/libsingular.so");

  // a more advanced procedure call
  // first, load the libary primdec.lib
  sleftv arg,r1,r2;

  // load the singular library primdec.lib:
  memset(&arg,0,sizeof(arg));
  memset(&r1,0,sizeof(r1));
  memset(&r2,0,sizeof(r2));
  arg.rtyp=STRING_CMD;
  arg.data=omStrDup("primdec.lib");
  r2.rtyp=LIB_CMD;
  int err=iiExprArith2(&r1,&r2,'(',&arg);
  if (err) printf("interpreter returns %d\n",err);
  // now, get the procedure to call
  idhdl primdecGTZ=ggetid("primdecGTZ");
  if (primdecGTZ==NULL)
    printf("primdecGTZ not found\n");
  else
  {
    // prepare the arguments
    // create a ring Q[x,y]
      // the variable names
      char **n=(char**)omAlloc(2*sizeof(char*)); 
      n[0]=(char*)"x";
      n[1]=(char*)"y";
      // create the ring
      ring R=rDefault(0,2,n);
      // n is not needed any more:
      omFreeSize(n,2*sizeof(char*));
    // make it the default ring, also for the interpeter
    idhdl newRingHdl=enterid("R" /* ring name*/,
                           0, /*nesting level, 0=global*/
                           RING_CMD,
                           &IDROOT,
                           FALSE);
    IDRING(newRingHdl)=R;
    rSetHdl(newRingHdl);
    // create the ideal to decompose
    ideal I=idInit(2,1); // with 2 elements
    // create x-25
    poly p=pOne(); pSetExp(p,1,1); 
    pSetm(p); // pSetm mut be called after a sequence of pSetExp/pSetExpV
    poly p2=pISet(25); p=pSub(p,p2);
    I->m[0]=p;
    // create 4*y^2
    p=pISet(4); pSetExp(p,2,2);  pSetm(p);
    I->m[1]=p;
    // create the arguments for primdecGTZ
    arg.rtyp=IDEAL_CMD;
    arg.data=(void *)I;
    // call primdecGTZ
    leftv res=iiMake_proc(primdecGTZ,NULL,&arg);
    if (res==NULL)
    { printf("primdecGTZ returned an error\n"); errorreported = 0; }
    else
    {
      printf("primdecGTZ returned type %d; %s\n",res->Typ(),Tok2Cmdname(res->Typ()));
      // if it is a list, get the parts:
      if (res->Typ()==LIST_CMD)
      {
        lists L=(lists)res->Data();
	printf("returned list consists of %d elements\n",L->nr+1);
	for(int i=0;i<=L->nr;i++)
	{
	  printf("elem %d has type %d (%s)\n",i,L->m[i].Typ(),Tok2Cmdname(L->m[i].Typ()));
	  if (L->m[i].Typ()==LIST_CMD)
	  {
            lists LL=(lists)L->m[i].Data();
	    printf("list consists of %d elements\n",LL->nr+1);
	    for(int j=0;j<=LL->nr;j++)
	    {
	      printf("elem %d,%d has type %d (%s)\n",i,j,LL->m[j].Typ(),Tok2Cmdname(LL->m[j].Typ()));
	      if (LL->m[j].Typ()==IDEAL_CMD)
	      {
	        // print it out:
	        printf("%s\n",LL->m[j].String()); 
		// access to the elements (r/o)
		ideal II=(ideal)LL->m[j].Data();
		printf("ideal with %d generators\n",IDELEMS(II));
		for(int jj=0;jj<IDELEMS(II);jj++)
		{
		  poly pp=II->m[jj];
		  if (pp==NULL)
		    printf("poly %d is 0\n",jj);
		  else
		    printf("poly %d starts with coeff 0x%lx, x-exponent %ld and y-exp. %ld\n",jj,(unsigned long)pGetCoeff(pp),pGetExp(pp,1),pGetExp(pp,2));
		}
	      }
	    }
	  }
	}
      }
    }
  }
}
/* coding of objects of type number:
   this depend on the corresponding ring
   for Z/p: number is equivalent to long, objects are in the range 0..p-1
   for Q: - number2mpz converts to mpz_t, if n is integer
          - if the last bit is 1 (if (SR_HDL(n) & SR_INT))
            then the value of n is a long l=SR_TO_INT(n)
          - otherwise, n is a pointer to snumber (mpz_t z, mpz_t n, s)
	    if s==3: the value of n is ithe GMP integer n->z
	    if s==0 or s==1: the value of n the rational of
	                     n->z / n->n
			     (s==0: n may not be normalized)
   for other coefficient fields: see the corresponding sources
*/
/* creating of objects of type number:
  - from int: nInit(i)
  - from mpz_t: mpz2number (only for Q!)
  - from number(Q) to number(any coeff): nInit_bigint
*/
/* creating objects of type poly:
   - the zero polynomials is represented by NULL
   - from int: pISet(i)
   - from number pNSet(n)
*/
