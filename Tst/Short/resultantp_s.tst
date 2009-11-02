// $Id$

//
// resultantp_s.tst - short tests for resultant calculations mod p.
//
// All univariate non-trivial examples come from gcdUnivP.fex or
// gcdUnivPAlpha.fex.  Most of the examples have a non-trivial gcd, so we
// compute always an disturbed example, too.  Some extra checks are added
// for calculation of resultants of polys of same degree.
//
// The examples involving a transcendental variable are the same as the
// examples with algebraic variables.
//
// To Do:
//
// o multivariate resultant calculations
//

LIB "tst.lib";
tst_init();
tst_ignore("CVS ID  : $Id$");

//
// - ring r1=32003,x,dp.
//

tst_ignore( "ring r1=32003,x,dp;" );
ring r1=32003,x,dp;

poly f=-9554*x^4-12895*x^3-10023*x^2-6213*x;
poly g;

// some trivial examples
resultant(0, 0, x);
resultant(0, 3123, x);
resultant(4353, 0, x);

resultant(0, f, x);
resultant(f, 0, x);

resultant(23123, f, x);
resultant(f, 13123, x);

// some less trivial examples
f=-9554*x^4-12895*x^3-10023*x^2-6213*x;
g=-9554*x^3-3341*x^2+6213*x;
resultant(f, g, x);
resultant(f, g+1, x);
resultant(f, g*x+1, x);

f=-11265*x^6+12161*x^5+10369*x^4-12161*x^3+896*x^2;
g=10669*x^8-10673*x^7+5*x^6+8*x^5-10681*x^4+10665*x^3+7*x^2;
resultant(f, g, x);
resultant(f, g+1, x);
resultant(f*x^2, g+1, x);

f=-7918*x^9-14406*x^8-7256*x^7+2092*x^6-2198*x^5-12539*x^4-14631*x^3-7150*x^2;
g=-14833*x^13-7011*x^12+15121*x^11-10864*x^10+12943*x^9-9871*x^8+10354*x^7-1437*x^6+6604*x^5-10394*x^4-3231*x^3-9348*x^2-2092*x;
resultant(f, g, x);
resultant(f, g+1, x);

f=106*x^13-583*x^12-1060*x^11+8056*x^10+1696*x^9-327*x^8+12508*x^7+8277*x^6+5609*x^5-12879*x^4+13144*x^3-2544*x^2;
g=11118*x^14-5404*x^13-908*x^12-13908*x^11+3188*x^10-8818*x^9+10439*x^8-14811*x^7+15530*x^6-4891*x^5+6322*x^4+15829*x^3-13686*x^2;
resultant(f, g, x);
resultant(f, g+1, x);

f=-14613*x^17+2235*x^16-298*x^15+4540*x^14+7214*x^13+5494*x^12-3122*x^11-4720*x^10+8300*x^9-6582*x^8-9908*x^7-15983*x^6-5802*x^5-8634*x^4+7899*x^3+10556*x^2+6931*x+11063;
g=-10168*x^20+6674*x^19+3004*x^18+13113*x^17+9449*x^16+9097*x^15-6420*x^14+50*x^13+434*x^12-15226*x^11+3727*x^10-14065*x^9-9751*x^8-15792*x^7+6004*x^6-5059*x^5+2479*x^4-12504*x^3-11328*x^2-11338*x-8280;
resultant(f, g, x);
resultant(f, g+1, x);

f=3812*x^22-6874*x^21+12586*x^20+3003*x^19-9568*x^18+11117*x^17 +7524*x^16+11138*x^15-9743*x^14+1892*x^13+12485*x^12-569*x^11-8265*x^10-5991*x^9+13701*x^8+2644*x^7-3936*x^6-15875*x^5+1289*x^4+3956*x^3-10099*x^2-6616*x+5401;
g=10652*x^20-4144*x^19-11810*x^18+8237*x^17-8675*x^16+6545*x^15-3601*x^14+14559*x^13+8090*x^12-8378*x^11+14255*x^10+8767*x^9-13932*x^8+11602*x^7-10751*x^6-4899*x^5+8637*x^4+14084*x^3-11583*x^2+5882*x+885;
resultant(f, g, x);
resultant(f, g+1, x);

