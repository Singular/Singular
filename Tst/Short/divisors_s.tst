//divisors_s.tst
//short tests for divisors.lib
//---------------------------
LIB "tst.lib";
tst_init();
LIB "divisors.lib";
example makeDivisor;
example divisorplus;
example multdivisor;
example isEqualDivisor;
example globalSectionsIdeal;
example rankOneSheafFromDivisor;
example lineBundleFromDivisor;
example globalSections;
example globalSectionsMultTable;
example globalSectionsBGG;
example degreeDivisor;
example linearlyEquivalent;
example effective;
example makeFormalDivisor;
example evaluateFormalDivisor;
example formaldivisorplus;
example multformaldivisor;
example degreeFormalDivisor;

// The private trusted-frame shortcut has the same section span for automatic,
// explicit-tail, and fully explicit multiplication-table calls:
ring trustedRing=0,(x,y,z),dp;
divisor trustedDivisor=makeDivisor(ideal(x2),ideal(y));
list trustedDirect=globalSections(trustedDivisor);
list trustedAutomatic=globalSectionsMultTable(trustedDivisor,0);
list trustedTail=globalSectionsMultTable(trustedDivisor,0,0);
list trustedFixed=globalSectionsMultTable(trustedDivisor,0,0,poly(1),0);
list trustedVerified=globalSectionsMultTable(trustedDivisor,0,0,poly(1),1);
list trustedBGG=globalSectionsBGG(trustedDivisor,0,1,z);
size(trustedAutomatic[1]);
size(trustedTail[1]);
size(trustedFixed[1]);
size(trustedVerified[1]);
size(trustedBGG[1]);
ideal trustedDirectCoordinates=trustedDirect[1]*trustedAutomatic[2];
ideal trustedAutomaticCoordinates=trustedAutomatic[1]*trustedDirect[2];
size(NF(trustedDirectCoordinates,std(trustedAutomaticCoordinates)));
size(NF(trustedAutomaticCoordinates,std(trustedDirectCoordinates)));
ideal trustedTailCoordinates=trustedTail[1]*trustedFixed[2];
ideal trustedFixedCoordinates=trustedFixed[1]*trustedTail[2];
size(NF(trustedTailCoordinates,std(trustedFixedCoordinates)));
size(NF(trustedFixedCoordinates,std(trustedTailCoordinates)));
ideal trustedBGGCoordinates=trustedBGG[1]*trustedFixed[2];
ideal trustedFixedBGGCoordinates=trustedFixed[1]*trustedBGG[2];
size(NF(trustedBGGCoordinates,std(trustedFixedBGGCoordinates)));
size(NF(trustedFixedBGGCoordinates,std(trustedBGGCoordinates)));

// The divisor bridge has to lift a qring module to its polynomial ambient:
ring r=31991,(x,y,z),dp;
ideal I=y2*z-x*(x-z)*(x+3z);
qring Q=std(I);
divisor P=makeDivisor(ideal(x,z),ideal(1));

// A divisor-created cache retains its canonical fractional frame.  Both
// generic constructor names use that cache, which can serve several twists
// without rediscovering a functional.
divisor cachedDivisor=multdivisor(4,P);
list canonicalFractional=globalSectionsIdeal(cachedDivisor);
RankOneSheaf cachedSheaf=rankOneSheaf(cachedDivisor);
RankOneSheaf cachedLineBundle=lineBundle(cachedDivisor);
int cachedFrameMatches=
  (cachedSheaf.trivializationDenom==canonicalFractional[2]);
if (size(cachedSheaf.trivializationImages)!=size(canonicalFractional[1]))
{
  cachedFrameMatches=0;
}
else
{
  int cachedFrameIndex;
  for (cachedFrameIndex=1;
       cachedFrameIndex<=size(canonicalFractional[1]);
       cachedFrameIndex++)
  {
    if (cachedSheaf.trivializationImages[cachedFrameIndex]
        !=canonicalFractional[1][cachedFrameIndex])
    {
      cachedFrameMatches=0;
    }
  }
}
cachedFrameMatches;
cachedSheaf.frameValidated;
cachedLineBundle.frameValidated;
cachedSheaf.trivializationShift;

