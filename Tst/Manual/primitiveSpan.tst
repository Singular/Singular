LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat V[3][2] =
1,4,
2,5,
3,6;
// should return a (3x2)-matrix whose columns
// generate the same lattice as [1, 2, 3] and [0, 1, 2]
intmat R = primitiveSpan(V);
print(R);
// another example
intmat W[2][2] =
1,0,
0,1;
// should return a (2x2)-matrix whose columns
// generate the same lattice as [1, 0] and [0, 1]
intmat S = primitiveSpan(W);
print(S);
kill V, R, S, W;
tst_status(1);$
