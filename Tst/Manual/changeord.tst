LIB "tst.lib"; tst_init();
LIB "ring.lib";
ring r=0,(x,y,u,v),(dp(2),ds);
def R=changeord("wp(2,3),dp"); R; "";
ideal i = x^2,y^2-u^3,v;
qring Q = std(i);
def Q'=changeord("lp",Q); setring Q'; Q';
kill R,Q,Q';
tst_status(1);$
