LIB "tst.lib";
tst_init();
LIB("ring.lib");


proc isNthPrimitiveRoot(number root, int n)
{
   if (n>0) { ASSUME(0, "0" != string(minpoly) ); }
   if ( n==1 ) { return(1==root); }
   number m=root;
   int i; 
   for (i=2;i<n;i++)
   {
       m = m*root;
       if (m==1) {return(0);}
   }
   return (1);
}



proc testRootofUnity()
{
    ring rng = (0,a),x,dp;
    minpoly = (a8-a7+a5-a4+a3-a+1);
    ASSUME(0, not isNthPrimitiveRoot(a,30));
    int i; 

    for(i = 1; i<100; i++)
    {
        kill rng;
        ring rng = (0,a),x,dp;
        minpoly = rootofUnity(i);
        ASSUME(0,  isNthPrimitiveRoot(a,i) );
    }
}

testRootofUnity();


tst_status(1); $;

