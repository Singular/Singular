LIB "tst.lib"; tst_init();
LIB "alexpoly.lib";
ring r=0,(x,y),ls;
// Irreducible Case
semigroup((x2-y3)^2-4x5y-x7);
// In the irreducible case, invariants() also calculates a minimal set of
// generators of the semigroup.
invariants((x2-y3)^2-4x5y-x7)[1][2];
// Reducible Case
poly f=(y2-x3)*(y2+x3)*(y4-2x3y2-4x5y+x6-x7);
semigroup(f);
tst_status(1);$
