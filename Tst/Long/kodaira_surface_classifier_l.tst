LIB "tst.lib";
tst_init();

LIB "kodaira_surface_classifier.lib";

// A non-subcanonical determinantal surface with a multi-row canonical module.
// This reaches the classifier's cached W5/frame path and compares it with the
// independent one-shot exact construction of the same pluricanonical map.
// Force the generic-linear backend so this regression does not depend on the
// optional SpaSM module or run the deliberately slower original fallback.
system("--random",12345678);
ring R=31991,(x0,x1,x2,x3,x4),dp;
ideal linearForms=randomid(maxideal(1),2,3);
ideal quadraticForms=randomid(maxideal(2),4,3);
matrix M[2][3]=linearForms[1],quadraticForms[1],quadraticForms[2],
                 linearForms[2],quadraticForms[3],quadraticForms[4];
ideal X=minor(M,2);

def A=KSCclassify(X,3,0,0,"linear");
A.plurigenera;
A.classification;
A.kodairaDimension;
A.phi5Dimension;
KSCphiDimExact(X,5);

// The demand-driven classifier must stop a non-subcanonical Enriques example
// after h^1(O_X), P1, P2, and P3.  Force the generic-linear fractional-frame
// route so this regression is independent of the optional SpaSM module.
LIB "nonGeneralTypeSurfacesP4.lib";

// Singular suppresses trailing zero rows when a module has no explicit
// ambient-rank attribute.  This K3 surface has a five-row canonical
// presentation but its unique canonical section uses only the first row.
// Preserve all five rows on the raw basis and exercise the classifier's
// defensive scalarization through the public exact-map command.
def RK=nonGeneralTypeSurfaceP4("k3_d8_pi6");
setring RK;
module k3Canonical=KSCcanonicalModule(surfaceIdeal);
list k3Basis=sheafSectionBasis(k3Canonical,0,0,1,0);
nrows(k3Canonical);
size(k3Basis[1]);
attrib(k3Basis[1],"rank");
nrows(matrix(k3Basis[1]));
KSCphiDimExact(surfaceIdeal,1);

def RE=nonGeneralTypeSurfaceP4("enriques_d9_pi6");
setring RE;
def AE=KSCclassify(surfaceIdeal,3,0,0,"linear");
AE.classification;
AE.kodairaDimension;
AE.plurigenera;
AE.irregularity;
AE.fallbackMaxPlurigenus;
KSCminimalCanonicalTorsionOrder(AE);
KSCminimalCanonicalPowerIsTrivial(AE,1);
KSCminimalCanonicalPowerIsTrivial(AE,2);
find(AE.certificate,"P_2=1, P_3=0")>0;

// In a build with SpaSM, exercise the exact order-three theorem branch on the
// smaller stored bielliptic model.  Keep the assertions silent so this long
// regression has identical reference output when the optional module is not
// built.  The larger degree-15 model belongs in the catalogue census, not in
// the routine test suite.
if (!defined(spasm_first_kernel_vector)) { load("sispasm.so","try"); }
int testOrderThreeBielliptic=0;
if (defined(spasm_first_kernel_vector)) { testOrderThreeBielliptic=1; }
if (testOrderThreeBielliptic)
{
  def RBi=nonGeneralTypeSurfaceP4("bielliptic_d10_pi6");
  setring RBi;
  if (!spasm_supports_current_ring())
  {
    ERROR("the order-three regression requires a supported SpaSM field");
  }
  def ABi=KSCauto(surfaceIdeal);
  if (ABi.kodairaDimension!="0")
  {
    ERROR("the order-three bielliptic surface was not classified with kappa zero");
  }
  if ((size(ABi.plurigenera)!=6) ||
      (ABi.plurigenera[1]!=0) || (ABi.plurigenera[2]!=0) ||
      (ABi.plurigenera[3]!=1) || (ABi.plurigenera[4]!=0) ||
      (ABi.plurigenera[5]!=0) || (ABi.plurigenera[6]!=1))
  {
    ERROR("the order-three bielliptic plurigenus certificate is wrong");
  }
  if (KSCminimalCanonicalTorsionOrder(ABi)!=3)
  {
    ERROR("the minimal canonical torsion order should be three");
  }
  if (ABi.fallbackMaxPlurigenus!=5)
  {
    ERROR("P_6 should be inferred without constructing the sixth canonical power");
  }
  if (KSCminimalCanonicalPowerIsTrivial(ABi,2) ||
      !KSCminimalCanonicalPowerIsTrivial(ABi,3) ||
      !KSCminimalCanonicalPowerIsTrivial(ABi,6))
  {
    ERROR("canonical-power triviality does not follow the exact order three");
  }
  if (find(ABi.certificate,"cached spasm")==0)
  {
    ERROR("the automatic order-three regression did not select SpaSM");
  }
}

