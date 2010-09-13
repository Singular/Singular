//
// test script for int command
//
ring r1 = 0,(x,y,z),dp;
r1;
"--------------------------------";
int i=10;
int j=i+3;
j;
"-------------------------------";
int k;
k;
"------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
