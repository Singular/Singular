LIB "tst.lib"; tst_init();
LIB("syzextra.so");

noop();

proc NegativeNumber(number c)
{
  string s = sprintf("(%s)", c); int i = 1;

  // cannot run ouside of s (with '(((('), right?
  while( s[i] == "(" || s[i] == " " ) { i++; };

  // now should either sign or digit or letter come, right?
  return (s[i] == "-" );
}


proc TestClearContent(def i, number c, def o)
{
  "";
  "Test: ClearContent(", i, " --?-> ", o, " / => ", c, "): ";
  int pass = 1;
  number @c = ClearContent(i);

  if( NegativeNumber(leadcoef(i)) )
  {
    "ERROR: negative leading coeff. after clearing  content: ", leadcoef(i), " instead of ", leadcoef(o);
    pass = 0;
  }
  if( @c != c )
  {
    "ERROR: wrong content: ", @c, " instead of ", c;
    pass = 0;
  }
  if( i != o )
  {
    "ERROR: wrong element after clearing content: ", i, " instead of ", o;
    pass = 0;
  }

  if( pass )
  {
    "[TestClearContent -- PASSED]";
  } else
  {
    basering;
    "ERROR: [TestClearContent -- FAILED]";
    m2_end(666);
  }
  "";
}


proc TestClearDenominators(def i, number c, def o)
{
  def ii = cleardenom(i);
  "";
  "Test: ClearDenominators(", i, " --?-> ", o, " / => ", c, "): ";
  int pass = 1;
  number @c = ClearDenominators(i);

//  if( NegativeNumber(leadcoef(i)) )
//  {
//    "ERROR: negative leading coeff. after clearing denominators: ", leadcoef(i), " instead of ", leadcoef(o);
//    pass = 0;
//  }
  if( @c != c )
  {
    "ERROR: wrong multiplier: ", @c, " instead of ", c;
    pass = 0;
  }
  if( i != o )
  {
    "ERROR: wrong element after clearing denominators: ", i, " instead of ", o;
    pass = 0;
  }

  number cntnt = ClearContent(i); // cleardenom seems to run clearcontent on its own...
  if( i != ii )
  {
    "WARNING/ERROR?: result of clearing denominators: ", i, " is inconsistent with cleardenom(): ", ii;
//    pass = 0;
  }


  if( pass )
  {
    "[TestClearDenominators -- PASSED]";
  } else
  {
    basering;
    "ERROR: [TestClearDenominators -- FAILED]";
    m2_end(666);
  }
  "";
}

