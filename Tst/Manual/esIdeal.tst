LIB "tst.lib"; tst_init();
LIB "equising.lib";
ring r=0,(x,y),ds;
poly f=x7+y7+(x-y)^2*x2y2;
list K=esIdeal(f);
option(redSB);
// Wahl's equisingularity ideal:
std(K[1]);
ring rr=0,(x,y),ds;
poly f=x4+4x3y+6x2y2+4xy3+y4+2x2y15+4xy16+2y17+xy23+y24+y30+y31;
list K=esIdeal(f);
vdim(std(K[1]));
// the latter should be equal to:
tau_es(f);
tst_status(1);$
