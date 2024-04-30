LIB "tst.lib";
tst_init();
LIB "gitfan.lib";

  intmat Q[5][10] =
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 1, 1, 0, 0, 0,
    0, 1, 1, 0, 0, 0, -1, 1, 0, 0,
    0, 1, 0, 1, 0, -1, 0, 0, 1, 0,
    0, 0, 1, 1, -1, 0, 0, 0, 0, 1;
  cone mov = movingCone(Q);
  mov;
  rays(mov);


  intmat Q[3][4] =
    1,0,1,0,
    0,1,0,1,
    0,0,1,1;
  GKZfan(Q);

  ring R = 0,T(1..10),wp(1,1,1,1,1,1,1,1,1,1);
  ideal J =
    T(5)*T(10)-T(6)*T(9)+T(7)*T(8),
    T(1)*T(9)-T(2)*T(7)+T(4)*T(5),
    T(1)*T(8)-T(2)*T(6)+T(3)*T(5),
    T(1)*T(10)-T(3)*T(7)+T(4)*T(6),
    T(2)*T(10)-T(3)*T(9)+T(4)*T(8);
  intmat Q[5][10] =
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 1, 1, 0, 0, 0,
    0, 1, 1, 0, 0, 0, -1, 1, 0, 0,
    0, 1, 0, 1, 0, -1, 0, 0, 1, 0,
    0, 0, 1, 1, -1, 0, 0, 0, 0, 1;
  list simplexSymmetryGroup = G25Action();
  list simplexOrbitRepresentatives = intvec( 1, 2, 3, 4, 5 ),
  intvec( 1, 2, 3, 5, 6 ),
  intvec( 1, 2, 3, 5, 7 ),
  intvec( 1, 2, 3, 5, 10 ),
  intvec( 1, 2, 3, 7, 9 ),
  intvec( 1, 2, 6, 9, 10 ),
  intvec( 1, 2, 3, 4, 5, 6 ),
  intvec( 1, 2, 3, 4, 5, 10 ),
  intvec( 1, 2, 3, 5, 6, 8 ),
  intvec( 1, 2, 3, 5, 6, 9 ),
  intvec( 1, 2, 3, 5, 7, 10 ),
  intvec( 1, 2, 3, 7, 9, 10 ),
  intvec( 1, 2, 3, 4, 5, 6, 7 ),
  intvec( 1, 2, 3, 4, 5, 6, 8 ),
  intvec( 1, 2, 3, 4, 5, 6, 9 ),
  intvec( 1, 2, 3, 5, 6, 9, 10 ),
  intvec( 1, 2, 3, 4, 5, 6, 7, 8 ),
  intvec( 1, 2, 3, 4, 5, 6, 9, 10 ),
  intvec( 1, 2, 3, 4, 5, 6, 7, 8, 9 ),
  intvec( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 );
  list afaceOrbitRepresentatives=afaces(J,simplexOrbitRepresentatives);
  list fulldimAfaceOrbitRepresentatives=fullDimImages(afaceOrbitRepresentatives,Q);
  list afaceOrbits=computeAfaceOrbits(fulldimAfaceOrbitRepresentatives,simplexSymmetryGroup);
  apply(afaceOrbits,size);
  list minAfaceOrbits = minimalAfaceOrbits(afaceOrbits);
  apply(minAfaceOrbits,size);
  list listOfOrbitConeOrbits = orbitConeOrbits(minAfaceOrbits,Q);
  apply(listOfOrbitConeOrbits,size);
  list listOfMinimalOrbitConeOrbits = minimalOrbitConeOrbits(listOfOrbitConeOrbits);
  size(listOfMinimalOrbitConeOrbits);
  list Asigma = groupActionOnQImage(simplexSymmetryGroup,Q);
  list actionOnOrbitconeIndices = groupActionOnHashes(Asigma,listOfOrbitConeOrbits);
  list OClist = listOfOrbitConeOrbits[1];
  for (int i =2;i<=size(listOfOrbitConeOrbits);i++){
    OClist = OClist + listOfOrbitConeOrbits[i];
  }
  cone mov = coneViaPoints(transpose(Q));
  mov = canonicalizeCone(mov);
  list Sigma = GITfanParallelSymmetric(OClist, Q, mov, actionOnOrbitconeIndices);
  Sigma;

  ring R = 0,T(1..10),wp(1,1,1,1,1,1,1,1,1,1);
  ideal J =
    T(5)*T(10)-T(6)*T(9)+T(7)*T(8),
    T(1)*T(9)-T(2)*T(7)+T(4)*T(5),
    T(1)*T(8)-T(2)*T(6)+T(3)*T(5),
    T(1)*T(10)-T(3)*T(7)+T(4)*T(6),
    T(2)*T(10)-T(3)*T(9)+T(4)*T(8);
  intmat Q[5][10] =
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 1, 1, 0, 0, 0,
    0, 1, 1, 0, 0, 0, -1, 1, 0, 0,
    0, 1, 0, 1, 0, -1, 0, 0, 1, 0,
    0, 0, 1, 1, -1, 0, 0, 0, 0, 1;
  list AF= afaces(J);
  list OC = orbitCones(AF,Q);
  list generatorsG = permutationFromIntvec(intvec( 1, 3, 2, 4, 6, 5, 7, 8, 10, 9 )),
                     permutationFromIntvec(intvec( 5, 7, 1, 6, 9, 2, 8, 4, 10, 3 ));
  list Asigmagens = groupActionOnQImage(generatorsG,Q);
//list actionOnOrbitconeIndicesForGenerators = groupActionOnHashes(Asigmagens,OC);
string elementInTermsOfGenerators =
"(x2^-1*x1^-1)^3*x1^-1";
//evaluateProduct(actionOnOrbitconeIndicesForGenerators, elementInTermsOfGenerators);

tst_status(1);$
