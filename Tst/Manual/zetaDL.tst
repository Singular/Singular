LIB "tst.lib"; tst_init();
LIB "reszeta.lib";
ring R=0,(x,y,z),dp;
ideal I=x2+y2+z3;
list re=resolve(I,"K");
zetaDL(re,1);
I=(xz+y2)*(xz+y2+x2)+z5;
list L=resolve(I,"K");
zetaDL(L,1);
//===== expected zeta function =========
// (20s^2+130s+87)/((1+s)*(3+4s)*(29+40s))
//======================================
tst_status(1);$
