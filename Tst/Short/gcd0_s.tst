// $Id$

//
// gcd0_s.tst - short tests for gcd calculations in Z.
//
// All univariate non-trivial examples come from gcdUniv0Std.fex or
// gcdUniv0Alpha.fex.  Some of the examples are multiplied with elements
// from Q to test clearing of denominators.
//
// The examples in variables `u' and `v' came from `coprasse(2/0/1)'
// in stdMultiv0Gcd.in.
//
// To Do:
//
// o multivariate gcd calculations with parameters
// o algrebraic extensions of char 0 not implemented yet
//   (but tests already exist)
//

LIB "tst.lib";
tst_init();
tst_ignore("CVS ID  : $Id$");

//
// - ring r1=0,x,dp.
//

tst_ignore( "ring r1=0,x,dp;" );
ring r1=0,x,dp;

poly f=(-9554*x^4-12895*x^3-10023*x^2-6213*x);
poly g;

// some trivial examples
gcd(0, 0);
gcd(0, 3123);
gcd(4353, 0);

gcd(0, f);
gcd(f, 0);

gcd(23123, f);
gcd(f, 13123);

// some trivial examples involving rational numbers
f=11/47894*19*(-9554*x^4-12895*x^3-10023*x^2-6213*x);

gcd(0, 3123/123456);
gcd(4353/8798798, 0);

gcd(0, f);
gcd(f, 0);

gcd(23123/3, f);
gcd(f, 13123/2);

// some less trivial examples
f=(2*x^3+2*x^2+2*x);
g=(x^3+2*x^2+2*x+1);
gcd(f, g);

f=(-x^9-6*x^8-11*x^7-17*x^6-14*x^5-14*x^4-8*x^3-6*x^2-2*x-1);
g=(-x^9-2*x^8-2*x^7-3*x^4-6*x^3-6*x^2-3*x-1);
gcd(f, g);

f=(4*x^9+12*x^8+29*x^7+42*x^6+54*x^5+48*x^4+35*x^3+17*x^2+6*x+1);
g=(x^11+6*x^10+11*x^9+17*x^8+14*x^7+14*x^6+8*x^5+6*x^4+2*x^3+x^2);
gcd(f, g);
gcd(1/13*f, 1/4*g);

f=(1412500*x^6+6218750*x^5-6910000*x^4-1201250*x^3-19470000*x^2-27277500*x);
g=(156600000*x^5-1363865625*x^4+2627604000*x^3-2727731250*x^2+4628808000*x);
gcd(f, g);

//
// - ring r2=(0,a),x,dp; minpoly=a^4+a^3+a^2+a+1.
//

tst_ignore( "ring r2=(0,a),x,dp;" );
ring r2=(0,a),x,dp;
minpoly=a^4+a^3+a^2+a+1;

poly f=(-9554*x^4-12895*x^3-10023*x^2-6213*x);
poly g;

// first, some of the r1 examples (slightly modified)
gcd(0, 0);
gcd(0, 3123*a);
gcd(4353, 0);

gcd(0, f);
gcd(f, 0);

gcd(23123, f);
gcd(f, 13123);

// some trivial examples involving rational numbers
f=11/47894*19*(-9554*x^4-12895*x^3-10023*x^2-6213*x);

gcd(0, 3123/(123456*a));
gcd(4353/8798798, 0);

gcd(0, f/a);
gcd(f, 0);

gcd(23123/(3*a), f);
gcd(f, 13123/2);

// some less trivial examples
f=(2*x^3+2*x^2+2*x);
g=(x^3+2*x^2+2*x+1);
gcd(f, g);

// we go on with modified variable names
tst_ignore( "ring r2=(0,v),u,dp;" );
kill r2;
ring r2=(0,v),u,dp;
minpoly=v^4+v^3+v^2+v+1;

poly f;
poly g;

// last not least, some less trivial examples
// involving the algebraic variable.  Examples
// are from r3.
f=(-8*u^2*v+8*u*v^2-24*u);
g=(-64*u^2*v+16*u*v^2);
gcd(f, g);

f=(192*u^6*v^4-240*u^5*v^5+384*u^5*v^3+48*u^4*v^6+96*u^4*v^4-576*u^4*v^2-48*u^3*v^5+144*u^3*v^3);
g=(1536*u^6*v^4-768*u^5*v^5-1536*u^5*v^3+96*u^4*v^6+768*u^4*v^4-96*u^3*v^5);
gcd(f, g);