SectionSpace cachedTwist0=rankOneSheafSectionBasis(cachedSheaf,0);
SectionSpace cachedTwist1=rankOneSheafSectionBasis(cachedSheaf,1);
SectionSpace cachedLineTwist0=lineBundleSectionBasis(cachedLineBundle,0);
list uncachedTwist0=globalSectionsMultTable(cachedDivisor,0);
list uncachedTwist1=globalSectionsMultTable(cachedDivisor,1);
list cachedBGGTwist0=globalSectionsBGG(cachedDivisor,0);
size(cachedTwist0);
size(cachedTwist1);
size(cachedLineTwist0);
size(cachedBGGTwist0[1]);
ideal cachedTwist0Coordinates=cachedTwist0.basis*uncachedTwist0[2];
ideal uncachedTwist0Coordinates=uncachedTwist0[1]*cachedTwist0.denom;
size(NF(cachedTwist0Coordinates,std(uncachedTwist0Coordinates)));
size(NF(uncachedTwist0Coordinates,std(cachedTwist0Coordinates)));
ideal cachedBGGCoordinates=cachedBGGTwist0[1]*uncachedTwist0[2];
ideal cachedMultTableBGGCoordinates=uncachedTwist0[1]*cachedBGGTwist0[2];
size(NF(cachedBGGCoordinates,std(cachedMultTableBGGCoordinates)));
size(NF(cachedMultTableBGGCoordinates,std(cachedBGGCoordinates)));
ideal cachedTwist1Coordinates=cachedTwist1.basis*uncachedTwist1[2];
ideal uncachedTwist1Coordinates=uncachedTwist1[1]*cachedTwist1.denom;
size(NF(cachedTwist1Coordinates,std(uncachedTwist1Coordinates)));
size(NF(uncachedTwist1Coordinates,std(cachedTwist1Coordinates)));

SectionSpace cachedDirect=rankOneSheafSectionBasisDirect(cachedSheaf,0);
size(cachedDirect);
ideal cachedDirectCoordinates=cachedDirect.basis*uncachedTwist0[2];
ideal cachedMultTableCoordinates=uncachedTwist0[1]*cachedDirect.denom;
size(NF(cachedDirectCoordinates,std(cachedMultTableCoordinates)));
size(NF(cachedMultTableCoordinates,std(cachedDirectCoordinates)));

// Here the canonical fractional ideal has generators in two degrees, while
// H^0(P) uses only the first module row. Trusted scalarization must pad the
// dropped trailing zero row before its kernel matrix multiplication.
list trailingRowBasis=globalSectionsMultTable(P,0);
size(trailingRowBasis[1]);
list B=globalSectionsMultTable(multdivisor(4,P),0);
size(B[1]);
B[3];

// The direct degree slice must not retain the qring equation as a zero
// "basis element" (this occurred for 8P before reducing modulo I):
list A=globalSections(multdivisor(8,P));
size(A[1]);
size(minbase(A[1]));
int hasZero;
int j;
for (j=1;j<=size(A[1]);j++)
{
  if (reduce(A[1][j],std(ideal(0)))==0) { hasZero=1; }
}
hasZero;
B=globalSectionsMultTable(multdivisor(8,P),0);
size(B[1]);
ideal directCoordinates=A[1]*B[2];
ideal multTableCoordinates=B[1]*A[2];
size(NF(directCoordinates,std(multTableCoordinates)));
size(NF(multTableCoordinates,std(directCoordinates)));

// Reduction alone is insufficient: quotient relations can also make the
// retained degree slice linearly dependent.  On this smooth conic Q_2 has
// dimension 5.
ring S=0,(x,y,z),dp;
ideal conicEquation=x2-y2-yz;
qring conic=std(conicEquation);
divisor principalConicDivisor=makeDivisor(ideal(z2),ideal(1));
list conicDirect=globalSections(principalConicDivisor);
size(conicDirect[1]);
size(minbase(conicDirect[1]));
list conicMultTable=globalSectionsMultTable(principalConicDivisor,0);
list conicBGG=globalSectionsBGG(principalConicDivisor,0);
size(conicMultTable[1]);
size(conicBGG[1]);
ideal conicDirectCoordinates=conicDirect[1]*conicMultTable[2];
ideal conicMultTableCoordinates=conicMultTable[1]*conicDirect[2];
size(NF(conicDirectCoordinates,std(conicMultTableCoordinates)));
size(NF(conicMultTableCoordinates,std(conicDirectCoordinates)));
ideal conicBGGCoordinates=conicBGG[1]*conicMultTable[2];
ideal conicMultTableBGGCoordinates=conicMultTable[1]*conicBGG[2];
size(NF(conicBGGCoordinates,std(conicMultTableBGGCoordinates)));
size(NF(conicMultTableBGGCoordinates,std(conicBGGCoordinates)));

tst_status(1);$
