LIB "tst.lib";tst_init();
proc test3 (int p)
{
  ring r= p, x(1..11), dp;
  poly d1= x(1)^2 + x(2)^2 + 1;
  poly f1= x(1)^2 - x(2)^2 - 2;
  poly g1= x(1)^2 + x(2)^2 + 2;
  poly f= d1*f1;
  poly g= d1*g1;
  poly testgcd= gcd (f,g);
  testgcd= testgcd/leadcoef (testgcd);
  testgcd == d1;
  int i, k;
  poly d, g2, f2;
  for (i= 3; i <= 11; i++)
  {
    d= x(1)^i;
    for (k= 2; k <= i; k++)
    {
      d= d + x(k)^i;
    }
    g2= d;
    f2= d;
    d= d + 1;
    f2= f2 - 2;
    g2= g2 + 2;
    f= f2*d;
    g= g2*d;
    testgcd= gcd (f,g);
    testgcd= testgcd/leadcoef (testgcd);
    testgcd == d;
  }
}

test3 (3);
tst_status();
test3 (43051);
tst_status();
test3 (0);
tst_status();
tst_status(1);
$
