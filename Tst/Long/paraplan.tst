LIB "tst.lib";
tst_init();

/////////////////////////////////////////////////////////////////////////////
/// Further examples for testing the main procedures
/// Timings on wawa Sept 29
/////////////////////////////////////////////////////////////////////////////
LIB"paraplanecurves.lib";
// -------------------------------------------------------
// Example 2
// -------------------------------------------------------
ring RR = 0, (x,y,z), dp;
poly f = y3-x2z;  // cusp at origin
adjointIdeal(f,1);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 0
testParametrization(f,RP1);
setring RP1; PARA;
kill RR;kill RP1;
// -------------------------------------------------------
// Example 3
// -------------------------------------------------------
ring RR = 0, (x,y,z), dp;
poly f=(xz-y^2)^2-x*y^3; // 1 sing at origin, 1 cusp, no OMPs
adjointIdeal(f,1);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f); // time 0
testParametrization(f,RP1);
setring RP1; PARA;
kill RR;kill RP1;
// -------------------------------------------------------
// Example 4
// -------------------------------------------------------
ring RR = 0, (x,y,z), dp;
poly f = y5-y4x+4y2x2z-x4z;  // 1 sing at origin, no OMPs, no cusps
adjointIdeal(f,1);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 0
testParametrization(f,RP1);
setring RP1; PARA;
kill RR;kill RP1;
// -------------------------------------------------------
// Example 5
// -------------------------------------------------------
ring RR = 0, (x,y,z), dp;
poly f = 259x5-31913x4y+939551x3y2+2871542x2y3+2845801xy4;
f = f+914489y5+32068x4z-1884547x3yz-8472623x2y2z-11118524xy3z;
f = f-4589347y4z+944585x3z2+8563304x2yz2+16549772xy2z2+9033035y3z2;
f = f-2962425x2z3-11214315xyz3-8951744y2z3+2937420xz4+4547571yz4-953955z5;
// 6 nodes
adjointIdeal(f,1);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 0
testParametrization(f,RP1);
setring RP1; PARA;
kill RR;kill RP1;
// -------------------------------------------------------
// Example 7
// -------------------------------------------------------
ring RR = 0, (x,y,z), dp;
poly f = y^8-x^3*(z+x)^5;  // 1 sing at origin, 1 further sing, no OMPs,
                           // no cusps
adjointIdeal(f,1);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 0
testParametrization(f,RP1);
setring RP1; PARA;
kill RR;kill RP1;
// -------------------------------------------------------
// Example 8
// -------------------------------------------------------
ring RR = 0, (x,y,z), dp;
poly f = 11y7+7y6x+8y5x2-3y4x3-10y3x4-10y2x5-x7-33y6-29y5x-13y4x2+26y3x3;
f = f+30y2x4+10yx5+3x6+33y5+37y4x-8y3x2-33y2x3-20yx4-3x5-11y4-15y3x;
f = f+13y2x2+10yx3+x4; // 3 OMPs of mult 3, 1 OMP of mult 4
f = homog(f,z);
adjointIdeal(f,1);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 0
testParametrization(f,RP1);
setring RP1; PARA;
kill RR;kill RP1;
// -------------------------------------------------------
// Example 9
// -------------------------------------------------------
ring RR = 0, (x,y,z), dp;
poly f = y^8-x^3*(z+x)^5;  // 1 sing at origin, 1 further sing, no OMPs,
                           // no cusps
