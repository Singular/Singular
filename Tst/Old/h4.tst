ring h4=32003,(t,x,y,z,w),dp;
ideal j=
4t4z3y+6z4x3t5+5x5z7,
5t6z3y3x+5x2z4t3y7+4x5t5y,
6z7t5y+6z3y2t4+2t8z6;
ideal i=j;
ideal i=homog(j,w);
ideal i0=std( i);
size(i0);
degree (i0);
i0;
kill h4;
ring rh8=32003,(w,x,y,z),dp;
int a =15;
int b =10;
int c =3;
int t =1;
poly f= x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
ideal k= jacob(f);
a =10;
b= 11;
c= 3;
t= 1;
f=x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
ideal j= jacob(f);
j =j+k;
ideal i=homog(j,w);
ideal i0=std( i);

size(i0);
degree (i0);
i0;
kill rh8,a,b,c,t;
$
