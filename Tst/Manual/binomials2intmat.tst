LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring S = 37,(u,v,w,x,y,z),dp;
ideal I = u2v-xyz, ux2-vyz, uvw-y2z;
binomials2intmat(I);
tst_status(1);$
