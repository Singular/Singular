//
// test script for freemodule command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;
"-------------------------------";
freemodule(3);
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
