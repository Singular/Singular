LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring R = 37,(u,v,w,x,y,z),dp;
ideal I = u2v-xyz, ux2-wyz, uvw-y2z;
def S = normalToricRingFromBinomials(I);
setring S;
I;
tst_status(1);$
