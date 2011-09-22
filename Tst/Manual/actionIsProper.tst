LIB "tst.lib"; tst_init();
LIB "ainvar.lib";
ring rf=0,x(1..7),dp;
matrix m[7][1];
m[4,1]=x(1)^3;
m[5,1]=x(2)^3;
m[6,1]=x(3)^3;
m[7,1]=(x(1)*x(2)*x(3))^2;
actionIsProper(m);
ring rd=0,x(1..5),dp;
matrix m[5][1];
m[3,1]=x(1);
m[4,1]=x(2);
m[5,1]=1+x(1)*x(4)^2;
actionIsProper(m);
tst_status(1);$
