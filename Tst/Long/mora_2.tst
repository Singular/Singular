LIB "tst.lib";
tst_init();
option(prot);

ring r=32003,(w,x,y,z),ds;
int a =10;
int b =10;
int c =11;
int alpha =5;
int beta= 5;
int t= 0;
poly f =x^a+y^b+z^c+x^alpha*y^(beta-5)+x^(alpha-2)*y^(beta-3)+x^(alpha-3)*y^(beta-4)*z^2+x^(alpha-4)*y^(beta-4)*(y^2+t*x)^2;
ideal j= jacob(f);
j=j,f;
ideal i=std(j);

tst_status(1);$