f=(-256*u^3*v+128*u^2*v^2-16*u*v^3);
g=(-64*u^3+48*u^2*v^2+32*u^2*v-192*u^2-12*u*v^3-4*u*v^2+48*u*v);
gcd(f, g);

//
// - ring r3=(0,t),x,dp.
//

tst_ignore( "ring r3=(0,t),x,dp;" );
ring r3=(0,t),x,dp;

poly f=(-9554*x^4-12895*x^3-10023*x^2-6213*x);
poly g;

// first, some of the r1 examples (slightly modified)
gcd(0, 0);
gcd(0, 3123*t);
gcd(4353, 0);

gcd(0, f);
gcd(f, 0);

gcd(23123, f);
gcd(f, 13123);

// some trivial examples involving rational numbers
f=11/47894*19*(-9554*x^4-12895*x^3-10023*x^2-6213*x);

gcd(0, 3123/(123456*t));
gcd(4353/8798798, 0);

gcd(0, f/t);
gcd(f, 0);

gcd(23123/(3*t), f);
gcd(f, 13123/2);

// some less trivial examples
f=(2*x^3+2*x^2+2*x);
g=(x^3+2*x^2+2*x+1);
gcd(f, g);

// we go on with modified variable names
tst_ignore( "ring r3=(0,u),v,dp;" );
kill r3;
ring r3=(0,u),v,dp;

poly f;
poly g;

// last not least, some less trivial examples
// involving the transcendental variable
f=(-8*u^2*v+8*u*v^2-24*u);
g=(-64*u^2*v+16*u*v^2);
gcd(f, g);

f=(192*u^6*v^4-240*u^5*v^5+384*u^5*v^3+48*u^4*v^6+96*u^4*v^4-576*u^4*v^2-48*u^3*v^5+144*u^3*v^3);
g=(1536*u^6*v^4-768*u^5*v^5-1536*u^5*v^3+96*u^4*v^6+768*u^4*v^4-96*u^3*v^5);
gcd(f, g);

f=(-256*u^3*v+128*u^2*v^2-16*u*v^3);
g=(-64*u^3+48*u^2*v^2+32*u^2*v-192*u^2-12*u*v^3-4*u*v^2+48*u*v);
gcd(f, g);

//
// - ring r4=0,(t,x),dp.
//
// The examples from r4 are those from r3 with parameters
// changed to ring variables.
//

tst_ignore( "ring r4=0,(t,x),dp;" );
ring r4=0,(t,x),dp;

poly f=(-9554*x^4-12895*x^3-10023*x^2-6213*x);
poly g;

// first, some of the r1 examples (slightly modified)
gcd(0, 0);
gcd(0, 3123*t);
gcd(4353, 0);

gcd(0, f);
gcd(f, 0);

gcd(23123, f);
gcd(f, 13123);

// some trivial examples involving rational numbers
f=11/47894*19*(-9554*x^4-12895*x^3-10023*x^2-6213*x);

gcd(0, 3123/123456*t);
gcd(4353/8798798, 0);

gcd(23123/3*t, f);
gcd(f, 13123/2);

// some less trivial examples
f=(2*x^3+2*x^2+2*x);
g=(x^3+2*x^2+2*x+1);
gcd(f, g);

// we go on with modified variable names
tst_ignore( "ring r4=0,(u,v),dp;" );
kill r4;
ring r4=0,(u,v),dp;

poly f;
poly g;

// last not least, some less trivial examples
// involving both variables
f=(-8*u^2*v+8*u*v^2-24*u);
g=(-64*u^2*v+16*u*v^2);
gcd(f, g);

f=(192*u^6*v^4-240*u^5*v^5+384*u^5*v^3+48*u^4*v^6+96*u^4*v^4-576*u^4*v^2-48*u^3*v^5+144*u^3*v^3);
g=(1536*u^6*v^4-768*u^5*v^5-1536*u^5*v^3+96*u^4*v^6+768*u^4*v^4-96*u^3*v^5);
gcd(f, g);

f=(-256*u^3*v+128*u^2*v^2-16*u*v^3);
g=(-64*u^3+48*u^2*v^2+32*u^2*v-192*u^2-12*u*v^3-4*u*v^2+48*u*v);
gcd(f, g);
tst_status(1);$