//
// - ring r2=(32003,a),x,dp; minpoly=a^4+8734*a^3+a^2+11817*a+1.
//

tst_ignore( "ring r2=(32003,a),x,dp;" );
ring r2=(32003,a),x,dp;
minpoly=a^4+8734*a^3+a^2+11817*a+1;

poly f=-9554*x^4-12895*x^3-10023*x^2-6213*x;
poly g;

// first, some of the r1 examples
resultant(0, 0, x);
resultant(0, 3123, x);
resultant(4353, 0, x);

resultant(0, f, x);
resultant(f, 0, x);

resultant(23123, f, x);
resultant(f, 13123, x);

// some less trivial examples
f=-9554*x^4-12895*x^3-10023*x^2-6213*x;
g=-9554*x^3-3341*x^2+6213*x;
resultant(f, g, x);
resultant(f, g+1, x);
resultant(f, g*x+1, x);

f=-11265*x^6+12161*x^5+10369*x^4-12161*x^3+896*x^2;
g=10669*x^8-10673*x^7+5*x^6+8*x^5-10681*x^4+10665*x^3+7*x^2;
resultant(f, g, x);
resultant(f, g+1, x);
resultant(f*x^2, g+1, x);

// now, examples involving the algebraic variable
f=(25836*a^3*x^6+30467*a^2*x^6+26792*a*x^6+30467*x^6+27083*a^3*x^5+30808*a^2*x^5+28163*a*x^5+292*x^5+12440*a^3*x^4+19396*a^2*x^4+19616*a*x^4+23236*x^4+25156*a^3*x^3+31764*a^2*x^3+31235*a*x^3+6459*x^3);
g=(14670*a^3*x^6+19715*a^2*x^6+22318*a*x^6+19715*x^6+19743*a^3*x^5+28179*a^2*x^5+19715*a*x^5+7335*x^5+6379*a^3*x^4+11397*a^2*x^4+23599*a*x^4+9861*x^4+16005*a^3*x^3+31525*a^2*x^3+30467*a*x^3+12918*x^3);
resultant(f, g, x);
resultant(f, g+1, x);

f=(21147*a^3*x^5+10147*a^3*x^4+8142*a^2*x^4+27671*a*x^4+29289*x^4+29289*a^3*x^3);
g=(6167*a^3*x^6+1536*a^2*x^6+5211*a*x^6+1536*x^6+17534*a^3*x^5+478*a^2*x^5+1536*a*x^5+19085*x^5+3203*a^3*x^4+26578*a^2*x^4+17052*a*x^4+26770*x^4+26002*a^3*x^3+24062*a^2*x^3+192*a*x^3+6386*x^3);
resultant(f, g, x);
resultant(f, g+1, x);

f=(2295*a^2*x^11+5897*a^3*x^10+765*a^2*x^10+27718*a^3*x^9+1283*a^2*x^9+23792*a*x^9+518*x^9+22026*a^3*x^8+11468*a^2*x^8+16834*a*x^8+24778*x^8+28460*a^3*x^7+8140*a^2*x^7+28036*a*x^7+1613*x^7+18196*a^3*x^6+13274*a^2*x^6+4638*a*x^6+31194*x^6+12206*a^3*x^5+10349*a^2*x^5+30979*a*x^5+8612*x^5);
g=(8192*a^3*x^10+8957*a^2*x^10+9659*a^3*x^9+28672*a^2*x^9+4553*a*x^9+12288*x^9+5819*a^3*x^8+320*a^2*x^8+14380*a*x^8+12104*x^8+12047*a^3*x^7+19823*a^2*x^7+6824*a*x^7+4335*x^7+29376*a^3*x^6+22190*a^2*x^6+31239*a*x^6+22647*x^6);
resultant(f, g, x);
resultant(f, g+1, x);

