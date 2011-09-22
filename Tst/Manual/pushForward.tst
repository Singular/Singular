LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring r = 0,(x,y,z),dp;
// Setting degrees for preimage ring.;
intmat grad[3][3] =
1,0,0,
0,1,0,
0,0,1;
setBaseMultigrading(grad);
// grading on r:
getVariableWeights();
getLattice();
// only for the purpose of this example
if( voice > 1 ){ /*keepring(r);*/ export(r); }
ring R = 0,(a,b),dp;
ideal i = a2-b2+a6-b5+ab3,a7b+b15-ab6+a6b6;
// The quotient ring by this ideal will become our image ring.;
qring Q = std(i);
listvar();
map f = r,-a2b6+b5+a3b+a2+ab,-a2b7-3a2b5+b4+a,a6-b6-b3+a2; f;
// TODO: Unfortunately this is not a very spectacular example...:
// Pushing forward f:
pushForward(f);
// due to pushForward we have got new grading on Q
getVariableWeights();
getLattice();
// only for the purpose of this example
if( voice > 1 ){ kill r; }
tst_status(1);$
