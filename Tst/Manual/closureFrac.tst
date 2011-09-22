LIB "tst.lib"; tst_init();
LIB "normal.lib";
ring R=0,(x,y),dp;
ideal ker=x2+y2;
export ker;
list L=primeClosure(R);          // We normalize R/ker
for (int i=1;i<=size(L);i++) { def R(i)=L[i]; }
setring R(2);
kill R;
phi;                             // The map R(1)-->R(2)
poly f=T(2);                     // We will get a representation of f
export f;
L[2]=R(2);
closureFrac(L);
setring R(1);
kill R(2);
fraction;                        // f=fraction[1]/fraction[2] via phi
kill R(1);
tst_status(1);$