//
// - ring r3=(32003,t),x,dp.
//

tst_ignore( "ring r3=(32003,t),x,dp;" );
ring r3=(32003,t),x,dp;

poly f=-9554*x^4-12895*x^3-10023*x^2-6213*x;
poly g;

// first, some of the r1 examples
resultant(0, 0, x);
resultant(0, 3123, x);
resultant(4353, 0, x);

resultant(0, f, x);
resultant(f, 0, x);

resultant(23123, f, x);
resultant(f, 13123, x);

// some less trivial examples
f=-9554*x^4-12895*x^3-10023*x^2-6213*x;
g=-9554*x^3-3341*x^2+6213*x;
resultant(f, g, x);
resultant(f, g+1, x);
resultant(f, g*x+1, x);

f=-11265*x^6+12161*x^5+10369*x^4-12161*x^3+896*x^2;
g=10669*x^8-10673*x^7+5*x^6+8*x^5-10681*x^4+10665*x^3+7*x^2;
resultant(f, g, x);
resultant(f, g+1, x);
resultant(f*x^2, g+1, x);

// now, examples involving the transcendental variable
f=(25836*t^3*x^6+30467*t^2*x^6+26792*t*x^6+30467*x^6+27083*t^3*x^5+30808*t^2*x^5+28163*t*x^5+292*x^5+12440*t^3*x^4+19396*t^2*x^4+19616*t*x^4+23236*x^4+25156*t^3*x^3+31764*t^2*x^3+31235*t*x^3+6459*x^3);
g=(14670*t^3*x^6+19715*t^2*x^6+22318*t*x^6+19715*x^6+19743*t^3*x^5+28179*t^2*x^5+19715*t*x^5+7335*x^5+6379*t^3*x^4+11397*t^2*x^4+23599*t*x^4+9861*x^4+16005*t^3*x^3+31525*t^2*x^3+30467*t*x^3+12918*x^3);
resultant(f, g, x);
resultant(f, g+1, x);

f=(21147*t^3*x^5+10147*t^3*x^4+8142*t^2*x^4+27671*t*x^4+29289*x^4+29289*t^3*x^3);
g=(6167*t^3*x^6+1536*t^2*x^6+5211*t*x^6+1536*x^6+17534*t^3*x^5+478*t^2*x^5+1536*t*x^5+19085*x^5+3203*t^3*x^4+26578*t^2*x^4+17052*t*x^4+26770*x^4+26002*t^3*x^3+24062*t^2*x^3+192*t*x^3+6386*x^3);
resultant(f, g, x);
resultant(f, g+1, x);

f=(2295*t^2*x^11+5897*t^3*x^10+765*t^2*x^10+27718*t^3*x^9+1283*t^2*x^9+23792*t*x^9+518*x^9+22026*t^3*x^8+11468*t^2*x^8+16834*t*x^8+24778*x^8+28460*t^3*x^7+8140*t^2*x^7+28036*t*x^7+1613*x^7+18196*t^3*x^6+13274*t^2*x^6+4638*t*x^6+31194*x^6+12206*t^3*x^5+10349*t^2*x^5+30979*t*x^5+8612*x^5);
g=(8192*t^3*x^10+8957*t^2*x^10+9659*t^3*x^9+28672*t^2*x^9+4553*t*x^9+12288*x^9+5819*t^3*x^8+320*t^2*x^8+14380*t*x^8+12104*x^8+12047*t^3*x^7+19823*t^2*x^7+6824*t*x^7+4335*x^7+29376*t^3*x^6+22190*t^2*x^6+31239*t*x^6+22647*x^6);
resultant(f, g, x);
resultant(f, g+1, x);
tst_status(1);$
