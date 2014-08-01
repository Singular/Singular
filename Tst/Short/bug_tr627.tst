LIB "tst.lib";
tst_init();

ring r = (0,a), x, dp;
minpoly = a2+1;
poly p = x;
subst(p,a,x);
subst(p,x,a);
       
p = x+a;
subst(p,a,x);
subst(p,x,a);

subst(minpoly,a,x);

ring r2 = (0,a), x, dp;
poly p = x;
subst(p,a,x);
subst(p,x,a);
       
p = x+a;
subst(p,a,x);
subst(p,x,a);

tst_status(1);$
