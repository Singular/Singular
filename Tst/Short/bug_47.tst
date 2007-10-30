LIB "tst.lib";
tst_init();

LIB "nctools.lib";
ring r = 0,(x,d),dp;
def S=Weyl(); setring S;
matrix E[2][2]=[-x,d],[x2,-x*d+1];
matrix T[2][2]=[d,d2],[x,x*d-1];
matrix A = E*T;
print(A);
A[2,1] - (E[2,1]*T[1,1] + E[2,2]*T[2,1]);

// Eintrag A[2,1]:
// in 3-0-0: 0 (richtig!)
// in 3-0-2: x (falsch!)
// in 3-0-3: 0 (richtig!)
// check: A[2,1] = E[2,1]*T[1,1] + E[2,2]*T[2,1];

tst_status(1);$
