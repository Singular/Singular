LIB "tst.lib";
tst_init();
ring R=QQ,(x,y,z),dp;
module a=[x3+y3+xy,1];
module b=[x];
list l=system ("DivRemIdU",a,b);
typeof(l[1]);
l[1];
attrib(l[1],"rank");
typeof(l[2]);
l[2];
typeof(l[3]);
l[3];
ideal A=x3+y3+xy;
ideal B=x;
list L=system ("DivRemIdU",A,B);
typeof(L[1]);
L[1];
NF(A,B);
attrib(L[1],"rank");
L[2];
L[3];
A=A,z;
L=system ("DivRemIdU",A,B);
L;
attrib(L[3],"rank");

// Finally statistics...
tst_status(1);$

