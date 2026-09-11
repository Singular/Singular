LIB "tst.lib"; tst_init();
LIB "freegb.lib";

ring r = integer,(b,a),Dp;
ring R = freeAlgebra(r,11);

// Expected bases below follow from b=1-a and b^n=0. Do not compute
// the reference basis with twostd: it must detect lost generators.
proc checkIdeal(ideal I, ideal expected)
{
  attrib(expected,"isSB",1);
  ideal J = twostd(I);
  return(intvec(size(reduce(I,J)), size(reduce(expected,J)),
                size(reduce(J,expected))));
}

ideal I = b*b*a+b*a+a-1,b*b*b;
ideal expected = b+a-1,(a-1)^3;
option(noredSB); option(noredTail);
checkIdeal(I,expected);
option(redSB);
checkIdeal(I,expected);
option(noredSB); option(redTail);
checkIdeal(I,expected);
option(redSB);
checkIdeal(I,expected);

// (1-b)*f+b^n*a=b+a-1 for f=(1+b+...+b^(n-1))*a-1.
// Vary the overlaps while retaining an independently known basis.
int n,k;
poly f;
for (n=2; n<=5; n++)
{
  f = -1;
  for (k=0; k<n; k++) { f = f+b^k*a; }
  I = f,b^n;
  expected = b+a-1,(a-1)^n;
  checkIdeal(I,expected);
}

// Multiplying every generator by 2 multiplies the strong basis by 2.
// This also exercises coefficients that are not units in Z.
I = 2*(b*b*a+b*a+a-1),2*b*b*b;
expected = 2*(b+a-1),2*(a-1)^3;
checkIdeal(I,expected);

tst_status(1);$
