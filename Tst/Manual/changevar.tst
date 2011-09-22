LIB "tst.lib"; tst_init();
LIB "ring.lib";
ring r=0,(x,y,u,v),(dp(2),ds);
ideal i = x^2,y^2-u^3,v;
qring Q = std(i);
setring(r);
def R=changevar("A()"); R; "";
def Q'=changevar("a,b,c,d",Q); setring Q'; Q';
kill R,Q,Q';
tst_status(1);$
