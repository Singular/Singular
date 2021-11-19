// File: tropical.tst
// Tests for tropical lib
LIB "tst.lib";
tst_init();
LIB "tropical.lib";
///////////////////////////////////////////////////////////////////////////
// A) Test for tropicalLifting and displayTropicalLifting
///////////////////////////////////////////////////////////////////////////
ring r0=0,(t,x),dp;
poly f=t7-6t4+3t3x+8t3-12t2x+6tx2-x3+t2;
f;
// The point -2/3 is in the tropical variety.
list L=tropicalLifting(f,intvec(3,-2),4);
L;
displayTropicalLifting(L,"subst");
// --------------------------------------------------------
// Example 2 - a field extension is necessary
// --------------------------------------------------------
poly g=(1+t2)*x2+t5x+t2;
g;
// The poin -1 is in the tropical variety.
displayTropicalLifting(tropicalLifting(g,intvec(1,-1),4),"subst");
// --------------------------------------------------------
// Example 3 - the ideal is not zero dimensional
// --------------------------------------------------------
ring r1=0,(t,x,y),dp;
poly f=(9t27-12t26-5t25+21t24+35t23-51t22-40t21+87t20+56t19-94t18-62t17+92t16+56t15-70t14-42t13+38t12+28t11+18t10-50t9-48t8+40t7+36t6-16t5-12t4+4t2)*x2+(-9t24+12t23-4t22-42t20+28t19+30t18-20t17-54t16+16t15+48t14-16t12+8t11-18t10-26t9+30t8+20t7-24t6+4t5+28t4-8t3-16t2+4)*xy+(6t16-10t15-2t14+16t13+4t12-18t11-10t10+24t9+6t8-20t7+8t6+8t5-20t4-4t3+12t2+4t-4)*y2+(-9t28+3t27+8t26-4t25-45t24-6t23+42t22+30t21-94t20-40t19+68t18+82t17-38t16-60t15+26t14+36t13-22t12-20t11+4t10+4t9+12t8+8t7-8t6-8t5+4t4)*x+(9t27-21t26+16t25+14t24+12t23-61t22+27t21+80t20-19t19-100t18+26t17+96t16-24t15-84t14+44t12-2t11-18t10+2t9+40t8+4t7-32t6-12t5+4t3+12t2-4)*y+(9t27-12t26+4t25+36t23-18t22-28t21-2t20+54t19+14t18-52t17-44t16+24t15+40t14-4t13-12t12+4t11-4t10-4t9+4t8);
f;
displayTropicalLifting(tropicalLifting(f,intvec(1,-1,-4),3),"subst");
// --------------------------------------------------------
// Example 4 - the ideal has even more equations
// --------------------------------------------------------
ring r2=0,(t,x,y,z),dp;
ideal i=t-x3+3yz,t2xy-2x2z;
i;
displayTropicalLifting(tropicalLifting(i,intvec(1,1,3,0),2),"subst");
// --------------------------------------------------------
// Example 5-7 - testing some options
// --------------------------------------------------------
setring r0;
poly f1=(x2-t3)*(x3-t5)*(x5-t7)*(x7-t11)*(x11-t13);
f1;
displayTropicalLifting(tropicalLifting(f1,intvec(7,-11),4,"noAbs"),"subst");
poly f2=(1+t2)*x2+t5x+t2;
f2;
displayTropicalLifting(tropicalLifting(f2,intvec(1,-1),4,"isZeroDimensional","findAll"),"subst");
poly f3=t7-6t4+3t3x+8t3-12t2x+6tx2-x3+t2;
f3;
displayTropicalLifting(tropicalLifting(f3,intvec(3,-2),4,"isInTrop","findAll"),"subst");
///////////////////////////////////////////////////////////////////////////
// B) Test for tropicalCurve, drawTropicalCurve and drawNewtonSubdivision
///////////////////////////////////////////////////////////////////////////
ring r=(0,t),(x,y),dp;
poly f=t*(x7+y7+1)+1/t*(x4+y4+x2+y2+x3y+xy3)+1/t7*x2y2;
list graph=tropicalCurve(f);
graph;
size(graph)-1;
drawTropicalCurve(graph,"onlytexfile");
poly g=t3*(x7+y7+1)+1/t3*(x4+y4+x2+y2+x3y+xy3)+1/t21*x2y2;
list tropical_g=tropicalise(g);
tropical_g;
drawTropicalCurve(tropical_g,"onlytexfile");
///////////////////////////////////////////////////////////////////////////
// C) Test for tropicalJInvariant, jInvariant, weierstrassForm
///////////////////////////////////////////////////////////////////////////
// tropcial_j_invariant computes the tropical j-invariant of the elliptic curve f
tropicalJInvariant(t*(x3+y3+1)+1/t*(x2+y2+x+y+x2y+xy2)+1/t2*xy);
// the Newton polygone need not be the standard simplex
tropicalJInvariant(x+y+x2y+xy2+1/t*xy);
// the curve can have arbitrary degree
tropicalJInvariant(t*(x7+y7+1)+1/t*(x4+y4+x2+y2+x3y+xy3)+1/t7*x2y2);
// the procedure does not realise, if the embedded graph of the tropical curve has
// a loop that can be resolved
tropicalJInvariant(1+x+y+xy+tx2y+txy2);
// but it does realise, if the curve has no loop at all ...
tropicalJInvariant(x+y+1);
// or if the embedded graph has more than one loop - even if only one cannot be resolved
tropicalJInvariant(1+x+y+xy+tx2y+txy2+t3x5+t3y5+tx2y2+t2xy4+t2yx4);
// f is already in Weierstrass form
weierstrassForm(y2+yx+3y-x3-2x2-4x-6);
// g is not, but wg is
g=x+y+x2y+xy2+1/t*xy;
poly wg=weierstrassForm(g);
wg;
// ... but it is not yet a simple, since it still has an xy-term, unlike swg
poly swg=weierstrassForm(g,1);
swg;
// the j-invariants of all three polynomials coincide ...
jInvariant(g);
jInvariant(wg);
jInvariant(swg);
// the following curve is elliptic as well
poly h=x22y11+x19y10+x17y9+x16y9+x12y7+x9y6+x7y5+x2y3;
// its Weierstrass form is
weierstrassForm(h);
jInvariant(x+y+x2y+y3+1/t*xy,"ord");
///////////////////////////////////////////////////////////////////////////
// D) Test for tropicalise, tropicaliseSet, tInitialForm,
//    tInitialIdeal, initialForm, initialIdeal
///////////////////////////////////////////////////////////////////////////
tropicalise(2t3x2-1/t*xy+2t3y2+(3t3-t)*x+ty+(t6+1));
ideal i=txy-y2+1,2t3x2+1/t*y-t6;
tropicaliseSet(i);
setring r1;
poly ff=t4x2+y2-t2xy+t4x-t9;
intvec w=-1,-2,-3;
tInitialForm(ff,w);
ideal ii=t2x-y+t3,t2x-y-2t3x;
w=-1,2,0;
// the t-initial forms of the generators are
tInitialForm(ii[1],w),tInitialForm(i[2],w);
// and they do not generate the t-initial ideal of i
tInitialIdeal(ii,w);
ring r5=0,(x,y),dp;
poly f=x3+y2-xy+x-1;
w=2,3;
initialForm(f,w);
initialIdeal(f,w);
///////////////////////////////////////////////////////////////////////////
// E) Test for texNumber, texPolynomial, texMatrix, texDrawBasic,
//    texDrawTropical, texDrawNewtonSubdivision, texDrawTriangulation
///////////////////////////////////////////////////////////////////////////
ring r6=(0,t),x,dp;
texNumber((3t2-1)/t3);
texPolynomial(1/t*x2-t2x+1/t);
matrix M[2][2]=3/2,1/t*x2-t2x+1/t,5,-2x;
texMatrix(M);
setring r;
string texf=texDrawTropical(tropicalCurve(x+y+1),list("",1));
texDrawBasic(texf);
graph=tropicalCurve(x+y+x2y+xy2+1/t*xy);
// compute the texdraw code of the tropical curve defined by f
texDrawTropical(graph);
// compute the texdraw code again, but set the scalefactor to 1
texDrawTropical(graph,"",1);
texDrawNewtonSubdivision(graph);
list polygon=intvec(1,1),intvec(3,0),intvec(2,0),intvec(1,0),intvec(0,0),intvec(2,1),intvec(0,1),intvec(1,2),intvec(0,2),intvec(0,3);
list triang=intvec(1,2,5),intvec(1,5,10),intvec(1,2,10);
texDrawTriangulation(triang,polygon);
///////////////////////////////////////////////////////////////////////////
// F) Test for Auxilary Procedures
///////////////////////////////////////////////////////////////////////////
ring r7=0,(x,y),dp;
ideal i=(x+1)*y2;
// y is NOT in the radical of i
radicalMemberShip(y,i);
ring r8=0,(x,y),ds;
ideal i=(x+1)*y2;
// since this time the ordering is local, y is in the radical of i
radicalMemberShip(y,i);
ring r9=(0,t),(x,y),dp;
poly f=t4x2+y2-t2xy+t4x-t9;
w=2,3;
tInitialFormPar(f,w);
f=t4x2+y2-t2xy+t4x-1/t6;
w=2,3;
tInitialFormParMax(f,w);
ideal i=t2x2+y2,x-t2;
solveTInitialFormPar(i);
detropicalise(3x+4y-1);
ring r10=0,(x,y),dp;
poly conic=2x2+1/2y2-1;
dualConic(conic);
setring r;
f=t2xy+1/t*y+t3;
parameterSubstitute(f,3);
parameterSubstitute(f,-1);
f=t2x+1/t*y-1;
tropicalSubst(f,2,x,x+t,y,tx+y+t2);
randomPolyInT(3,-2,5);
randomPolyInT(3,-2,5,1);
//////////////////////////////
ring trop_r=0,(x,y,z,w),dp;
ideal I=x-2y+3z,3y-4z+5w;
tropicalVariety(I);
tropicalVariety(I,number(2));
tropicalVariety(I[1],number(3));

tst_status(1);$
