LIB "tst.lib";
tst_init();

// functions return ring-dep data whose args are not ring-dep.
var(1);
gen(1);
farey(2,3);
farey(bigint(2),3);
farey(2,bigint(3));
farey(bigint(2),bigint(3));
freemodule(3);
maxideal(2);
monomial(1);
monomial(intvec(1));
par(2);
resolution(list());
ring r=0,(x,y,z),dp;
var(1);
gen(1);
farey(2,3);
farey(bigint(2),3);
farey(2,bigint(3));
farey(bigint(2),bigint(3));
freemodule(3);
maxideal(2);
monomial(1);
monomial(intvec(1));
par(2);
resolution(list());

tst_status(1);$
