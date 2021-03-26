// $Id$

// sba_s.tst -- long tests for sba
// uses rcyclic examples from ISSAC'98 paper

LIB "tst.lib";
tst_init();

LIB"polylib.lib";

proc katsuran(int z)
{
   int n = z;
   ring R = 32003,(x(0..(n))),dp;
   ideal i = katsura(n);
        string bench = "KATSURA-"+string(n);
   export(R);
   export(i);
   return(bench);
}

proc katsuranh(int z)
{
   int n = z;
   ring R = 32003,(x(0..(n)),h),dp;
   ideal i = homog(katsura(n),h);
        string bench = "KATSURA-"+string(n)+"-H";
   export(R);
   export(i);
   return(bench);
}

string bench = katsuran(11);
sprintf(bench);
ideal f;
f = sba(i,1,0);
f = sba(i,1,1);
killall;
string bench = katsuranh(11);
sprintf(bench);
ideal f;
f = sba(i,1,0);
f = sba(i,1,1);
killall;

tst_status(1);$
LIB "polylib.lib";
killall();
killall("proc");
exit;
