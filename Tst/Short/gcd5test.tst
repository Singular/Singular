LIB "tst.lib";tst_init();
proc test5 (int p)
{
  ring r= p, x(1..11), dp;
  poly d1= x(1) + 1;
  poly d2= x(2) + 1;
  poly f1= x(1) - 2;
  poly f2= x(2) - 2;
  poly g1= x(1) + 2;
  poly g2= x(2) + 2;
  poly d= d1*d2 - 2;
  poly f= f1*f2 + 2;
  poly g= g1*g2 - 2;
  f= f*d;
  g= g*d;
  poly testgcd= gcd (f,g);
  testgcd= testgcd/leadcoef (testgcd);
  testgcd == d;
  int i;
  for (i= 3; i <= 11; i++)
  {
    d2= d2*(x(i) + 1);
    f2= f2*(x(i) - 2);
    g2= g2*(x(i) + 2);

    d= d1*d2 - 2;
    f= f1*f2 + 2;
    g= g1*g2 - 2;
    f= f*d;
    g= g*d;
    testgcd= gcd (f,g);
    testgcd= testgcd/leadcoef (testgcd);
    testgcd == d;
  }
}

test5 (3);
tst_status();
test5 (43051);
tst_status();
test5 (0);
tst_status();
tst_status(1);
$