proc TestClearRingX(poly X)
{
// // clearcontent:

// with polynomials in 'X'
TestClearContent(poly(1), number(1), poly(1)); // {1} -> {1}, c=1
TestClearContent(poly(2), number(2), poly(1)); // {2} -> {1}, c=2
TestClearContent(poly(222222222222*X + 2), number(2), poly(111111111111*X + 1)); // {222222222222, 2 } -> { 111111111111, 1} c=2
TestClearContent(poly(2*X + 222222222222), number(2), poly(1*X + 111111111111)); // {2, 222222222222 } -> { 1, 111111111111} c=2

// use vector instead:
TestClearContent(vector(1), number(1), vector(1)); // {1} -> {1}, c=1
TestClearContent(vector(2), number(2), vector(1)); // {2} -> {1}, c=2
TestClearContent(vector([222222222222, 2]), number(2), vector([111111111111, 1])); // {222222222222, 2 } -> { 111111111111, 1} c=2
TestClearContent(vector([2, 222222222222]), number(2), vector([1, 111111111111])); // {2, 222222222222 } -> { 1, 111111111111} c=2


// with negative leading coeff!
TestClearContent(-poly(1), -number(1), poly(1)); // {1} -> {1}, c=1
TestClearContent(-poly(2), -number(2), poly(1)); // {2} -> {1}, c=2
TestClearContent(-poly(222222222222*X + 2), -number(2), poly(111111111111*X + 1)); // {222222222222, 2 } -> { 111111111111, 1} c=2
TestClearContent(-poly(2*X + 222222222222), -number(2), poly(1*X + 111111111111)); // {2, 222222222222 } -> { 1, 111111111111} c=2

// use vector instead:
TestClearContent(-vector(1), -number(1), vector(1)); // {1} -> {1}, c=1
TestClearContent(-vector(2), -number(2), vector(1)); // {2} -> {1}, c=2
TestClearContent(-vector([222222222222, 2]), -number(2), vector([111111111111, 1])); // {222222222222, 2 } -> { 111111111111, 1} c=2
TestClearContent(-vector([2, 222222222222]), -number(2), vector([1, 111111111111])); // {2, 222222222222 } -> { 1, 111111111111} c=2




TestClearDenominators(poly(1), number(1), poly(1)); // {1} -> {1}, c=1
TestClearDenominators(poly(2), number(1), poly(2)); // {2} -> {2}, c=1
TestClearDenominators(poly(X + (1/2)), number(2), poly(2*X + 1)); // {1, 1/2 } -> {2, 1}, c=2
TestClearDenominators(poly((1/2)*X + 1), number(2), poly(X + 2)); // {1/2, 1} -> {1, 2}, c=2
TestClearDenominators(poly((1/3)*(X*X*X)+(1/4)*X*X+(1/6)*X+1), number(12), poly(4*X*X*X+3*X*X+2*X+12)); // {1/3, 1/4, 1/6, 1 } -> {4, 3, 2, 12}, c=12
TestClearDenominators(poly((1/2)*X*X*X+(1/4)*X*X+(3/2)*X+111111111111), number(4), poly(2*X*X*X+X*X+6*X+444444444444)); // {1/2, 1/4, 3/2, 111111111111 } -> {2, 1, 6, 444444444444} , c=4



TestClearDenominators(vector([1]), number(1), vector([1])); // {1} -> {1}, c=1
TestClearDenominators(vector([2]), number(1), vector([2])); // {2} -> {2}, c=1
TestClearDenominators(vector([1, 1/2]), number(2), vector([2, 1])); // {1, 1/2} -> {2, 1}, c=2
TestClearDenominators(vector([1/2, 1]), number(2), vector([1, 2])); // {1/2, 1} -> {1, 2}, c=2
TestClearDenominators(vector([1/3,1/4,1/6,1]), number(12), vector([4,3,2,12])); // {1/3, 1/4, 1/6, 1 } -> {4, 3, 2, 12}, c=12
TestClearDenominators(vector([1/2,1/4,3/2,111111111111]), number(4), vector([2,1,6,444444444444])); // {1/2, 1/4, 3/2, 111111111111 } -> {2, 1, 6, 444444444444} , c=4

/*
TestClearDenominators(-poly(1), -number(1), poly(1)); // {1} -> {1}, c=1
TestClearDenominators(-poly(2), -number(1), poly(2)); // {2} -> {2}, c=1

TestClearDenominators(-poly(X + (1/2)), -number(2), poly(2*X + 1)); // {1, 1/2 } -> {2, 1}, c=2
TestClearDenominators(-poly((1/2)*X + 1), -number(2), poly(X + 2)); // {1/2, 1} -> {1, 2}, c=2
TestClearDenominators(-poly((1/3)*(X*X*X)+(1/4)*X*X+(1/6)*X+1), -number(12), poly(4*X*X*X+3*X*X+2*X+12)); // {1/3, 1/4, 1/6, 1 } -> {4, 3, 2, 12}, c=12
TestClearDenominators(-poly((1/2)*X*X*X+(1/4)*X*X+(3/2)*X+111111111111), -number(4), poly(2*X*X*X+X*X+6*X+444444444444)); // {1/2, 1/4, 3/2, 111111111111 } -> {2, 1, 6, 444444444444} , c=4

TestClearDenominators(-vector([1]), -number(1), vector([1])); // {1} -> {1}, c=1
TestClearDenominators(-vector([2]), -number(1), vector([2])); // {2} -> {2}, c=1
TestClearDenominators(-vector([1, 1/2]), -number(2), vector([2, 1])); // {1, 1/2} -> {2, 1}, c=2
TestClearDenominators(-vector([1/2, 1]), -number(2), vector([1, 2])); // {1/2, 1} -> {1, 2}, c=2
TestClearDenominators(-vector([1/3,1/4,1/6,1]), -number(12), vector([4,3,2,12])); // {1/3, 1/4, 1/6, 1 } -> {4, 3, 2, 12}, c=12
TestClearDenominators(-vector([1/2,1/4,3/2,111111111111]), -number(4), vector([2,1,6,444444444444])); // {1/2, 1/4, 3/2, 111111111111 } -> {2, 1, 6, 444444444444} , c=4
*/

}

