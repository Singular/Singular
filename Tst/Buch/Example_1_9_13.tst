LIB "tst.lib";
tst_init();

// ======================== example 1.9.3 =====================================
ring r = 0,(e,f,h),dp;
matrix C[3][3];
C[1,2] = 1; C[1,3] = 1; C[2,3] = 1; 
matrix D[3][3];
D[1,2] = -h; 
D[1,3] = 2e; 
D[2,3] = -2f;
def S=ncalgebra(C,D); setring S;
S;
fe; // "commutative" syntax, not correct 
f*e; // correct non-commutative syntax

tst_status(1);$

