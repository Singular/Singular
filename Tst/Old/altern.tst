//
// test script for alternating command
//
pagelength = 10000;
ring r = 0,(x,y,z),dp;
r;
"--------------------------------";
alternating=1;
poly s=xy;
s;
"-------------------------------";
listvar(all);
kill r;
//
// test script for dring command
//
pagelength = 10000;
ring r1 = 0,(x,y,z),dp;
r1;
"------------------------------";
dring d1=0,(x,d,h),dp;
d1;
"----------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
