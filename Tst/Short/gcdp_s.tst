// $Id$

//
// gcdp_s.tst - short tests for gcd calculations mod p.
//
// All univariate non-trivial examples come from gcdUnivP.fex
// or gcdUnivPAlpha.fex.
//
// The example in variables `a', `c', `e', ... came from `untitled2(6/p/1)'
// in stdMultivPGcd.in.
//
// To Do:
//
// o multivariate gcd calculations with parameters
//

LIB "tst.lib";
tst_init();
tst_ignore("CVS ID  : $Id$");

//
// - functions.
//

//
// simplifyGcd() - print gcd as well as normalized gcd.
//
proc simplifyGcd ( poly f, poly g )
{
  poly d=gcd( f, g );
  d;
  simplify( d, 1 );
}

//
// - ring r1=32003,x,dp.
//

tst_ignore( "ring r1=32003,x,dp;" );
ring r1=32003,x,dp;

poly f=-9554*x^4-12895*x^3-10023*x^2-6213*x;
poly g;

// some trivial examples
gcd(0, 0);
gcd(0, 3123);
gcd(4353, 0);
simplifyGcd(0, 0);
simplifyGcd(0, 3123);
simplifyGcd(4353, 0);

simplifyGcd(0, f);
simplifyGcd(f, 0);

simplifyGcd(23123, f);
simplifyGcd(f, 13123);

// some less trivial examples
f=-9554*x^4-12895*x^3-10023*x^2-6213*x;
g=-9554*x^3-3341*x^2+6213*x;
simplifyGcd(f, g);

f=-11265*x^6+12161*x^5+10369*x^4-12161*x^3+896*x^2;
g=10669*x^8-10673*x^7+5*x^6+8*x^5-10681*x^4+10665*x^3+7*x^2;
simplifyGcd(f, g);

f=-7918*x^9-14406*x^8-7256*x^7+2092*x^6-2198*x^5-12539*x^4-14631*x^3-7150*x^2;
g=-14833*x^13-7011*x^12+15121*x^11-10864*x^10+12943*x^9-9871*x^8+10354*x^7-1437*x^6+6604*x^5-10394*x^4-3231*x^3-9348*x^2-2092*x;
simplifyGcd(f, g);

//
// - ring r2=(32003,a),x,dp; minpoly=a^4+8734*a^3+a^2+11817*a+1.
//

tst_ignore( "ring r2=(32003,a),x,dp;" );
ring r2=(32003,a),x,dp;
minpoly=a^4+8734*a^3+a^2+11817*a+1;

poly f=-9554*x^4-12895*x^3-10023*x^2-6213*x;
poly g;

// first, some of the r1 examples
simplifyGcd(0, 0);
simplifyGcd(0, 3123);
simplifyGcd(4353, 0);

simplifyGcd(0, f);
simplifyGcd(f, 0);

simplifyGcd(23123, f);
simplifyGcd(f, 13123);

// some less trivial examples
f=-9554*x^4-12895*x^3-10023*x^2-6213*x;
g=-9554*x^3-3341*x^2+6213*x;
simplifyGcd(f, g);

f=-11265*x^6+12161*x^5+10369*x^4-12161*x^3+896*x^2;
g=10669*x^8-10673*x^7+5*x^6+8*x^5-10681*x^4+10665*x^3+7*x^2;
simplifyGcd(f, g);

// now, examples involving the algebraic variable
f=(21147*a^3*x^5+10147*a^3*x^4+8142*a^2*x^4+27671*a*x^4+29289*x^4+29289*a^3*x^3);
g=(6167*a^3*x^6+1536*a^2*x^6+5211*a*x^6+1536*x^6+17534*a^3*x^5+478*a^2*x^5+1536*a*x^5+19085*x^5+3203*a^3*x^4+26578*a^2*x^4+17052*a*x^4+26770*x^4+26002*a^3*x^3+24062*a^2*x^3+192*a*x^3+6386*x^3);
simplifyGcd(f, g);

f=(2295*a^2*x^11+5897*a^3*x^10+765*a^2*x^10+27718*a^3*x^9+1283*a^2*x^9+23792*a*x^9+518*x^9+22026*a^3*x^8+11468*a^2*x^8+16834*a*x^8+24778*x^8+28460*a^3*x^7+8140*a^2*x^7+28036*a*x^7+1613*x^7+18196*a^3*x^6+13274*a^2*x^6+4638*a*x^6+31194*x^6+12206*a^3*x^5+10349*a^2*x^5+30979*a*x^5+8612*x^5);
g=(8192*a^3*x^10+8957*a^2*x^10+9659*a^3*x^9+28672*a^2*x^9+4553*a*x^9+12288*x^9+5819*a^3*x^8+320*a^2*x^8+14380*a*x^8+12104*x^8+12047*a^3*x^7+19823*a^2*x^7+6824*a*x^7+4335*x^7+29376*a^3*x^6+22190*a^2*x^6+31239*a*x^6+22647*x^6);
simplifyGcd(f, g);

//
// - ring r3=(32003,t),x,dp.
//

