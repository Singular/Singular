//
// test script for res command
//
//option(prot);
ring r1=32003,(a,b,x,y,z),ls;
r1;
"-------------------------";
ideal i=a2x2+ax2y+x2yz,a2y2+ay2z+y2zb,a2z2+az2b+xz2b,a2b2+axb2+xyb2;
i;
ideal i1=std(i);
i1;
list T=nres(i1,4);
def T(1..4)=T[1..4];
"------------------------";
module m=[x2y,0],[1,x2z3];
m;
module m1=std(m);
m1;
list M=res(m1,0);
def M(1..5)=M[1..5];
"-----------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
