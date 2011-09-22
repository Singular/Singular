LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring exring=0,(x,y,t),ds;
poly f=x3+2xy2+y2;
list Hne=develop(f);
list hne_extended=extdevelop(Hne,10);
//   compare the HNE matrices ...
print(Hne[1]);
print(hne_extended[1]);
// ... and the resulting parametrizations:
param(Hne);
param(hne_extended);
param(hne_extended,0);
// An example with more than one branch:
list L=hnexpansion(f*(x2+y4));
def HNring = L[1]; setring HNring;
param(hne);
tst_status(1);$
