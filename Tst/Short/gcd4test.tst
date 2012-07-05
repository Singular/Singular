LIB "tst.lib";tst_init();
proc test4 (int p)
{
  ring r= p, x(1..11), dp;
  poly d= x(1)^2*x(2)^2 + 1;
  poly f1= x(1)^2 - x(2)^2 - 1;
  poly g1= x(1)*x(2) + 2;
  poly f= d*f1;
  poly g= d*g1^2;
  poly testgcd= gcd (f,g);
  testgcd= testgcd/leadcoef (testgcd);
  testgcd == d;
  int i;
  for (i= 3; i <= 11; i++)
  {
    d= d + x(i)^2;
    f1= f1 + x(i)^2;
    g1= g1 + x(i);
    f= f1*d;
    g= d*g1^2;
    testgcd= gcd (f,g);
    testgcd= testgcd/leadcoef (testgcd);
    testgcd == d;
  }
}

test4 (3);
tst_status();
test4 (43051);
tst_status();
test4 (0);
tst_status();
tst_status(1);
$
