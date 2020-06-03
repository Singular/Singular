LIB "tst.lib"; tst_init();
LIB "ring.lib";
def r=defring("0",5,"u","ls"); r; setring r;"";
def R=defring("(2,A)",10,"x(","(dp(3),ws(1,2,3),ds)"); R; setring R;
kill R,r;
tst_status(1);$
