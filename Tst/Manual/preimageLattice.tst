LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat P[2][3] =
2,6,10,
4,8,12;
intmat B[2][1] =
1,
0;
// should be a (3x2)-matrix with columns e.g. [1,1,-1] and [0,3,-2] (the generated lattice should be identical)
print(preimageLattice(P,B));
// another example
intmat W[3][3] =
1,0,0,
0,1,1,
0,2,0;
intmat Z[3][2] =
1,0,
0,1,
0,0;
// should be a (3x2)-matrix with columns e.g. [1,0,0] and [0,0,-1] (the generated lattice should be identical)
print(preimageLattice(W,Z));
tst_status(1);$
