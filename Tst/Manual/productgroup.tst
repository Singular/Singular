LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat S1[2][2] =
1,0,
0,1;
intmat L1[2][1] =
2,
0;
intmat S2[2][2] =
1,0,
0,2;
intmat L2[2][1] =
0,
3;
list G = createGroup(S1,L1);
list H = createGroup(S2,L2);
list N = productgroup(G,H);
print(N);
kill G,H,N,S1,L1,S2,L2;
tst_status(1);$
