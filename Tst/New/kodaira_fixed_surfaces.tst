LIB "tst.lib";
tst_init();

LIB "nonGeneralTypeSurfacesP4.lib";
LIB "kodaira_surface_classifier.lib";

// Check the mathematical result, the stopping point, and (when available)
// that the ordinary public command selected its SpaSM implementation.
proc KSCtestFixedSurface(string surfaceName,
  string expectedClass,
  string expectedKappa,
  intvec expectedP,
  int expectedQ,
  int expectedTorsion,
  int expectedMaxP,
  int requireSpaSM)
{
  def callerRing=basering;
  def exampleRing=nonGeneralTypeSurfaceP4(surfaceName);
  setring exampleRing;

  def C=KSCauto(surfaceIdeal);
  if (C.classification!=expectedClass)
  {
    ERROR(surfaceName+": wrong classification: "+C.classification);
  }
  if (C.kodairaDimension!=expectedKappa)
  {
    ERROR(surfaceName+": wrong Kodaira dimension: "+C.kodairaDimension);
  }
  if (C.plurigenera!=expectedP)
  {
    ERROR(surfaceName+": wrong plurigenera: "+string(C.plurigenera));
  }
  if (C.irregularity!=expectedQ)
  {
    ERROR(surfaceName+": wrong irregularity: "+string(C.irregularity));
  }
  if (C.minimalCanonicalTorsionOrder!=expectedTorsion)
  {
    ERROR(surfaceName+": wrong canonical torsion order: "+
          string(C.minimalCanonicalTorsionOrder));
  }
  if (C.fallbackMaxPlurigenus!=expectedMaxP)
  {
    ERROR(surfaceName+": wrong maximum plurigenus: "+
          string(C.fallbackMaxPlurigenus));
  }
  if (C.algorithm!="economical")
  {
    ERROR(surfaceName+": did not terminate in the economical classifier");
  }
  if (C.subcanonical!=0)
  {
    ERROR(surfaceName+": unexpectedly used a subcanonical shortcut");
  }
  if (requireSpaSM)
  {
    if (find(C.certificate,"cached spasm")==0)
    {
      ERROR(surfaceName+": automatic classification did not use SpaSM");
    }
  }
  setring callerRing;
}

ring capabilityRing=31991,(t),dp;

