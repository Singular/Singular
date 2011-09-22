LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring r = 0, (x, y, z), dp;
intmat S1[3][3] =
1, 0, 0,
0, 1, 0,
0, 0, 1;
intmat L1[3][1] =
0,
0,
0;
def G1 = createGroup(S1, L1); // (S1 + L1)/L1
printGroup(G1);
setBaseMultigrading(S1, L1); // to change...
ring R = 0, (a, b, c), dp;
intmat S2[2][3] =
1, 0,
0, 1;
intmat L2[2][1] =
0,
2;
def G2 = createGroup(S2, L2);
printGroup(G2);
setBaseMultigrading(S2, L2); // to change...
map F = r, a, b, c;
intmat A[nrows(L2)][nrows(L1)] =
1, 0, 0,
3, 2, -6;
// graded ring homomorphism is given by (compatible):
print(F);
print(A);
isGradedRingHomomorphism(r, ideal(F), A);
def h = createGradedRingHomomorphism(r, ideal(F), A);
print(h);
// not a homo..
intmat B[nrows(L2)][nrows(L1)] =
1, 1, 1,
0, 0, 0;
print(B);
isGradedRingHomomorphism(r, ideal(F), B); // FALSE: there is no such homomorphism!
// Therefore: the following command should return an error
// createGradedRingHomomorphism(r, ideal(F), B);
tst_status(1);$
