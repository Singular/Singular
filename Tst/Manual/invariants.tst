LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring exring=0,(x,y),dp;
list Hne=develop(y4+2x3y2+x6+x5y);
list INV=invariants(Hne);
INV[1];                   // the characteristic exponents
INV[2];                   // the generators of the semigroup of values
INV[3],INV[4];            // the Puiseux pairs in packed form
INV[5] div 2;             // the delta-invariant
INV[6];                   // the sequence of multiplicities
// To display the invariants more 'nicely':
displayInvariants(Hne);
/////////////////////////////
INV=invariants((x2-y3)*(x3-y5));
INV[1][1];                // the characteristic exponents of the first branch
INV[2][6];                // the sequence of multiplicities of the second branch
print(INV[size(INV)][1]);         // the contact matrix of the branches
print(INV[size(INV)][2]);         // the intersection numbers of the branches
INV[size(INV)][3];                // the delta invariant of the curve
tst_status(1);$
