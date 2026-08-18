LIB "tst.lib";
tst_init();

proc leftRelationsAreZero(module generators, module relations)
{
  int i,j;
  vector image;
  for(i=1; i<=size(relations); i++)
  {
    image=0;
    for(j=1; j<=size(generators); j++)
    {
      image=image+relations[i][j]*generators[j];
    }
    if(image!=0)
    {
      return(0);
    }
  }
  return(1);
}

proc moduleReducesToZero(module m, module reducers)
{
  module G=std(reducers);
  int i;
  for(i=1; i<=size(m); i++)
  {
    if(reduce(m[i],G)!=0)
    {
      return(0);
    }
  }
  return(1);
}

LIB "sresext.lib";
ring r=0,(x,y,z),dp;
def E=Exterior(); setring E;

// Coefficients and PBW signs must be included in every lift quotient.
module m=x+2y,y+3z;
m=std(m);
module s=schreyerSyzExt(m);
ASSUME(0,leftRelationsAreZero(m,s));
module builtinSyz=syz(m);
ASSUME(0,moduleReducesToZero(builtinSyz,s));
ASSUME(0,moduleReducesToZero(s,builtinSyz));

// Higher lifts must use the inherited Schreyer order consistently.
resolution sr=sresExt(m,4);
module d1=sr[1];
module d2=sr[2];
module d3=sr[3];
module d4=sr[4];
ASSUME(0,leftRelationsAreZero(d1,d2));
ASSUME(0,leftRelationsAreZero(d2,d3));
ASSUME(0,leftRelationsAreZero(d3,d4));

// The order regression is independent of nontrivial coefficients.
module u=y+z,x-z;
resolution ur=sresExt(u,3);
module ud2=ur[2];
module ud3=ur[3];
ASSUME(0,leftRelationsAreZero(ud2,ud3));

// The ideal-first presentation gives the projected syzygy from the
// lifting example in sresext.lib's accompanying Schreyer construction.
vector f1=[x,y,0,z,0,0];
vector f2=[0,x,y,0,z,0];
vector f3=[0,0,0,x,y,z];
module B=std(module(f1,f2,f3));
module S=schreyerSyzExt(B);
ASSUME(0,leftRelationsAreZero(B,S));
ASSUME(0,S[1]==x*gen(1)+y*gen(2)+z*gen(3));

// Projection takes full normal forms modulo all square generators.
vector projectionF=[x,y];
vector projectionG=[x+y,z];
module projectionModule=std(module(projectionF,projectionG));
resolution projectionRes=sresExt(projectionModule,3);
module projectionD1=projectionRes[1];
module projectionD2=projectionRes[2];
module projectionD3=projectionRes[3];
ASSUME(0,leftRelationsAreZero(projectionD1,projectionD2));
ASSUME(0,leftRelationsAreZero(projectionD2,projectionD3));

// Stop cleanly when a resolution reaches a zero syzygy module.
module zeroModule=0;
module zeroSyz=schreyerSyzExt(zeroModule);
ASSUME(0,size(zeroSyz)==0);
resolution zeroRes=sresExt(zeroModule,4);
ASSUME(0,size(zeroRes)==1);
module unitModule=1;
module unitSyz=schreyerSyzExt(unitModule);
ASSUME(0,size(unitSyz)==0);
resolution unitRes=sresExt(unitModule,4);
ASSUME(0,size(unitRes)==2);

// Variable names ending in "2" are not square monomials.
ring namedVars=0,(x1,x2),dp;
def namedExterior=Exterior(); setring namedExterior;
module namedModule=x2;
module namedSyz=schreyerSyzExt(namedModule);
ASSUME(0,leftRelationsAreZero(namedModule,namedSyz));
ASSUME(0,namedSyz[1]==x2*gen(1));

// Preserve the ambient free-module rank when projected rows vanish.
ring finiteField=5,(x,y,z),dp;
def finiteExterior=Exterior(); setring finiteExterior;
module finiteModule=std(module(x+2y,y+3z));
resolution finiteRes=sresExt(finiteModule,5);
module fd1=finiteRes[1];
module fd2=finiteRes[2];
module fd3=finiteRes[3];
module fd4=finiteRes[4];
module fd5=finiteRes[5];
ASSUME(0,leftRelationsAreZero(fd1,fd2));
ASSUME(0,leftRelationsAreZero(fd2,fd3));
ASSUME(0,leftRelationsAreZero(fd3,fd4));
ASSUME(0,leftRelationsAreZero(fd4,fd5));

tst_status(1);$
