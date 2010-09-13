LIB "tst.lib";
tst_init();
//
// test script for sres command
//
//option(prot);
ring r1=32003,(a,b,x,y,z),ls;
r1;
"-------------------------";
ideal i=a2x2+ax2y+x2yz,a2y2+ay2z+y2zb,a2z2+az2b+xz2b,a2b2+axb2+xyb2;
i;
ideal i1=std(i);
i1;
sres(i1,4);
list(sres(i1,4));
"------------------------";
module m=[x2y,0],[1,x2z3];
m;
module m1=std(m);
m1;
sres(m1,0);
list(sres(m1,0));
"-----------------------";
listvar(all);
kill r1;
tst_status(1);$;
