LIB "tst.lib";
tst_init();

// Factorization above the old Factory characteristic limit.
ring r=536871001,x,dp;
list F=factorize(x2-1);
poly f=1;
for (int i=1; i<=size(F[1]); i++)
{
  f=f*F[1][i]^F[2][i];
}
f==x2-1;
size(F[1])==3;

tst_status(1);$
