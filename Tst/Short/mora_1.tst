LIB "tst.lib";
tst_init();

option(prot);
ring r=32003,(x,y,z),ds;
int a =35;
int b =25;
int c =20;
int alpha =7;
int beta= 11;
int t= 1;
poly f =x^a+y^b+z^c+x^alpha*y^(beta-5)+x^(alpha-2)*y^(beta-3)+x^(alpha-3)*y^(beta-4)*z^2+x^(alpha-4)*y^(beta-4)*(y^2+t*x)^2;
ideal j= jacob(f);
j=j,f;
ideal i=std(j);
tst_status(1);$


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

ring r=32003,(w,x,y,z),ds;
int a =20;
int b =10;
int c =3;
int t =1;
poly f= x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
ideal j=jacob(f);
ideal i=std(j);

ring r=32003,(x,y,z),ds;
int a =30;
int b =20;
int c =7;
int t =1;
poly f= x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
ideal j=jacob(f);
ideal i=std(j);

ring r=32003,(x,y,z),ds;
int a=20;
poly f =xyz*(x+y+z)^2 +(x+y+z)^3 +x^a+y^a+z^a;
ideal i= jacob(f);
i=i,f;
ideal j=std( i);

ring r=32003,(w,x,y,z),ds;
int a=7;
poly f =xyz*(x+y+z)^2 +(x+y+z)^3 +x^a+y^a+z^a;
ideal i= jacob(f);
i=i,f;
ideal j=std( i);

ring s=32003,(x,y,z,u),ds;
int a=7;
poly f =xyzu*(x+y+z+u) +(x+y+z+u)^2 +x^a+y^a+z^a+u^a;
ideal i= jacob(f);
ideal j=std( i);

ring r=32003,(x,y,z,t,u,v),ds;
ideal i=
(y+u+v-zt)*(z+t+2u-3xt),
z2+t2+2u2-3u2v,
yx+t2+2yv-xyz,
xv-yu-zt-t2-u2-v2,
-1569yz3+31250x2tu,
-587yt+15625zv;
ideal j=std( i);

ring r = 32003,(z(3..1),x(13..7),u(6..2)),ds;
ideal in =
u(5)*u(2)^2-u(3)*u(4)^2-u(3)*u(5)^2+u(5)*u(3)^2,u(6)^10,
2*u(3)*x(7)*u(4)+u(4)*u(2)*u(5)-2*x(7)*u(2)*u(5),
u(3)*u(5)^2-u(5)*u(3)^2-u(4)*u(2)*u(3),
-2*x(7)*u(2)-2*u(3)*x(8)+u(4)*u(2)+u(3)*u(5),
x(11)*u(2)-u(3)*x(10),
-4*x(12)+3*x(10)+u(4),-4*x(13)+3*x(11)+u(5),2*u(3)*x(8)-u(2)^2-u(3)^2,
z(1)*u(2)-u(6),z(2)*u(3)-u(6),
z(3)*(u(5)*u(2)-u(3)*u(4))-u(6);
ideal im=
u(3)*u(5)^2-u(5)*u(3)^2-u(4)*u(2)*u(3),
-4*x(12)+3*x(10)+u(4),-4*x(13)+3*x(11)+u(5),2*u(3)*x(8)-u(2)^2-u(3)^2,
z(3)*(u(5)*u(2)-u(3)*u(4))-u(6);
ideal i=in*im;
ideal j=std( i);

ring r = 32003,(x(1..7)),ds;
ideal i =
-2*x(4)*x(5)-2*x(3)*x(6)^2-2*x(2)*x(7)-1*x(1)^3,
-2*x(1)*x(2)-2*x(4)*x(6)^2-2*x(3)*x(7)-1*x(2)^3,
-2*x(1)*x(3)-2*x(5)*x(6)^2-2*x(4)*x(7)-1*x(3)^3,
-2*x(2)*x(3)-2*x(1)*x(4)-2*x(5)*x(7)-1*x(4)^3,
-2*x(2)*x(5)-2*x(1)^2;
ideal j=std( i);

