LIB "tst.lib";
tst_init();

// overflow in hilbert_series (ring with 1 var had limit 2^16)
ring r=0,(w,x,y,z,u,v),dp;
int n=10923;
ideal i=w^n,x^n,y^n,z^n,u^n,v^n;
hilb(std(i),1);

// special case hilb(ideal(1),1) PR #1233
ideal I=1;
hilb(std(I),1);

tst_status(1);$
