//
// test script for test command
//
pagelength = 10000;
//option(prot);
ring r1=32003,(a,b,x,y,z),ls;
r1;
"--------------------------";
test(0,8,15);
test(-15,24);
option();
"--------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
