proc sum
{
  if (#[1] <= 1) { return(#[1]); }
  return (#[1] + sum(#[1] -1));
}
proc fak
{
  if (#[1] <= 1) { return(1); }
  return (#[1] * fak(#[1] -1));
}
proc binom
{
  return (fak(#[1]) / (fak(#[1]-#[2]) * fak(#[2])));
}
"nesting too deep for fak(30)!!!!";fak(30);
int i;
int j;
//for (i = 2; i<=10; i=i+1)
//{
//  i, "sum", sum(i), "fak", fak(i);
//}
//"binom(5,1)", binom(5,1);
//$;
string b;
for (i=1; i<=16; i=i+1)
{
  b = "binoms " + string(i) + " ";
  for (j=0; j<=i; j=j+1)
  {
    b = b + " " + string(binom(i,j));
  }
  b;
}
LIB "tst.lib";tst_status(1);$;
"binom(5,0)", binom(5,0);
"binom(5,1)", binom(5,1);
"binom(5,2)", binom(5,2);
"binom(5,3)", binom(5,3);
"binom(5,4)", binom(5,4);
"binom(5,5)", binom(5,5);
LIB "tst.lib";tst_status(1);$;
