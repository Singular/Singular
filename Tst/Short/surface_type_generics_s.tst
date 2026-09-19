LIB "tst.lib";
tst_init();

LIB "surface_type_generics.lib";
system("--random",12345678);

ring r=31991,(dummy),dp;

size(surface_generic_surface_types());
surface_generic_catalogueType("k3_d12_pi14");
surface_generic_catalogueType("elliptic_d12_pi14_ss_0");
surface_generic_catalogueType("unknown_label");
size(surface_generic_catalogueNames());

list S=surface_generic_cubic_scroll(3);
S[3];
setring S[1];
nvars(basering);
size(S[2]);
dim(std(S[2]));

ring r2=31991,(dummy),dp;
list V=surface_generic_projected_veronese(3);
V[3];
setring V[1];
nvars(basering);
size(V[2]);
dim(std(V[2]));

ring r3=31991,(dummy),dp;
list K=surface_generic_K3(3);
K[3];
setring K[1];
nvars(basering);
size(K[2]);
dim(std(K[2]));

ring r4=31991,(dummy),dp;
list A=surface_generic_abelian_product(2,5);
A[3];
setring A[1];
nvars(basering);
size(A[2]);
dim(std(A[2]));

ring r5=31991,(dummy),dp;
list R=surface_generic_elliptic_ruled(2);
R[3];
setring R[1];
nvars(basering);
size(R[2]);
dim(std(R[2]));

ring r6=31991,(dummy),dp;
list E=surface_generic_elliptic_surface(2,3);
E[3];
setring E[1];
nvars(basering);
size(E[2]);
dim(std(E[2]));

ring r7=31991,(dummy),dp;
list C=surface_generic_Enriques_cover(3);
C[3];
setring C[1];
nvars(basering);
size(C[2]);
dim(std(C[2]));

ring r8=31991,(dummy),dp;
list Q=surface_generic_Enriques_quotient_image(3);
Q[3];
setring Q[1];
nvars(basering);
size(Q[2]);
dim(std(Q[2]));

ring r9=31991,(dummy),dp;
list B=surface_generic_bielliptic_cover_data(2,5);
B[3];
setring B[1];
nvars(basering);
size(B[2]);
dim(std(B[2]));

// Full older 34-dimensional Decker--Ein--Schreyer d=11, pi=10 family.
// This follows Popescu's liaison chain and certifies all smoothness hypotheses.
system("--random",12345678);
ring r10=1019,(dummy),dp;
list DES=surface_generic_enriques_d11_pi10_DES_dim34(20,1,20);
size(DES);
DES[3];
setring DES[1];
ideal rawDES=DES[2];
attrib(rawDES,"familyDimension");
attrib(rawDES,"constructionAttempts")>0;
attrib(rawDES,"smoothnessChecked");
attrib(rawDES,"residualScrollDegree");
ideal IDES=minbase(rawDES);
nvars(basering);
homog(IDES);
ideal GDES=std(IDES);
dim(GDES);
mult(GDES);
intvec hpDES=hilbPoly(GDES);
hpDES;
1+(hpDES[3]-hpDES[2]) div 2;
int quinticsDES;
int sexticsDES;
int desIndex;
ideal quinticPartDES=ideal();
for(desIndex=1;desIndex<=size(IDES);desIndex=desIndex+1)
{
  if(deg(IDES[desIndex])==5)
  {
    quinticsDES=quinticsDES+1;
    quinticPartDES[size(quinticPartDES)+1]=IDES[desIndex];
  }
  if(deg(IDES[desIndex])==6) { sexticsDES=sexticsDES+1; }
}
quinticsDES,sexticsDES;
ideal residualScrollDES=std(quotient(quinticPartDES,IDES));
dim(residualScrollDES);
mult(residualScrollDES);
hilbPoly(residualScrollDES);
size(minbase(residualScrollDES));
ideal residualIntersectionDES=std(residualScrollDES+IDES);
dim(residualIntersectionDES);
mult(residualIntersectionDES);

