//
// test script for echo command
//
ring r = 0,(x,y,z),dp;
r;
"--------------------------------";
echo=1;
int i=echo;
i;
"--------------------------------";
echo=2;
int j=echo;
"-------------------------------";
listvar(all);
kill r;
LIB "tst.lib";tst_status(1);$;
