proc isComplex (list c)
{
  int i;
  module @test;
  for (i=1;i<=size(c)-1;i=i+1)
  {
    @test=c[i]*c[i+1];
    if (size(@test)!=0)
    {
      @test;
      "argument is not a complex at position",i;
      return(0);
    }
  }
  "argument is a complex";
  return(1);
}

ring r;
ideal i=maxideal(2);
matrix A=matrix(i);
A=x*A+y*A+z*A;
i=ideal(A);
i;
list c=koszul(0,i),koszul(1,i),koszul(2,i),koszul(3,i),koszul(4,i);
c;
isComplex(c);
$
