// $Id$

// sba_s.tst -- long tests for sba
// uses rcyclic examples from ISSAC'98 paper

LIB "tst.lib";
tst_init();

LIB"poly.lib";

proc cyclicn(int z)
{
   int n = z;
   ring R = 32003,(x(0..(n-1))),dp;
   ideal i = cyclic(n);
        string bench = "CYCLIC-"+string(n);
   export(R);
   export(i);
   return(bench);
}

proc cyclicnh(int z)
{
   int n = z;
   ring R = 32003,(x(0..(n-1)),h),dp;
   ideal i = homog(cyclic(n),h);
        string bench = "CYCLIC-"+string(n)+"-H";
   export(R);
   export(i);
   return(bench);
}

string bench = cyclicn(7);
sprintf(bench);
ideal f;
f = sba(i,1,0);
f = sba(i,1,1);
f = sba(i,0,0);
f = sba(i,0,1);
string bench = cyclicnh(8);
sprintf(bench);
ideal f;
f = sba(i,0,1);

tst_status(1);$
