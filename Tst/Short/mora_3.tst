LIB "tst.lib";
tst_init();
option(prot);

ring r=32003,(w,x,y,z),ds;
int a =20;
int b =10;
int c =3;
int t =1;
poly f= x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
ideal j=jacob(f);
ideal i=std(j);

tst_status(1);$


