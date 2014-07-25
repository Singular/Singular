LIB "tst.lib";
tst_init();

// overflow in subst

ring rng = 0,(x,y,z,aa),(L(2^16-1),dp); // to make it indep of the machine
ideal I = x^10000;

I = subst (I,x,x^ 10000);

poly f = x^10000;
f = subst (f,x,x^ 10000);

x^100000;

tst_status(1);$
