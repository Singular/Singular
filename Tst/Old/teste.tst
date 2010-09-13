//
// test script for test command
//
//option(prot);
ring r1=32003,(a,b,x,y,z),ls;
r1;
"--------------------------";
test(0,15);
test(-15,24);
option();
"--------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
