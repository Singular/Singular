#!/bin/sh

directAutoLoad=$("$SINGULAR_BIN_DIR/Singular" -q -c '
ring r=32003,(x,y,z),dp;
int directAutoLoadAdvertised=system("with","spasm");
load("sispasm.so","try");
int directAutoLoadOK=1;
if (!defined(spasm_first_kernel_vector)) { directAutoLoadOK=0; }
if (!defined(spasm_supports_current_ring)) { directAutoLoadOK=0; }
if (directAutoLoadOK)
{
  if (!spasm_supports_current_ring()) { directAutoLoadOK=0; }
}
if (directAutoLoadOK) { print("SPASM_DIRECT_AUTOLOAD_OK"); }
else
{
  if (directAutoLoadAdvertised) { print("SPASM_DIRECT_AUTOLOAD_FAIL"); }
  else { print("SPASM_DIRECT_AUTOLOAD_SKIP"); }
}
quit;
')
case "$directAutoLoad" in
  *SPASM_DIRECT_AUTOLOAD_OK*|*SPASM_DIRECT_AUTOLOAD_SKIP*) ;;
  *) echo "$directAutoLoad"; exit 1 ;;
esac

automaticLoad=$("$SINGULAR_BIN_DIR/Singular" -q -c '
ring r=32003,(x,y,z),dp;
LIB "sheafcoh.lib";
// Test generic lazy loading in its own process, independently of the
// classifiers automatic loader in the main regression below.
module P=-x*gen(1)+gen(2);
attrib(P,"isHomog",intvec(-1,0));
SectionSpace V=lineBundleSectionBasis(P,0,ideal(0));
if (size(V)!=3) { print("AUTO_SECTION_FAIL"); }
else
{
  if (defined(spasm_first_kernel_vector)) { print("AUTO_SECTION_SPASM"); }
  else { print("AUTO_SECTION_GENERIC"); }
}
quit;
')
case "$automaticLoad" in
  *AUTO_SECTION_SPASM*|*AUTO_SECTION_GENERIC*) ;;
  *) echo "$automaticLoad"; exit 1 ;;
esac

