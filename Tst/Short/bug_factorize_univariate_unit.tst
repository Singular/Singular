LIB "tst.lib";
tst_init();

// The internal univariate factorizer used to Hensel-lift the modular leading
// coefficient as a polynomial factor, producing a wrong exponent for x.
ring R = 0,x,dp;
poly f = (1024*x^2-4160*x+4245)^2*(32*x-65)^4;
list factors = factorize(f);
poly product = 1;
for (int i = 1; i <= size(factors[1]); i++)
{
  product = product*(factors[1][i]^factors[2][i]);
}
product == f;

tst_status(1);$
