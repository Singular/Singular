LIB "tst.lib";
tst_init();
ring R1=0,(x,y),dp;
ring R2=32003,(a,b),dp;
def R=R1+R2;
R;
tst_status(1);$