ring R = 0, (x), dp;
TestClearRingX(x);
kill R;

ring R = 0, (x, y, z), dp;
TestClearRingX(x);
TestClearRingX(y);
TestClearRingX(z);

TestClearRingX(xy);
TestClearRingX(yz);
TestClearRingX(xz);

TestClearRingX(xyz);


TestClearContent(poly(9x2y2z-18xyz2-18xyz+18z2+18z), number(9), poly(x2y2z-2xyz2-2xyz+2z2+2z)); // I[6]: Manual/Generalized_Hilbert_Syzygy_Theorem.tst
TestClearContent(-poly(9x2y2z-18xyz2-18xyz+18z2+18z), -number(9), poly(x2y2z-2xyz2-2xyz+2z2+2z)); // -_


TestClearContent(poly(4x3+2xy3), number(2), poly(2x3+xy3)); // j[1]: Old/err3.tst
TestClearContent(-poly(4x3+2xy3), -number(2), poly(2x3+xy3)); // j[1]: Old/err3.tst

TestClearContent(poly(2xy), number(2), poly(xy)); // _[2]: Manual/Delta.tst
TestClearContent(poly(6x2z+2y2z), number(2), poly(3x2z+y2z)); // _[3]: Manual/Delta.tst

kill R;
ring R=0,(x,y),dp;
TestClearRingX(x);
TestClearRingX(y);
TestClearRingX(xy);

TestClearDenominators(poly(1/2x2 + 1/3y), number(6), poly(6*(1/2x2 + 1/3y)));


1/2x2 + 1/3y; cleardenom(_);


kill R;
ring R = (0, m1, m2, M, g, L), (Dt), (dp(1), C);

// Manual/canonize.tst
(-g)/(m2)*gen(3)+(-m1*g)/(m2^2)*gen(2);

TestClearDenominators((-g)/(m2)*gen(3)+(-m1*g)/(m2^2)*gen(2), number((m2^2)), (-m2*g)*gen(3)+(-m1*g)*gen(2));

// cleardenom(_);



kill R;
ring R = (0, I, T, Px, Py, Cx, Cy, Sx, Sy, a, b, dt, dx, dy), (i, t, x, y, cx, cy, sx, sy), (c, lp(8));
ideal q= cy^2+sy^2-1, cx^2+sx^2-1, i^2+1;
q = std(q);
q;

qring Q = q;
basering;

number n = (Cy^2*dt^2);

cleardenom(n);

n;

denominator(n);

numerator(n);

kill R;


kill R;
ring R = (0,t), (x), dp;

TestClearRingX(x);

minpoly = t^2 + 1;
TestClearRingX(x);

// the following tests are wrong: t never appears in denominators (due to minpoly)
// [(1/(2t)), 1] -> [1, (2t)], (2t)

[(1/(2t)), 1];
cleardenom(_);

[((1/3)/(2t)), 1];
cleardenom(_);

// TestClearDenominators(vector([(1/(2t)), 1]), number(2t), vector([1, (2t)]));
// alg:  [((1/3)/(2t)), 1] ->[1, (6t)], (6t)
// TestClearDenominators(vector([((1/3)/(2t)), 1]), number(6t), vector([1, (6t)]));

// trans. [((1/3)/(2t)), 1] -> [(1/3), (2t)], (2t) ???





