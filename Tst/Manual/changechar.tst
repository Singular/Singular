LIB "tst.lib"; tst_init();
LIB "ring.lib";
ring r=0,(x,y,u,v),(dp(2),ds);
def R=changechar("2,A"); R;"";
def R1=changechar("32003",R); setring R1; R1;
kill R,R1;
tst_status(1);$
