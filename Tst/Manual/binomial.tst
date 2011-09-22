LIB "tst.lib"; tst_init();
LIB "general.lib";
binomial(200,100);"";                 //type bigint
int n,k = 200,100;
bigint b1 = binomial(n,k);
ring r = 0,x,dp;
poly b2 = coeffs((x+1)^n,x)[k+1,1];  //coefficient of x^k in (x+1)^n
b1-b2;                               //b1 and b2 should coincide
tst_status(1);$
