proc fak2
{
  if (#[1] <= #[2]) { return(#[2]); }
  return (#[1] * fak2(#[1] -1,#[2]));
}

proc fak
{
  return (fak2(#[1],1));
}

proc binom
{
  if (#[2] == 0) { return(1); }
  return ( fak2(#[1],#[1]-#[2]+1) div fak(#[2]) );
}
// -------------------------------------------------------

int i;
int j;
string b;
for (i=1; i<=5; i=i+1)
{
  b = "binomial coefficients " + string(i) + " ";
  for (j=0; j<=i; j=j+1)
  {
    b = b + " " + string(binom(i,j));
  }
  b;
}
listvar(all);
LIB "tst.lib";tst_status(1);$;
