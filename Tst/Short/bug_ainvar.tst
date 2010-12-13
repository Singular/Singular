LIB "tst.lib";
tst_init();

ring R=0,(x(1..4)),dp;
matrix m[4][1];
m[2,1]=x(1);
m[3,1]=x(1)+x(2);
m[4,1]=x(2)*x(3)+1;
LIB "ainvar.lib";
ideal in=invariantRing(m,x(2),x(1),0);
in;

tst_status(1);$
