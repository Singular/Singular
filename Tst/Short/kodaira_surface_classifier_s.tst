LIB "tst.lib";
tst_init();

LIB "kodaira_surface_classifier.lib";

ring R=0,(x0,x1,x2,x3),dp;

ideal cubic=x0^3+x1^3+x2^3+x3^3;
list C=KSCsurfaceData(cubic);
C[2..4];
KSCsubcanonicalIndex(cubic);
def AC=KSCautoEconomical(cubic);
AC.classification;
AC.kodairaDimension;
KSCminimalCanonicalTorsionOrder(AC);
KSCminimalCanonicalPowerIsTrivial(AC,2);

ideal quartic=x0^4+x1^4+x2^4+x3^4;
list Q=KSCsurfaceData(quartic);
Q[2..4];
KSCsubcanonicalIndex(quartic);
intvec PQ=KSCplurigenera(quartic,3);
PQ;
def AK=KSCautoEconomical(quartic);
AK.classification;
AK.kodairaDimension;
KSCminimalCanonicalTorsionOrder(AK);
KSCminimalCanonicalPowerIsTrivial(AK,3);
AK.keepData;
def AKkeep=KSCautoEconomical(quartic,3,0,1);
AKkeep.keepData;
size(string(AKkeep.inputIdeal));
list SQ=KSCcheckSurface(quartic);
SQ[5];

// A caller's positive global degBound must not truncate any part of a public
// plurigenus or classifier command.  Exercise the direct numerical command,
// the economical classifier, and KSCauto's complete fallback wrapper at two
// bounds, checking both the known K3 result and restoration after every call.
int shortSavedDegBound=degBound;
degBound=2;
KSCplurigenera(quartic,2);
degBound;
def boundedClassify2=KSCclassify(quartic,3,0,0,"original");
boundedClassify2.classification;
boundedClassify2.plurigenera;
degBound;
def boundedAuto2=KSCauto(quartic,3,1,0,0,"original");
boundedAuto2.classification;
boundedAuto2.plurigenera;
degBound;

degBound=3;
KSCplurigenera(quartic,2);
degBound;
def boundedClassify3=KSCclassify(quartic,3,0,0,"original");
boundedClassify3.classification;
boundedClassify3.plurigenera;
degBound;
def boundedAuto3=KSCauto(quartic,3,1,0,0,"original");
boundedAuto3.classification;
boundedAuto3.plurigenera;
degBound;
degBound=shortSavedDegBound;

ideal quintic=x0^5+x1^5+x2^5+x3^5;
KSCsubcanonicalIndex(quintic);
def AQ=KSCautoEconomical(quintic);
AQ.classification;
AQ.kodairaDimension;

// A smooth (2,2) complete-intersection surface in P4 has a terminal
// codimension-two resolution.  This certifies the direct transposed-
// differential canonical presentation and its anti-canonical twist.
ring R5=31991,(z0,z1,z2,z3,z4),dp;
ideal twoQuadrics=z0^2+z1^2+z2^2+z3^2+z4^2,
                  z0^2+2*z1^2+3*z2^2+4*z3^2+5*z4^2;
module twoQuadricsStructure=twoQuadrics;
twoQuadricsStructure=std(twoQuadricsStructure);
attrib(twoQuadricsStructure,"isHomog",intvec(0));
resolution twoQuadricsResolution=minres(fres(twoQuadricsStructure,3));
size(twoQuadricsResolution[3]);
module twoQuadricsCanonical=KSCcanonicalModule(twoQuadrics);
attrib(twoQuadricsCanonical,"KSCcanonicalMethod");
KSCsubcanonicalIndex(twoQuadrics);
KSCplurigenera(twoQuadrics,2);

// A raw Schreyer resolution of this mixed-degree complete intersection has
// contractible summands.  Minimizing the fres segment must remove them before
// the terminal differential is dualized; otherwise the correct canonical
// module is nonminimal and the one-row K3 certificate is hidden.
ideal quadricCubic=z0^2+z1^2+z2^2+z3^2+z4^2,
                   z0^3+z1^3+z2^3+z3^3+z4^3;
module quadricCubicCanonical=KSCcanonicalModule(quadricCubic);
attrib(quadricCubicCanonical,"KSCcanonicalMethod");
nrows(quadricCubicCanonical);
attrib(quadricCubicCanonical,"isHomog");
KSCsubcanonicalIndex(quadricCubic);
setring R;

// Exercise dimH-certified scalar sections and both the estimated and exact map
// paths, including their empty, one-section, and surface-image branches.
KSCphiDim(cubic,1);
KSCphiDim(quartic,1);
KSCphiDim(quintic,1);
KSCphiDimExact(cubic,1);
KSCphiDimExact(quartic,1);
KSCphiDimExact(quintic,1);
KSCphiDimExactOriginal(cubic,1);
KSCphiDimExactOriginal(quartic,1);
KSCphiDimExactOriginal(quintic,1);
list Adj=KSCadjunctionPrepass(cubic);
Adj[1..3];
find(Adj[4],"original classifier")>0;

// Automatic classifier-level backend selection must use the original path
// whenever SpaSM cannot support the coefficient field.  The strict public
// SpaSM section commands themselves are unchanged.
ring R2=2,(z0,z1,z2,z3),dp;
ideal cubic2=z0^3+z1^3+z2^3+z3^3;
list Adj2=KSCadjunctionPrepass(cubic2);
Adj2[1..3];
find(Adj2[4],"original classifier")>0;

ring Ra=(0,a),(u0,u1,u2,u3),dp;
minpoly=a2+1;
ideal quinticA=u0^5+u1^5+u2^5+u3^5;
KSCphiDimExactOriginal(quinticA,1);

// The classifier computes Ext over the ambient polynomial ring S.  Reject a
// quotient-ring encoding instead of silently computing Ext over S/(f).
ring Rq=0,(q0,q1,q2,q3),dp;
ideal quarticQ=q0^4+q1^4+q2^4+q3^4;
qring Qq=std(quarticQ);
ideal zeroIdeal=0;
KSCplurigenera(zeroIdeal,3);

ring S=0,(y0,y1),dp;
ideal notsurface=0;
def P12bad=KSCp12Certificate(notsurface);
P12bad.classification;
P12bad.algorithm;
KSCminimalCanonicalTorsionOrder(P12bad);
setring R;

tst_status(1);$
