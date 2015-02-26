LIB "tst.lib";
tst_init();

// test for fix of trac bug 657 with error
// zz_pX InvMod: can't compute multiplicative inverse

ring rng =(7),(a,x,y,z,w),dp;

ideal PS =
-2*a*y*z^2-2*y*z^2+2*a*z-3*z-1,
-2*x*w^2-3*a*x+x,
-a*y^3+y^3+2*a-2,
a^2+a+3;

char_series(PS);

tst_status(1);$
