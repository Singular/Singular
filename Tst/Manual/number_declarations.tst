LIB "tst.lib"; tst_init();
  // finite field Z/p, p<= 32003
  ring r = 32003,(x,y,z),dp;
  number n = 4/6;
  n;
  // finite field GF(p^n), p^n <= 32767
  // z is a primitive root of the minimal polynomial
  ring rg= (7^2,z),x,dp;
  number n = 4/9+z;
  n;
  // the rational numbers
  ring r0 = 0,x,dp;
  number n = 4/6;
  n;
  // algebraic extensions of Z/p or Q
  ring ra=(0,a),x,dp;
  minpoly=a^2+1;
  number n=a3+a2+2a-1;
  n;
  a^2;
  // transcedental extensions of Z/p or Q
  ring rt=(0,a),x,dp;
  number n=a3+a2+2a-1;
  n;
  a^2;
  // machine floating point numbers, single precision
  ring R_0=real,x,dp;
  number n=4/6;
  n;
  n=0.25e+2;
  n;
  // floating point numbers, arbitrary prescribed precision
  ring R_1=(real,50),x,dp;
  number n=4.0/6;
  n;
  n=0.25e+2;
  n;
  // floating point complex numbers, arbitrary prescribed precision
  // the third parameter gives the name of the imaginary unit
  ring R_2=(complex,50,i),x,dp;
  number n=4.0/6;
  n;
  n=0.25e+2*i+n;
  n;
tst_status(1);$
