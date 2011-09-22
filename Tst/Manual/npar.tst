LIB "tst.lib"; tst_init();
LIB "findifs.lib";
ring r = (0,dh,dt,theta,A),t,dp;
npar(dh);
number T = theta;
npar(T);
npar(dh^2);
tst_status(1);$