adjointIdeal(f,1);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 0
testParametrization(f,RP1);  
setring RP1; PARA;
kill RR;kill RP1;
// -------------------------------------------------------
// Example 10
// -------------------------------------------------------
ring SS = 0, (u,v,z), dp;
poly f = u^4-14*u^2*v^2+v^4+8*u^2*v*z+8*v^3*z; // 1 OMP of mult 3 at orgin
adjointIdeal(f,1);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 0
testParametrization(f,RP1);
setring RP1; PARA;
kill SS;kill RP1;
// -------------------------------------------------------
// Example 11
// -------------------------------------------------------
ring SS = 0, (u,v,z), dp;
poly f = 14440*u^5-16227*u^4*v+10812*u^3*v^2-13533*u^2*v^3+3610*u*v^4;
f = f+1805*v^5+14440*u^4*z-18032*u^3*v*z+16218*u^2*v^2*z-12626*u*v^3*z;
f = f+3610*v^4*z+3610*u^3*z^2-4508*u^2*v*z^2+5406*u*v^2*z^2-2703*v^3*z^2;
// 1 OMP of mult 3 at origin, 2 nodes
adjointIdeal(f,1);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 0
testParametrization(f,RP1);
setring RP1; PARA;
kill SS;kill RP1;
// -------------------------------------------------------
// Example 12
// -------------------------------------------------------
ring SS = 0, (u,v,z), dp;
poly f = u^6+3*u^4*v^2+3*u^2*v^4+v^6-4*u^4*z^2-34*u^3*v*z^2-7*u^2*v^2*z^2;
f = f+12*u*v^3*z^2+6*v^4*z^2+36*u^2*z^4+36*u*v*z^4+9*v^2*z^4;
// needs field extension *** 6 nodes, 2 cusps, 1 sing at 0
adjointIdeal(f,1);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 0
testParametrization(f,RP1);
setring RP1; PARA;
kill SS;kill RP1;
// -------------------------------------------------------
// Example 13
// -------------------------------------------------------
ring SS = 0, (u,v,z), dp;
poly f = -24135/322*u^6-532037/6440*u^5*v+139459/560*u^4*v^2;
f = f-1464887/12880*u^3*v^3+72187/25760*u^2*v^4+9/8*u*v^5+1/8*v^6;
f = f-403511/3220*u^5*z-40817/920*u^4*v*z+10059/80*u^3*v^2*z;
f = f-35445/1288*u^2*v^3*z+19/4*u*v^4*z+3/4*v^5*z-20743/805*u^4*z^2;
f = f+126379/3220*u^3*v*z^2-423417/6440*u^2*v^2*z^2+11/2*u*v^3*z^2;
f = f+3/2*v^4*z^2+3443/140*u^3*z^3+u^2*v*z^3+u*v^2*z^3+v^3*z^3;
// 2 OMPs of mult 3 (1 at origin), 4 nodes
adjointIdeal(f,1);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 5
testParametrization(f,RP1);
setring RP1; PARA;
kill SS;kill RP1;
// -------------------------------------------------------
// Example 14
// -------------------------------------------------------
ring SS = 0, (u,v,z), dp;
poly f = 
2*u^7+u^6*v+3*u^5*v^2+u^4*v^3+2*u^3*v^4+u^2*v^5+2*u*v^6+v^7
-7780247/995328*u^6*z-78641/9216*u^5*v*z-10892131/995328*u^4*v^2*z
-329821/31104*u^3*v^3*z-953807/331776*u^2*v^4*z-712429/248832*u*v^5*z
+1537741/331776*v^6*z+2340431/248832*u^5*z^2+5154337/248832*u^4*v*z^2
+658981/41472*u^3*v^2*z^2+1737757/124416*u^2*v^3*z^2
-1234733/248832*u*v^4*z^2-1328329/82944*v^5*z^2-818747/248832*u^4*z^3
-1822879/124416*u^3*v*z^3-415337/31104*u^2*v^2*z^3
+1002655/124416*u*v^3*z^3+849025/82944*v^4*z^3;
// 3 OMPs of mult 3, 1 OMP of mult 4 at origin
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 1
testParametrization(f,RP1);
setring RP1; PARA;
kill SS;kill RP1;
// -------------------------------------------------------
// Example 15
// -------------------------------------------------------
ring SS = 0, (u,v,z), dp;
poly f = 590819418867856650536224u7-147693905508217596067968u6v;
f = f+229117518934972047619978u5v2-174050799674982973889542u4v3;
f = f-92645796479789150855110u3v4-65477418713685583062704u2v5;
f = f+4529961835917468460168uv6+7715404057796585983136v7;
f = f-413640780091141905428104u6z+571836835577486968144618u5vz;
f = f-551807810327826605739444u4v2z-488556410340789283359926u3v3z;
f = f-473466023008413178155962u2v4z+48556741573432247323608uv5z;
f = f+77647371229172269259528v6z+340450118906560552282893u5z2;
f = f-433598825064368371610344u4vz2-937281070591684636591672u3v2z2;
f = f-1388949843915129934647751u2v3z2+204081793110898617103998uv4z2;
f = f+335789953068251652554308v5z2+6485661002496681852577u4z3;
f = f-772700266516318390630202u3vz3-2068348417248100329533330u2v2z3;
f = f+440320154612359641806108uv3z3+808932515589210854581618v4z3;
f = f-229384307132237615286548u3z4-1564303565658228216055227u2vz4;
f = f+520778334468674798322974uv2z4+1172483905704993294097655v3z4;
f = f-480789741398016816562100u2z5+322662751598958620410786uvz5;
f = f+1022525576391791616258310v2z5+82293493608853837667471uz6;
f = f+496839109904761426785889vz6+103766136235628614937587z7; // 15 nodes
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 72
testParametrization(f,RP1);  
setring RP1; PARA;
kill SS;kill RP1;