result=$("$SINGULAR_BIN_DIR/Singular" -q -c '
ring r=32003,(x,y,z),dp;
int spasmAdvertised=system("with","spasm");
LIB "sheafcoh.lib";
LIB "kodaira_surface_classifier.lib";
// The automatic classifier must itself load a compatible developer-tree
// module even when the main executable reflects an older configure result.
ring automaticRing=32003,(u0,u1,u2,u3),dp;
ideal automaticCubic=u0^3+u1^3+u2^3+u3^3;
list automaticAdjunction=KSCadjunctionPrepass(automaticCubic);
setring r;
int completeSpaSMInterface=1;
if (!defined(spasm_first_kernel_vector)) { completeSpaSMInterface=0; }
if (!defined(spasm_kernel_basis)) { completeSpaSMInterface=0; }
if (!completeSpaSMInterface)
{
  if (spasmAdvertised) { print("SPASM_TEST_FAIL"); }
  else
  {
    if (find(automaticAdjunction[4],"original classifier")==0)
    {
      print("SPASM_TEST_FAIL");
    }
    else { print("SPASM_TEST_SKIP"); }
  }
}
else
{
  int ok=spasm_supports_current_ring();
  module automaticPresentation=-x*gen(1)+gen(2);
  attrib(automaticPresentation,"isHomog",intvec(-1,0));
  SectionSpace automaticSections=lineBundleSectionBasis(
                                  automaticPresentation,0,ideal(0));
  if (size(automaticSections)!=3) { ok=0; }
  RankOneSheaf automaticSheaf=lineBundle(automaticPresentation,ideal(0));
  SectionSpace automaticTwist=lineBundleSectionBasis(automaticSheaf,1);
  if (size(automaticTwist)!=6) { ok=0; }
  matrix A[2][3]=1,2,3,2,4,6;
  module K=spasm_first_kernel_vector(module(A));
  if (size(K)==0) { ok=0; }
  if (size(module(A*matrix(K)))!=0) { ok=0; }
  module fullK=spasm_kernel_basis(module(A));
  if ((size(fullK)!=2) || (nrows(matrix(fullK))!=3)) { ok=0; }
  if (size(module(A*matrix(fullK)))!=0) { ok=0; }
  matrix B[3][2]=1,0,0,1,1,1;
  module L=spasm_first_kernel_vector(module(B));
  if (size(L)!=0) { ok=0; }
  if (nrows(matrix(L))!=2) { ok=0; }
  module fullL=spasm_kernel_basis(module(B));
  if ((size(fullL)!=0) || (nrows(matrix(fullL))!=2)) { ok=0; }
  matrix zeroMatrix[2][3];
  module fullZeroKernel=spasm_kernel_basis(module(zeroMatrix));
  if ((size(fullZeroKernel)!=3) ||
      (nrows(matrix(fullZeroKernel))!=3)) { ok=0; }
  matrix C[3][2]=1,0,0,1,0,0;
  spasm sparseC=module(C);
  sparseC=sparseC;
  matrix roundTripC=to_matrix(sparseC);
  if ((nrows(roundTripC)!=3) || (ncols(roundTripC)!=2)) { ok=0; }
  module P=-x*gen(1)+gen(2);
  attrib(P,"isHomog",intvec(-1,0));
  SectionSpace V=rankOneSheafSectionBasisSpaSM(P,0,ideal(0));
  SectionSpace W=lineBundleSectionBasisSpaSM(P,0,ideal(0));
  SectionSpace G=rankOneSheafSectionBasisLinear(P,0,ideal(0));
  if ((size(V)!=3) || (size(W)!=3) || (size(G)!=3)) { ok=0; }
  ideal VG=V.basis*G.denom;
  ideal GV=G.basis*V.denom;
  if (size(NF(VG,std(GV)))!=0) { ok=0; }
  if (size(NF(GV,std(VG)))!=0) { ok=0; }
  ideal automaticCoordinates=automaticSections.basis*V.denom;
  ideal forcedCoordinates=V.basis*automaticSections.denom;
  if (size(NF(automaticCoordinates,std(forcedCoordinates)))!=0) { ok=0; }
  if (size(NF(forcedCoordinates,std(automaticCoordinates)))!=0) { ok=0; }
  // A nonlinear prime support exercises the primality preflight. Reducible
  // and nonreduced supports must retain the validated original frame method.
  ideal conicSupport=x2+y*z;
  module conicPresentation=-x*gen(1)+gen(2),
    conicSupport[1]*gen(1),conicSupport[1]*gen(2);
  attrib(conicPresentation,"isHomog",intvec(-1,0));
  SectionSpace conicSections=lineBundleSectionBasis(
                              conicPresentation,0,conicSupport);
  if (size(conicSections)!=3) { ok=0; }
  module reduciblePresentation=y*gen(1),x*gen(2);
  attrib(reduciblePresentation,"isHomog",intvec(0,0));
  SectionSpace reducibleSections=rankOneSheafSectionBasis(
                                  reduciblePresentation,0,ideal(x*y));
  if (size(reducibleSections)!=2) { ok=0; }
  module thickenedLine=x2;
  attrib(thickenedLine,"isHomog",intvec(0));
  SectionSpace thickenedSections=lineBundleSectionBasis(
                                  thickenedLine,0,ideal(x2));
  if (size(thickenedSections)!=1) { ok=0; }
  SectionSpace BGGSpace=rankOneSheafSectionBasisBGGSpaSM(P,0,ideal(0),1,x);
  if (size(BGGSpace)!=3) { ok=0; }
  module sectionPoint=x,y;
  attrib(sectionPoint,"isHomog",intvec(0));
  module sectionTailSpaSM=sheafSectionModuleSpaSM(sectionPoint,-2);
  module sectionTailAuto=sheafSectionModule(sectionPoint,-2);
  module sectionTailLinear=sheafSectionModuleLinear(sectionPoint,-2);
  if ((dimGradedPart(sectionTailSpaSM,-3)!=0) ||
      (dimGradedPart(sectionTailSpaSM,-2)!=1) ||
      (dimGradedPart(sectionTailSpaSM,2)!=1)) { ok=0; }
  if ((dimGradedPart(sectionTailAuto,-2)!=1) ||
      (dimGradedPart(sectionTailLinear,-2)!=1)) { ok=0; }
  ring sectionQAmbient=32003,(a,b,c),dp;
  qring sectionQ=std(a2+b2+c2);
  module sectionQO=0;
  attrib(sectionQO,"isHomog",intvec(0));
  module sectionQTail=sheafSectionModuleSpaSM(sectionQO,-1);
  if ((dimGradedPart(sectionQTail,-1)!=0) ||
      (dimGradedPart(sectionQTail,0)!=1) ||
      (dimGradedPart(sectionQTail,2)!=5)) { ok=0; }
  RankOneSheaf automaticQSheaf=lineBundle(sectionQO,ideal(0));
  SectionSpace automaticQSections=lineBundleSectionBasis(automaticQSheaf,1);
  if (basering!=sectionQ) { ok=0; }
  if (size(automaticQSections)!=3) { ok=0; }
  setring sectionQAmbient;
  qring reducibleQ=std(a*b);
  module reducibleQPresentation=b*gen(1),a*gen(2);
  attrib(reducibleQPresentation,"isHomog",intvec(0,0));
  SectionSpace reducibleQSections=rankOneSheafSectionBasis(
                                   reducibleQPresentation,0,ideal(0));
  if (basering!=reducibleQ) { ok=0; }
  if (size(reducibleQSections)!=2) { ok=0; }
  ring sectionPZero=32003,(t),dp;
  module sectionPZeroO=0;
  attrib(sectionPZeroO,"isHomog",intvec(0));
  module sectionPZeroTail=sheafSectionModuleSpaSM(sectionPZeroO,-2);
  if ((dimGradedPart(sectionPZeroTail,-3)!=0) ||
      (dimGradedPart(sectionPZeroTail,-2)!=1) ||
      (dimGradedPart(sectionPZeroTail,1)!=1)) { ok=0; }
  ring r4=32003,(x0,x1,x2,x3),dp;
  ideal quintic=x0^5+x1^5+x2^5+x3^5;
  int phiLinear=KSCphiDim(quintic,1);
  int phiSpaSM=KSCphiDimSpaSM(quintic,1);
  int phiExactSpaSM=KSCphiDimExactSpaSM(quintic,1);
  if ((phiLinear!=2) || (phiSpaSM!=2) || (phiExactSpaSM!=2)) { ok=0; }
  if (find(automaticAdjunction[4],"SpaSM")==0) { ok=0; }
  // Exercise the demand-driven automatic classifier on a small, deterministic,
  // non-subcanonical elliptic surface.  The exact P2 Riemann--Roch gap now
  // proves kappa=1 before |5K| is formed; the exact SpaSM map command and the
  // automatic SpaSM selector are tested independently above.
  system("--random",12345678);
  ring r5=31991,(v0,v1,v2,v3,v4),dp;
  ideal automaticLinearForms=randomid(maxideal(1),4,3);
  ideal automaticCubicForms=randomid(maxideal(3),2,3);
  matrix automaticMatrix[2][3]=
    automaticLinearForms[1],automaticLinearForms[2],automaticCubicForms[1],
    automaticLinearForms[3],automaticLinearForms[4],automaticCubicForms[2];
  ideal automaticSurface=minor(automaticMatrix,2);
  def automaticClassification=KSCclassify(automaticSurface);
  if (automaticClassification.subcanonical!=0) { ok=0; }
  if (automaticClassification.kodairaDimension!="1") { ok=0; }
  if ((size(automaticClassification.plurigenera)!=2) ||
      (automaticClassification.plurigenera[1]!=2) ||
      (automaticClassification.plurigenera[2]!=3)) { ok=0; }
  if (automaticClassification.fallbackMaxPlurigenus!=2) { ok=0; }
  if (automaticClassification.phi5Dimension!=-1) { ok=0; }
  if (find(automaticClassification.certificate,
           "no higher canonical power or rational map was constructed")==0)
  {
    ok=0;
  }
  ring r2=2,(x,y,z),dp;
  if (spasm_supports_current_ring()) { ok=0; }
  module P2=-x*gen(1)+gen(2);
  attrib(P2,"isHomog",intvec(-1,0));
  // The explicit generic command remains available, independently of SpaSM.
  SectionSpace V2=rankOneSheafSectionBasisLinear(P2,0,ideal(0));
  if (size(V2)!=3) { ok=0; }
  SectionSpace automaticTwo=lineBundleSectionBasis(P2,0,ideal(0));
  if (size(automaticTwo)!=3) { ok=0; }
  module sectionPoint2=x,y;
  attrib(sectionPoint2,"isHomog",intvec(0));
  module sectionTailAuto2=sheafSectionModule(sectionPoint2,-2);
  module sectionTailLinear2=sheafSectionModuleLinear(sectionPoint2,-2);
  if ((dimGradedPart(sectionTailAuto2,-2)!=1) ||
      (dimGradedPart(sectionTailLinear2,-2)!=1)) { ok=0; }
  ring rationalRing=0,(x,y,z),dp;
  module rationalPresentation=-x*gen(1)+gen(2);
  attrib(rationalPresentation,"isHomog",intvec(-1,0));
  SectionSpace rationalSections=lineBundleSectionBasis(
                                 rationalPresentation,0,ideal(0));
  if (size(rationalSections)!=3) { ok=0; }
  if (ok) { print("SPASM_TEST_OK"); }
  else { print("SPASM_TEST_FAIL"); }
}
quit;
')

case "$result" in
  *SPASM_TEST_SKIP*)
    case "$automaticLoad" in
      *AUTO_SECTION_GENERIC*) ;;
      *) echo "$automaticLoad"; exit 1 ;;
    esac
    unavailable=$("$SINGULAR_BIN_DIR/Singular" -q -c '
ring r=32003,(x,y,z),dp;
LIB "sheafcoh.lib";
module O=0;
attrib(O,"isHomog",intvec(0));
rankOneTrivializationSpaSM(O,ideal(0));
quit;
' 2>&1)
    unavailableSection=$("$SINGULAR_BIN_DIR/Singular" -q -c '
ring r=32003,(x,y,z),dp;
LIB "sheafcoh.lib";
module O=0;
attrib(O,"isHomog",intvec(0));
sheafSectionModuleSpaSM(O,-1);
quit;
' 2>&1)
    case "$unavailable" in
      *"SpaSM kernel requested, but the sispasm module is not available"*) ;;
      *) echo "$unavailable"; exit 1 ;;
    esac
    case "$unavailableSection" in
      *"SpaSM kernel requested, but the sispasm module is not available"*) exit 77 ;;
      *) echo "$unavailableSection"; exit 1 ;;
    esac
    ;;
  *SPASM_TEST_OK*)
    case "$automaticLoad" in
      *AUTO_SECTION_SPASM*) ;;
      *) echo "$automaticLoad"; exit 1 ;;
    esac
    ;;
  *) echo "$result"; exit 1 ;;
esac

# A strict SpaSM request must fail before even the free-module shortcut when
# the current coefficient field is unsupported.
unsupported=$("$SINGULAR_BIN_DIR/Singular" -q -c '
ring r=2,(x,y,z),dp;
LIB "sheafcoh.lib";
module O=0;
attrib(O,"isHomog",intvec(0));
rankOneTrivializationSpaSM(O,ideal(0));
quit;
' 2>&1)

unsupportedSection=$("$SINGULAR_BIN_DIR/Singular" -q -c '
ring r=2,(x,y,z),dp;
LIB "sheafcoh.lib";
module O=0;
attrib(O,"isHomog",intvec(0));
sheafSectionModuleSpaSM(O,-1);
quit;
' 2>&1)

case "$unsupported" in
  *"SpaSM kernel requested, but the current coefficient field is unsupported"*) ;;
  *) echo "$unsupported"; exit 1 ;;
esac

case "$unsupportedSection" in
  *"SpaSM kernel requested, but the current coefficient field is unsupported"*) exit 0 ;;
  *) echo "$unsupportedSection"; exit 1 ;;
esac
