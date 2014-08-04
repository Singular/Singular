LIB "tst.lib";
tst_init();
LIB("ring.lib");


proc parIsNthPrimitiveRoot(int n)
{
   if (n==0) {  return( minpoly==number(0) ); }
   ASSUME(0, "0" != string(minpoly) ); 
   number m=1;
   int i; 
   for (i=1;i<n;i++)
   {
       m = m*par(1);
       if (m==1) {return(0);}
   }
   ASSUME(0, 1 == m*par(1) );
   return (1);
}



proc testRootofUnity()
{
    ring rng = (0,a),x,dp;
    minpoly = (a8-a7+a5-a4+a3-a+1);
    ASSUME(0, not parIsNthPrimitiveRoot(30));
    int i; 

    for(i = 0; i<100; i++)
    {
        kill rng;
        ring rng = (0,a),x,dp;
        minpoly = rootofUnity(i);
        ASSUME(0,  parIsNthPrimitiveRoot(i) );
    }
}

testRootofUnity();


tst_status(1); $;

