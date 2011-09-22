LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring r=0,(x,y),dp;
// First, an example which requires no field extension:
list Hne=hnexpansion(x4-y6);
size(Hne);           // number of branches
displayHNE(Hne);     // HN expansion of branches
param(Hne[1]);       // parametrization of 1st branch
param(Hne[2]);       // parametrization of 2nd branch
// An example which requires a field extension:
list L=hnexpansion((x4-y6)*(y2+x4));
def R=L[1]; setring R; displayHNE(hne);
basering;
setring r; kill R;
// Computing only one representative per conjugacy class:
L=hnexpansion((x4-y6)*(y2+x4),"ess");
def R=L[1]; setring R; displayHNE(hne);
L[2];     // number of branches in respective conjugacy classes
tst_status(1);$
