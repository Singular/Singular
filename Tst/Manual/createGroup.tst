LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat S[3][3] =
1, 0, 0,
0, 1, 0,
0, 0, 1;
intmat L[3][2] =
1, 1,
1, 3,
1, 5;
def G = createGroup(S, L); // (S+L)/L
printGroup(G);
kill S, L, G;
/////////////////////////////////////////////////
intmat S[2][3] =
1, -2, 1,
1,  1, 0;
intmat L[2][1] =
0,
2;
def G = createGroup(S, L); // (S+L)/L
printGroup(G);
kill S, L, G;
// ----------- extreme case ------------ //
intmat S[1][3] =
1,  -1, 10;
// Torsion:
intmat L[1][1] =
0;
def G = createGroup(S, L); // (S+L)/L
printGroup(G);
tst_status(1);$
