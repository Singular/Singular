LIB "tst.lib"; tst_init();
LIB "gmspoly.lib";
ring R=0,(x,y,z),dp;
poly f=x+y+z+x2y2z2;
def Rs=goodBasis(f);
setring(Rs);
b;
print(jet(A,0));
print(jet(A/var(1),0));
tst_status(1);$
