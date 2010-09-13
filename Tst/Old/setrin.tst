//
// test script for setring command
//
//option(prot);
ring r1=32003,(a,b,x,y,z),ls;
r1;
"------------------------";
ring r2=0,(x,y,z),ds;
r2;
"---------------------";
setring r1;
"---------------------";
listvar(all);
kill r1,r2;
LIB "tst.lib";tst_status(1);$;