// Retain and test the existing normalized Moore/Bring syzygy specialization as a
// separate constructor; it is locally incomplete inside the 34-dimensional
// family, but produces the same degree and generator profile.
system("--random",12345678);
ring r11=1009,(dummy),dp;
list MB=surface_generic_enriques_d11_pi10_MB_dim31(20,0,20);
size(MB);
MB[3];
setring MB[1];
ideal rawMB=MB[2];
attrib(rawMB,"constructionDimensionUpperBound");
attrib(rawMB,"ambientDESFamilyDimension");
attrib(rawMB,"smoothnessChecked");
ideal IMB=minbase(rawMB);
nvars(basering);
homog(IMB);
ideal GMB=std(IMB);
dim(GMB);
mult(GMB);
intvec hpMB=hilbPoly(GMB);
hpMB;
int quinticsMB;
int sexticsMB;
int mbIndex;
for(mbIndex=1;mbIndex<=size(IMB);mbIndex=mbIndex+1)
{
  if(deg(IMB[mbIndex])==5)
  {
    quinticsMB=quinticsMB+1;
  }
  if(deg(IMB[mbIndex])==6)
  {
    sexticsMB=sexticsMB+1;
  }
}
quinticsMB,sexticsMB;

// Stored P4 catalogue: fields, generators, fingerprints and ring lifetime.
LIB "nonGeneralTypeSurfacesP4.lib";
proc surface_generic_testFixed(string name)
{
  def fixedRing=nonGeneralTypeSurfaceP4(name);
  setring fixedRing;
  intvec degrees;
  int termCount;
  int j;
  for(j=1;j<=size(surfaceIdeal);j=j+1)
  {
    degrees[j]=deg(surfaceIdeal[j]);
    termCount=termCount+size(surfaceIdeal[j]);
  }
  print(name+"|"+string(char(basering))+"|"+string(nvars(basering))+
        "|"+string(size(surfaceIdeal))+"|"+string(degrees)+"|"+string(termCount));
  print(attrib(surfaceIdeal,"catalogueName")+"|"+attrib(surfaceIdeal,"catalogueType")+
        "|"+attrib(surfaceIdeal,"sourceSHA256"));
  map evaluate=basering,2,3,5,7,11;
  print(string(evaluate(surfaceIdeal)));
}
list fixedNames=nonGeneralTypeSurfaceP4Names();
int fixedIndex;
for(fixedIndex=1;fixedIndex<=size(fixedNames);fixedIndex=fixedIndex+1)
{
  surface_generic_testFixed(fixedNames[fixedIndex]);
}

ring fixedCaller=0,(dummy),dp;
poly callerObject=dummy^2+1;
system("--random",314159);
int nextWithoutLoad=random(1,1000000);
system("--random",314159);
def firstFixed=nonGeneralTypeSurfaceP4("bordiga");
int nextAfterLoad=random(1,1000000);
nextWithoutLoad==nextAfterLoad;
char(basering)==0;
callerObject==dummy^2+1;
setring firstFixed;
ideal firstFixedIdeal=surfaceIdeal;
def repeatedFixed=nonGeneralTypeSurfaceP4("bordiga");
setring repeatedFixed;
ideal mappedFirst=imap(firstFixed,firstFixedIdeal);
int generatorsAgree=1;
for(fixedIndex=1;fixedIndex<=size(surfaceIdeal);fixedIndex=fixedIndex+1)
{
  if(surfaceIdeal[fixedIndex]!=mappedFirst[fixedIndex])
  {
    generatorsAgree=0;
  }
}
generatorsAgree;
proc surface_generic_testFixedWrapper(string name)
{
  return(nonGeneralTypeSurfaceP4(name));
}
def wrappedFixed=surface_generic_testFixedWrapper("enriques_d11_pi10");
setring wrappedFixed;
char(basering)==43;
attrib(surfaceIdeal,"catalogueName")=="enriques_d11_pi10";
setring firstFixed;
surfaceIdeal[1]==firstFixedIdeal[1];
setring fixedCaller;
callerObject==dummy^2+1;

tst_status(1);$