// On the smaller quintic elliptic scroll the non-ACM resolution has a nonzero
// third differential.  Thus the terminal-differential shortcut must decline
// and the codimension-two liaison theorem supplies omega_X.  Compare its P1
// and all twists -2,...,3 with the independent historical Ext construction.
def RS=nonGeneralTypeSurfaceP4("quintic_elliptic_scroll");
setring RS;
module linkageCanonical=KSCcanonicalModule(surfaceIdeal);
attrib(linkageCanonical,"KSCcanonicalMethod");
list oldSupportData=sat(std(surfaceIdeal),maxideal(1));
ideal oldSupport=std(oldSupportData[1]);
module oldStructure=oldSupport;
attrib(oldStructure,"isHomog",intvec(0));
resolution oldResolution=mres(oldStructure,3);
size(oldResolution[3])>0;
int savedPrintlevel=printlevel;
printlevel=-1;
module oldCanonical=Ext_R(2,oldStructure);
printlevel=savedPrintlevel;
intvec oldWeights=attrib(oldCanonical,"isHomog");
int i;
for (i=1; i<=size(oldWeights); i++) { oldWeights[i]=oldWeights[i]+5; }
attrib(oldCanonical,"isHomog",oldWeights);
list oldCanonicalSat=sat(std(oldCanonical),maxideal(1));
oldCanonical=oldCanonicalSat[1];
attrib(oldCanonical,"isHomog",oldWeights);
intvec linkageH0;
intvec oldExtH0;
int twist;
for (twist=-2; twist<=3; twist++)
{
  linkageH0[twist+3]=dimH(0,linkageCanonical,twist);
  oldExtH0[twist+3]=dimH(0,oldCanonical,twist);
}
intvec expectedTwistedH0=0,0,0,0,5,15;
linkageH0;
oldExtH0;
linkageH0==expectedTwistedH0;
oldExtH0==expectedTwistedH0;

// A positive global degBound can make raw mres falsely suppress F3.  This
// stored surface gives size(F3)=8 exactly but size(F3)=0 with degBound=4.
// The public canonical-module command must clear the bound for saturation and
// for the complete dispatch, choose linkage rather than terminal, and restore
// the caller's value.
def RB=nonGeneralTypeSurfaceP4("rational_d8_pi5");
setring RB;
int originalDegBound=degBound;
degBound=0;
list boundedSupportData=sat(std(surfaceIdeal),maxideal(1));
ideal boundedSupport=std(boundedSupportData[1]);
module boundedStructure=boundedSupport;
attrib(boundedStructure,"isHomog",intvec(0));
resolution exactResolution=mres(boundedStructure,3);
size(exactResolution[3]);
degBound=4;
resolution truncatedResolution=mres(boundedStructure,3);
size(truncatedResolution[3]);
module boundedCanonical=KSCcanonicalModule(surfaceIdeal);
attrib(boundedCanonical,"KSCcanonicalMethod");
degBound;
degBound=originalDegBound;

// The abelian P4 selector must use the cached fractional ideal only after a
// complete raw support resolution certifies the scalar slice.  This stored
// non-subcanonical model has q=2, P1=1, slice degree 20, and the certified
// nonminimal regularity bound 7.  No minimized support resolution, explicit
// section basis, or multiplication tail is needed.
def RA=nonGeneralTypeSurfaceP4("abelian_d15_pi21_quintic_3");
setring RA;
def AA=KSCclassify(surfaceIdeal,3,0,0,"linear");
AA.classification;
AA.kodairaDimension;
AA.plurigenera;
AA.irregularity;
AA.fallbackMaxPlurigenus;
KSCminimalCanonicalTorsionOrder(AA);
KSCminimalCanonicalPowerIsTrivial(AA,5);
find(AA.certificate,"Hilbert-function dimension")>0;
find(AA.certificate,"slice degree 20 >= regularity bound 7")>0;

tst_status(1);$
