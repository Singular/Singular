LIB "tst.lib";
tst_init();
LIB "general.lib";
example A_Z;
example ASCII;
example binomial;
example deleteSublist;
example factorial;
example fibonacci;
//example kmemory;
example killall;
example number_e;
example number_pi;
example primes;
example product;
example sort;
example sum;

ring r=0,(x,y,z),dp;
poly f=x^30+y^30;
def l = watchdog(1,"factorize(eval("+string(f)+"))");
int ok = 0;
if (typeof(l) == "string")
{
  if (l == "Killed")
  {
    ok = 1;
  }
}
else
{
  poly pp = x16+x14y2-x10y6-x8y8-x6y10+x2y14+y16;
  if (typeof(l) == "list")
  {
    if (l[1][2] == pp)    
    {
     ok = 1;
    }
  }
}
ok;
watchdog(100,"factorize(eval("+string(f)+"))");

    

tst_status(1);$