// -------------------------------------------------------
// Example 16
// -------------------------------------------------------
ring SS = 0, (u,v,z), dp;
poly f = 25*u^8+184*u^7*v+518*u^6*v^2+720*u^5*v^3+576*u^4*v^4+282*u^3*v^5;
f = f+84*u^2*v^6+14*u*v^7+v^8+244*u^7*z+1326*u^6*v*z+2646*u^5*v^2*z;
f = f+2706*u^4*v^3*z+1590*u^3*v^4*z+546*u^2*v^5*z+102*u*v^6*z+8*v^7*z;
f = f+854*u^6*z^2+3252*u^5*v*z^2+4770*u^4*v^2*z^2+3582*u^3*v^3*z^2;
f = f+1476*u^2*v^4*z^2+318*u*v^5*z^2+28*v^6*z^2+1338*u^5*z^3+3740*u^4*v*z^3;
f = f+4030*u^3*v^2*z^3+2124*u^2*v^3*z^3+550*u*v^4*z^3+56*v^5*z^3+1101*u^4*z^4;
f = f+2264*u^3*v*z^4+1716*u^2*v^2*z^4+570*u*v^3*z^4+70*v^4*z^4+508*u^3*z^5;
f = f+738*u^2*v*z^5+354*u*v^2*z^5+56*v^3*z^5+132*u^2*z^6+122*u*v*z^6;
f = f+28*v^2*z^6+18*u*z^7+8*v*z^7+z^8; // 3 nodes, 1 sing
adjointIdeal(f,1);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 20
testParametrization(f,RP1);
setring RP1; PARA;
kill SS;kill RP1;
// -------------------------------------------------------
// Example 17
// -------------------------------------------------------
ring SS = 0, (u,v,z), dp;
poly f = -2*u*v^4*z^4+u^4*v^5+12*u^4*v^3*z^2+12*u^2*v^4*z^3-u^3*v*z^5;
f = f+11*u^3*v^2*z^4-21*u^3*v^3*z^3-4*u^4*v*z^4+2*u^4*v^2*z^3-6*u^4*v^4*z;
f = f+u^5*z^4-3*u^5*v^2*z^2+u^5*v^3*z-3*u*v^5*z^3-2*u^2*v^3*z^4+u^3*v^4*z^2;
f = f+v^5*z^4; // 1 OMP of mult 4, 3 OMPs of mult 5, 1 sing at origin
f = subst(f,z,u+v+z);
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 5
testParametrization(f,RP1);
setring RP1; PARA;
kill SS;kill RP1;
// -------------------------------------------------------
// Example 18
// -------------------------------------------------------
ring SS = 0, (u,v,z), dp;
poly f = u^5*v^5+21*u^5*v^4*z-36*u^4*v^5*z-19*u^5*v^3*z^2+12*u^4*v^4*z^2;
f = f+57*u^3*v^5*z^2+u^5*v^2*z^3+u^4*v^3*z^3-53*u^3*v^4*z^3-19*u^2*v^5*z^3;
f = f+u^5*v*z^4+43*u^3*v^3*z^4+u*v^5*z^4+u^5*z^5-15*u^3*v^2*z^5+u^2*v^3*z^5;
f = f+u*v^4*z^5+v^5*z^5; // 1 OMP of mult 4, 3 OMPs of mult 5 (1 at origin)
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 8
testParametrization(f,RP1);
setring RP1; PARA;
kill SS;kill RP1;
// -------------------------------------------------------
// Example 19
// -------------------------------------------------------
ring SS = 0, (u,v,z), dp;
poly f = u^10+6*u^9*v-30*u^7*v^3-15*u^6*v^4+u^5*v^5+u^4*v^6+6*u^3*v^7;
f = f+u^2*v^8+7*u*v^9+v^10+5*u^9*z+24*u^8*v*z-30*u^7*v^2*z-120*u^6*v^3*z;
f = f-43*u^5*v^4*z+5*u^4*v^5*z+20*u^3*v^6*z+10*u^2*v^7*z+29*u*v^8*z+5*v^9*z;
f = f+10*u^8*z^2+36*u^7*v*z^2-105*u^6*v^2*z^2-179*u^5*v^3*z^2-38*u^4*v^4*z^2;
f = f+25*u^3*v^5*z^2+25*u^2*v^6*z^2+46*u*v^7*z^2+10*v^8*z^2+10*u^7*z^3;
f = f+24*u^6*v*z^3-135*u^5*v^2*z^3-117*u^4*v^3*z^3-u^3*v^4*z^3+25*u^2*v^5*z^3;
f = f+34*u*v^6*z^3+10*v^7*z^3+5*u^6*z^4+6*u^5*v*z^4-75*u^4*v^2*z^4;
f = f-27*u^3*v^3*z^4+10*u^2*v^4*z^4+11*u*v^5*z^4+5*v^6*z^4+u^5*z^5;
f = f-15*u^3*v^2*z^5+u^2*v^3*z^5+u*v^4*z^5+v^5*z^5;
// 1 OMP of mult 4, 3 OMPs of mult 5 (1 at origin)
adjointIdeal(f,2);
def RP1 = paraPlaneCurve(f);  // time 2
testParametrization(f,RP1);
setring RP1; PARA;
kill SS;kill RP1;
// -------------------------------------------------------
// Example 20
// -------------------------------------------------------
ring R = 0, (x,y,z), dp;
system("random", 4711);
poly p = x^2 + 2*y^2 + 5*z^2 - 4*x*y + 3*x*z + 17*y*z;
def S = rationalPointConic(p); // quadratic field extension,
                              // minpoly = a^2 - 2
if (testPointConic(p, S) == 1)
{ "point lies on conic"; }
else
{ "point does not lie on conic"; }
// -------------------------------------------------------
// Example 21
// -------------------------------------------------------
ring R = 0, (x,y,z), dp;
system("random", 4711);
poly p = x^2 - 1857669520 * y^2 + 86709575222179747132487270400 * z^2;
def S = rationalPointConic(p); // same as current basering,
                              // no extension needed
if (testPointConic(p, S) == 1)
{ "point lies on conic"; }
else
{ "point does not lie on conic"; }

tst_status(1);$


