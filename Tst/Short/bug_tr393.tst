LIB "tst.lib";
tst_init();

// overflow checks:
ring r;
poly p=2x3000;
map f=r,x3000;
f(p);
subst (p,x,y3000);

tst_status(1);$