tst_ignore( "ring r3=(32003,t),x,dp;" );
ring r3=(32003,t),x,dp;

poly f=(-9554*x^4-12895*x^3-10023*x^2-6213*x);
poly g;

// first, some of the r1 examples (slightly modified)
simplifyGcd(0, 0);
simplifyGcd(0, 3123*t);
simplifyGcd(4353, 0);

simplifyGcd(0, f/t);
simplifyGcd(f, 0);

simplifyGcd(23123/t, f);
simplifyGcd(f, 13123);

// some less trivial examples
f=-9554*x^4-12895*x^3-10023*x^2-6213*x;
g=-9554*x^3-3341*x^2+6213*x;
simplifyGcd(f, g);

// we go on with modified variable names
tst_ignore( "ring r3=(32003,a,c,e),(f,h,k),dp;" );
kill r3;
ring r3=(32003,a,c,e),(f,h,k),dp;

poly fPoly;
poly gPoly;

fPoly=(16001*c^2*f^2*k^2-16001*c*e*f*k^3+c*f^3*h*k-e*f^2*h*k^2);
gPoly=(c^3*e*f*h-4*c*e^2*f*h^2);
simplifyGcd(fPoly, gPoly);

fPoly=(c^4*k^2-5*c^3*f*h*k+4*c^2*e*h*k^2+4*c^2*f^2*h^2-4*c*e*f*h^2*k);
gPoly=(-c^3*f*k^2+c^2*e*k^3+2*c^2*f^2*h*k-2*c*e*f*h*k^2);
simplifyGcd(fPoly, gPoly);

fPoly=(-8001*c^4*e*f*k^6+8001*c^4*f^3*h*k^4+8001*c^3*e^2*k^7+8001*c^3*e*f^2*h*k^5+16001*c^3*f^4*h^2*k^3+16001*c^2*e^2*f*h*k^6-16001*c^2*e*f^3*h^2*k^4);
gPoly=(16001*c^7*e*h*k^3-15998*c^6*e*f*h^2*k^2-7*c^5*e*f^2*h^3*k-8*c^4*e^2*f*h^3*k^2+4*c^4*e*f^3*h^4+8*c^3*e^3*h^3*k^3+24*c^3*e^2*f^2*h^4*k-24*c^2*e^3*f*h^4*k^2-16*c^2*e^2*f^3*h^5+16*c*e^3*f^2*h^5*k);
simplifyGcd(fPoly, gPoly);

//
// - ring r4=32003,(t,x),dp.
//
// The examples from r4 are those from r3 with parameters
// changed to ring variables.
//

tst_ignore( "ring r4=32003,(t,x),dp;" );
ring r4=32003,(t,x),dp;

poly f=(-9554*x^4-12895*x^3-10023*x^2-6213*x);
poly g;

// first, some of the r1 examples (slightly modified)
simplifyGcd(0, 0);
simplifyGcd(0, 3123*t);
simplifyGcd(4353, 0);

simplifyGcd(0, f);
simplifyGcd(f, 0);

simplifyGcd(23123*t, f);
simplifyGcd(f, 13123);

// some less trivial examples
f=-9554*x^4-12895*x^3-10023*x^2-6213*x;
g=-9554*x^3-3341*x^2+6213*x;
simplifyGcd(t*f, g);

// we go on with modified variable names
tst_ignore( "ring r4=32003,(a,c,e,f,h,k),dp;" );
kill r4;
ring r4=32003,(a,c,e,f,h,k),dp;

poly fPoly;
poly gPoly;

fPoly=(16001*c^2*f^2*k^2-16001*c*e*f*k^3+c*f^3*h*k-e*f^2*h*k^2);
gPoly=(c^3*e*f*h-4*c*e^2*f*h^2);
simplifyGcd(fPoly, gPoly);

fPoly=(c^4*k^2-5*c^3*f*h*k+4*c^2*e*h*k^2+4*c^2*f^2*h^2-4*c*e*f*h^2*k);
gPoly=(-c^3*f*k^2+c^2*e*k^3+2*c^2*f^2*h*k-2*c*e*f*h*k^2);
simplifyGcd(fPoly, gPoly);

fPoly=(-8001*c^4*e*f*k^6+8001*c^4*f^3*h*k^4+8001*c^3*e^2*k^7+8001*c^3*e*f^2*h*k^5+16001*c^3*f^4*h^2*k^3+16001*c^2*e^2*f*h*k^6-16001*c^2*e*f^3*h^2*k^4);
gPoly=(16001*c^7*e*h*k^3-15998*c^6*e*f*h^2*k^2-7*c^5*e*f^2*h^3*k-8*c^4*e^2*f*h^3*k^2+4*c^4*e*f^3*h^4+8*c^3*e^3*h^3*k^3+24*c^3*e^2*f^2*h^4*k-24*c^2*e^3*f*h^4*k^2-16*c^2*e^2*f^3*h^5+16*c*e^3*f^2*h^5*k);
simplifyGcd(fPoly, gPoly);
tst_status(1);$
