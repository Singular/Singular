//
// test script for rvar command
//
pagelength = 10000;
ring r1 = 32003,(x,y,z),dp;
r1;
"-------------------------------";
rvar(y);
"--------------------------------";
rvar(r);
"-----------------------------";
rvar(r1);
"------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
