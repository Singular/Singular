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
isGroup(G);
printGroup(G);
tst_status(1);$
