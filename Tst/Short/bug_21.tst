LIB "tst.lib";
tst_init();

ring r1=0,x,(c,dp);
ring r2=0,y,(c,dp);
def r=r1+r2;
r;

tst_status(1);$
