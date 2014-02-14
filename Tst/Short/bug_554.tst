LIB "tst.lib";
tst_init();

LIB("ehv.lib");

ring rng = 0,x,dp;
ideal I=0;
def rI = radEHV(I);
ASSUME(0, size(rI)==0 );


tst_status(1); $;

