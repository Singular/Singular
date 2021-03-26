// $Id$

// sba_s.tst -- long tests for sba
// uses rcyclic examples from ISSAC'98 paper

LIB "tst.lib";
tst_init();

LIB"polylib.lib";

proc katsuran(int z)
{
   int n = z;
   ring R = 32003,(x(0..(n))),dp;
   ideal i = katsura(n);
        string bench = "KATSURA-"+string(n);
   export(R);
   export(i);
   return(bench);
}

proc katsuranh(int z)
{
   int n = z;
   ring R = 32003,(x(0..(n)),h),dp;
   ideal i = homog(katsura(n),h);
        string bench = "KATSURA-"+string(n)+"-H";
   export(R);
   export(i);
   return(bench);
}

proc cyclicn(int z)
{
   int n = z;
   ring R = 32003,(x(0..(n-1))),dp;
   ideal i = cyclic(n);
        string bench = "CYCLIC-"+string(n);
   export(R);
   export(i);
   return(bench);
}

proc cyclicnh(int z)
{
   int n = z;
   ring R = 32003,(x(0..(n-1)),h),dp;
   ideal i = homog(cyclic(n),h);
        string bench = "CYCLIC-"+string(n)+"-H";
   export(R);
   export(i);
   return(bench);
}

proc econ(int n) {

  int z = n-1;
  ring R = 32003,(x(0..z)),dp;

  int m,j,k;
  ideal i;
  poly p;
  p = 1;
  for(m=0;m<=z-2;m++) {
    p = p*x(m)*x(z);
    k = 0;
    for(j=m+1;j<=z-1;j++) {
      p = p + x(k)*x(j)*x(z);
      k++;
    }
    p = p - (m+1);
    i[m+1] = p;
    p = 1;
  }
  i[n-1]  = x(z-1)*x(z)-(z);
  p = 0;
  for(m=0;m<=z-1;m++) {
    p = p + x(m);
  }
  p = p + 1;
  i[n] = p;

        string bench = "ECO-"+string(z+1);
  export(i);
  export(R);

  return(bench);
}

proc econh(int n) {

  int z = n-1;
  ring R = 32003,(x(0..z),h),dp;

  int m,j,k;
  ideal i;
  poly p;
  p = 1;
  for(m=0;m<=z-2;m++) {
    p = p*x(m)*x(z)*h;
    k = 0;
    for(j=m+1;j<=z-1;j++) {
      p = p + x(k)*x(j)*x(z);
      k++;
    }
    p = p - (m+1)*h^3;
    i[m+1] = p;
    p = 1;
  }
  i[n-1]  = x(z-1)*x(z)-(z)*h^2;
  p = 0;
  for(m=0;m<=z-1;m++) {
    p = p + x(m);
  }
  p = p + h;
  i[n] = p;

        string bench = "ECO-"+string(z+1)+"-H";
  export(i);
  export(R);

  return(bench);
}

int k;
for (k=6; k>2; k--)
{
  string bench = cyclicn(k);
  sprintf(bench);
  ideal f;
  f = sba(i,3,0);
  f = sba(i,3,1);
  f = sba(i,2,0);
  f = sba(i,2,1);
  f = sba(i,1,0);
  f = sba(i,1,1);
  f = sba(i,0,0);
  f = sba(i,0,1);
  kill i,f,R,bench;
  string bench = cyclicnh(k);
  sprintf(bench);
  ideal f;
  f = sba(i,3,0);
  f = sba(i,3,1);
  f = sba(i,2,0);
  f = sba(i,2,1);
  f = sba(i,1,0);
  f = sba(i,1,1);
  f = sba(i,0,0);
  f = sba(i,0,1);
  kill i,f,R,bench;
  string bench = katsuran(k);
  sprintf(bench);
  ideal f;
  f = sba(i,3,0);
  f = sba(i,3,1);
  f = sba(i,2,0);
  f = sba(i,2,1);
  f = sba(i,1,0);
  f = sba(i,1,1);
  f = sba(i,0,0);
  f = sba(i,0,1);
  kill i,f,R,bench;
  string bench = katsuranh(k);
  sprintf(bench);
  ideal f;
  f = sba(i,3,0);
  f = sba(i,3,1);
  f = sba(i,2,0);
  f = sba(i,2,1);
  f = sba(i,1,0);
  f = sba(i,1,1);
  f = sba(i,0,0);
  f = sba(i,0,1);
  kill i,f,R,bench;
  string bench = econ(k);
  sprintf(bench);
  ideal f;
  f = sba(i,3,0);
  f = sba(i,3,1);
  f = sba(i,2,0);
  f = sba(i,2,1);
  f = sba(i,1,0);
  f = sba(i,1,1);
  f = sba(i,0,0);
  f = sba(i,0,1);
  kill i,f,R,bench;
  string bench = econh(k);
  sprintf(bench);
  ideal f;
  f = sba(i,3,0);
  f = sba(i,3,1);
  f = sba(i,2,0);
  f = sba(i,2,1);
  f = sba(i,1,0);
  f = sba(i,1,1);
  f = sba(i,0,0);
  f = sba(i,0,1);
  kill i,f,R,bench;
}

tst_status(1);$
LIB "polylib.lib";
killall();
killall("proc");
exit;
