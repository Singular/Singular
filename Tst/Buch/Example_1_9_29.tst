LIB "tst.lib";
tst_init();

// ==============================example 1.9.29 ===============================

ring s = 0,(p,q),dp;
def S=ncalgebra(1,q^2); setring S; // setting the relation qp = pq + q^2
ideal I = p+q, p2+q2;
eliminate(I,q);

eliminate(I,p);

tst_status(1);$

