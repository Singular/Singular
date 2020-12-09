// $Id$

// sba_s.tst -- long tests for sba
// uses rcyclic examples from ISSAC'98 paper

LIB "tst.lib";
tst_init();

LIB"polylib.lib";

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

string bench = econ(11);
sprintf(bench);
ideal f;
f = sba(i,0,1);
killall;
string bench = econh(11);
sprintf(bench);
ideal f;
f = sba(i,1,1);
killall;

tst_status(1);$
LIB "polylib.lib";
killall();
killall("proc");
exit;
