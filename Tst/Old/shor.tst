//
// test script for short command
//
pagelength = 10000;
ring r = 13,x,dp;
r;
"---------------------------";
int save=short;
short=1;
x2;
short=0;
x2;
short=save;
"--------------------------";
listvar(all);
kill r;
LIB "tst.lib";tst_status(1);$;
