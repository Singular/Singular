LIB "tst.lib";
tst_init();

// overflow checks:
ring r;
poly p=2x300;
map f=r,x300;
f(p);
subst (p,x,y300);

tst_status(1);$
