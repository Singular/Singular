LIB "tst.lib";
tst_init();

LIB "sing.lib";
ring R=32003,(x(1..6),y(1..6)),ds;
poly f1=y(1)^7-x(1)^9+x(1)^4*y(1)^4;
poly f2=y(2)^7-x(2)^9+x(2)^4*y(2)^4;
poly f3=y(3)^7-x(3)^9+x(3)^4*y(3)^4;
poly f4=y(4)^7-x(4)^9+x(4)^4*y(4)^4;
poly f5=y(5)^7-x(5)^9+x(5)^4*y(5)^4;
poly f6=y(6)^7-x(6)^9+x(6)^4*y(6)^4;
poly g=f1+f2+f3+f4+f5+f6;
milnor(g);
tjurina(g);

tst_status(1);$

