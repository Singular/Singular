LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring R = 0, (x, y, z), dp;
// Weights of variables
intmat M[3][3] =
1, 0, 0,
0, 1, 0,
0, 0, 1;
// Torsion:
intmat L[3][2] =
1, 1,
1, 3,
1, 5;
// attaches M & L to R (==basering):
setBaseMultigrading(M, L); // Grading: Z^3/L
def G = getGradingGroup();
printGroup( G );
G[1] == M; G[2] == L;
kill L, M, G;
// ----------- isomorphic multigrading -------- //
// Weights of variables
intmat M[2][3] =
1, -2, 1,
1,  1, 0;
// Torsion:
intmat L[2][1] =
0,
2;
// attaches M & L to R (==basering):
setBaseMultigrading(M, L); // Grading: Z + (Z/2Z)
def G = getGradingGroup();
printGroup( G );
G[1] == M; G[2] == L;
kill L, M, G;
// ----------- extreme case ------------ //
// Weights of variables
intmat M[1][3] =
1,  -1, 10;
// Torsion:
intmat L[1][1] =
0;
// attaches M & L to R (==basering):
setBaseMultigrading(M); // Grading: Z^3
def G = getGradingGroup();
printGroup( G );
G[1] == M; G[2] == L;
kill L, M, G;
tst_status(1);$