// Keep availability-dependent control flow inside a procedure so that the
// echoed test input is identical with and without the optional SpaSM module.
proc KSCrunFixedSurfaces()
{
if (!defined(spasm_first_kernel_vector))
{
  load("sispasm.so","try");
}

int spaSMUsable=0;
if (defined(spasm_first_kernel_vector))
{
  if (defined(spasm_supports_current_ring))
  {
    if (spasm_supports_current_ring())
    {
      spaSMUsable=1;
    }
  }
}

intvec zeroP=0,0,0,0,0,0;
intvec oneP=1,1,1,1,1,1;

if (spaSMUsable)
{
  KSCtestFixedSurface("bordiga","rational surface","-infinity",zeroP,0,0,2,1);
  KSCtestFixedSurface("rational_d7_pi4","rational surface","-infinity",zeroP,0,0,2,1);
  KSCtestFixedSurface("veronese","rational surface","-infinity",zeroP,0,0,2,1);
  KSCtestFixedSurface("rational_d8_pi6","rational surface","-infinity",zeroP,0,0,2,1);
  KSCtestFixedSurface("castelnuovo","rational surface","-infinity",zeroP,0,0,2,1);
  KSCtestFixedSurface("cubic_scroll","rational surface","-infinity",zeroP,0,0,2,1);
  KSCtestFixedSurface("rational_d8_pi5","rational surface","-infinity",zeroP,0,0,2,1);
  KSCtestFixedSurface("rational_d9_pi7","rational surface","-infinity",zeroP,0,0,2,1);
  KSCtestFixedSurface("rational_d10_pi9_quart_2","rational surface","-infinity",zeroP,0,0,2,1);
  KSCtestFixedSurface("rational_d10_pi9_quart_1","rational surface","-infinity",zeroP,0,0,2,1);
  KSCtestFixedSurface("rational_d10_pi8","rational surface","-infinity",zeroP,0,0,2,1);
  KSCtestFixedSurface("rational_d11_pi11_ss_inf","rational surface","-infinity",zeroP,0,0,2,1);
  KSCtestFixedSurface("rational_d9_pi6","rational surface","-infinity",zeroP,0,0,2,1);

  KSCtestFixedSurface("elliptic_d7_pi6","properly elliptic surface","1",intvec(2,3),0,0,2,1);
  KSCtestFixedSurface("elliptic_d8_pi7","properly elliptic surface","1",intvec(2,3),0,0,2,1);
  KSCtestFixedSurface("elliptic_d10_pi10","properly elliptic surface","1",intvec(2,3),0,0,2,1);
  KSCtestFixedSurface("elliptic_d12_pi13","properly elliptic surface","1",intvec(2,3),0,0,2,1);
  KSCtestFixedSurface("elliptic_d12_pi14_ss_inf","properly elliptic surface","1",intvec(2,3),0,0,2,1);
  KSCtestFixedSurface("elliptic_d9_pi7","properly elliptic surface","1",intvec(1,2),0,0,2,1);
  KSCtestFixedSurface("elliptic_d10_pi9","properly elliptic surface","1",intvec(1,2),0,0,2,1);

  KSCtestFixedSurface("abelian_d10_pi6","abelian surface","0",oneP,2,1,4,1);
  KSCtestFixedSurface("abelian_d15_pi21_quintic_3","abelian surface","0",oneP,2,1,4,1);

  KSCtestFixedSurface("k3_d7_pi5","K3 surface","0",oneP,0,1,2,1);
  KSCtestFixedSurface("k3_d8_pi6","K3 surface","0",oneP,0,1,2,1);
  KSCtestFixedSurface("k3_d9_pi8","K3 surface","0",oneP,0,1,2,1);
  KSCtestFixedSurface("k3_d10_pi9_quart_1","K3 surface","0",oneP,0,1,2,1);
  KSCtestFixedSurface("k3_d11_pi11_ss_2","K3 surface","0",oneP,0,1,2,1);
  KSCtestFixedSurface("k3_d10_pi9_quart_2","K3 surface","0",oneP,0,1,2,1);

  KSCtestFixedSurface("quintic_elliptic_scroll",
    "ruled surface over a curve of genus h^1(O_X)","-infinity",zeroP,1,0,6,1);

  KSCtestFixedSurface("enriques_d10_pi8","Enriques surface","0",intvec(0,1,0,1,0,1),0,2,3,1);
  KSCtestFixedSurface("enriques_d9_pi6","Enriques surface","0",intvec(0,1,0,1,0,1),0,2,3,1);
  KSCtestFixedSurface("enriques_d11_pi10","Enriques surface","0",intvec(0,1,0,1,0,1),0,2,3,1);

  KSCtestFixedSurface(
    "bielliptic_d10_pi6",
    "hyperelliptic-type kappa 0 surface in positive characteristic; verify canonical torsion for the fine subtype",
    "0",intvec(0,0,1,0,0,1),1,3,5,1);
}
else
{
  // Keep source builds without the optional module useful and bounded while
  // still exercising three distinct automatic-classifier decisions.
  KSCtestFixedSurface("rational_d7_pi4","rational surface","-infinity",
                      zeroP,0,0,2,0);
  KSCtestFixedSurface("k3_d7_pi5","K3 surface","0",oneP,0,1,2,0);
  KSCtestFixedSurface("elliptic_d7_pi6","properly elliptic surface","1",
                      intvec(2,3),0,0,2,0);
}
}

KSCrunFixedSurfaces();

print("KSC_FIXED_SURFACES_OK");
tst_status(1);$
