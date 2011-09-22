LIB "tst.lib"; tst_init();
LIB "cisimplicial.lib";
int a = 95;
intvec v = 18,51,13;
oneDimBelongSemigroup(a,v);
"// 95 = 1*18 + 1*25 + 2*13";
oneDimBelongSemigroup(a,v,2);
"// 95 is not a combination of 18 and 52;";
tst_status(1);$
