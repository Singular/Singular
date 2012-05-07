LIB "tst.lib";tst_init();
proc test2 (int p)
{
  ring r= p, x(1..11), dp;
  poly d1= x(1) + x(2) + 1;
  poly f1= x(1) - x(2) - 2;
  poly g1= x(1) + x(2) + 2;
  poly d= d1^2;
  poly f= d*f1^2;
  poly g= d*g1^2;
  poly testgcd= gcd (f,g);
  testgcd= testgcd/leadcoef (testgcd);
  testgcd == d;
  int i;
  for (i= 3; i <= 11; i++)
  {
    d1= d1 + x(i);
    f1= f1 - x(i);
    g1= g1 + x(i);
    d= d1^2;
    f= (f1^2)*d;
    g= (g1^2)*d;
    testgcd= gcd (f, g);
    testgcd= testgcd/leadcoef (testgcd);
    testgcd == d;
  }
}

test2 (3);
tst_status();
test2 (43051);
tst_status();
test2 (0);
tst_status();
tst_status(1);
$

