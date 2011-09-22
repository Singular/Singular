LIB "tst.lib"; tst_init();
ring r3=32003,(x,y,z),ds;
int a,b,c,t=11,10,3,1;
poly f=x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3
  +x^(c-2)*y^c*(y2+t*x)^2;
ideal i=jacob(f);
ideal i0=std(i);
degree(i0);
tst_status(1);$
