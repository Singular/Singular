//
// test script for kill command
//
ring r1=32003,(x,y,z),dp;
r1;
"-----------------------";
int i=34;
kill i;
i;
"--------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;