/*
< _[1]=8x4l*gen(3)-16x3l2*gen(1)+8x2l3*gen(2)-8x*gen(2)+8y*gen(1)-8z*gen(3)
< _[2]=8x5*gen(3)-16x4l*gen(1)+8x3l2*gen(2)+8y*gen(2)-8z*gen(1)
---
> _[1]=33554432x4l*gen(3)-67108864x3l2*gen(1)+33554432x2l3*gen(2)-33554432x*gen(2)+33554432y*gen(1)-33554432z*gen(3)
> _[2]=33554432x5*gen(3)-67108864x4l*gen(1)+33554432x3l2*gen(2)+33554432y*gen(2)-33554432z*gen(1)


< j[3]=11x4y2+9x5+9x7
---
> j[3]=55x4y2+45x5+45x7



< qsat[1]=12zt+3z-10
< qsat[2]=5z2+12xt+3x
< qsat[3]=144xt2+72xt+9x+50z
---
> qsat[1]=21600zt+5400z-18000
> qsat[2]=9000z2+21600xt+5400x
> qsat[3]=259200xt2+129600xt+16200x+90000z




< q[1]=12zty+3zy-10y2
< q[2]=60z2t-36xty-9xy-50zy
< q[3]=12xty2+5z2y+3xy2
< q[4]=z3y+2xy3
---
> q[1]=216zty+54zy-180y2
> q[2]=1080z2t-648xty-162xy-900zy
> q[3]=648xty2+270z2y+162xy2
> q[4]=270z3y+540xy3



< qsat[1]=12zt+3z-10y
< qsat[2]=12xty+5z2+3xy
< qsat[3]=144xt2+72xt+9x+50z
< qsat[4]=z3+2xy2
---
> qsat[1]=23328000zt+5832000z-19440000y
> qsat[2]=233280000xty+97200000z2+58320000xy
> qsat[3]=279936000xt2+139968000xt+17496000x+97200000z
> qsat[4]=97200000z3+194400000xy2



*/


/*
experiments master Singular (together with Claus / exts. of Q):

> ring R = (0,t), x, dp;
> cleardenom ( [1/(2t), 1] );
(2t)*gen(2)+gen(1)
> cleardenom ( [(1/3)/(2t), 1] );
(6t)*gen(2)+gen(1)
> number a = 1/3;
> a;
1/3
> poly p = (a / (2t)) * x + 1;
> p;
1/(6t)*x+1
> poly p = ((a + t) / (2t+1)) * x + 1;
// ** redefining p **
> p;
(3t+1)/(6t+3)*x+1
> ^Z



 ring R = (0,t), x, dp;cleardenom ( [(1/3)/(2t), 1] );
(6t)*gen(2)+gen(1)
> ring R = (0,t), x, dp; minpoly = t2 + 1; cleardenom ( [(1/3)/(2t), 1] );
6*gen(2)+(-t)*gen(1)
>  [(1/3)/(2t), 1];
gen(2)+(-1/6t)*gen(1)



*/


kill R;
ring R = (0,a), (x, y, z, u, v), (a(1, 2, 0, 0, 0), ws(1, 2, 3, 4, 5), C);

// poly pp = 1/3*x3+1/4*x2+1/6*x+1 ; ClearDenominators(pp); pp;

TestClearRingX(x);

minpoly = a2 + 1;

R;
//   characteristic : 0
//   1 parameter    : a
//   minpoly        : (a2+1)
//   number of vars : 5
//        block   1 : ordering a
//                  : names    x y z u v
//                  : weights  1 2 0 0 0
//        block   2 : ordering ws
//                  : names    x y z u v
//                  : weights  1 2 3 4 5
//        block   3 : ordering C

option(); //options: intStrategy redefine usage prompt
attrib(I); //          no attributes

(a)*x2+(2a)*xv+(a)*v2;
cleardenom(_); // x2+2*xv+v2

ideal I = y2+(a)*x+v3, (-a)*y4-2*y2v+(-a)*y2v3-xv3+(a)*v2-2*v4;

poly(2a);
cleardenom(_); // 1??

poly(-2a);
cleardenom(_); // 1??


(-2a)*x + (4a)*y;

cleardenom(_); // 1??

kill Q;

poly P = (2a)*x;
poly Q = (4a)*y;

gcd(P, Q);
gcd(-P, Q);
gcd(P, -Q);
gcd(-P, -Q);


y2+(a)*x+v3;
cleardenom(_);

I[2] + a* y2 * I[1];
cleardenom(_);



"GB: "; groebner(I); // _[1]=x2+2*xv+v2 _[2]=y2+(a)*x+v3
"SB: "; std(I);      // _[1]=x2+2*xv+v2 _[2]=y2+(a)*x+v3


tst_status(1);$


exit;


