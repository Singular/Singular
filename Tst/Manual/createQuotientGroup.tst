LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat I[3][3] =
1, 0, 0,
0, 1, 0,
0, 0, 1;
intmat L[3][2] =
1, 1,
1, 3,
1, 5;
// The group Z^3 / L can be constructed as follows:
// shortcut:
def G = createQuotientGroup(L);
printGroup(G);
// the general way:
def GG = createGroup(I, L); // (I+L)/L
printGroup(GG);
tst_status(1);$
