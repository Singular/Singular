LIB "tst.lib";
tst_init();

int n=2;
int m=3;
ring A1 = 0,(x(1..n),y(1..m)),(dp(n),ds(m));
poly f  = x(1)*x(2)^2+1+y(1)^10+x(1)*y(2)^5+y(3);
f;

1>y(1)^10;  

ring A2 = 0,(x(1..n),y(1..m)),(ds(n),dp(m));
fetch(A1,f);

x(1)*y(2)^5<1;

ring A3 = 0,(x(1..n),y(1..m)),(dp(n),ds(2),dp(m-2));
fetch(A1,f);

tst_status(1);$
