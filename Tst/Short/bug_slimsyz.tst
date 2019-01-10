LIB "tst.lib";
tst_init();

//rAssure_TDeg in slimgb versus ro_syz
ring r = 0, (x1), (dp(1), c);
module a1 = 2*x1*(gen(6)), gen(6)*(2-8*(x1)),
gen(1)*(x1-2*(x1^2))+gen(6)*(-2+8*(x1));
module a2 = gen(6), gen(1)*(-(x1)+2*(x1^2));
matrix T;
module result = lift(a1, a2, T, "slimgb");
result;

tst_status(1);$
