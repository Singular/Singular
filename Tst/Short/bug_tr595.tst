LIB "tst.lib";
tst_init();

LIB("normal.lib");

// to suppress printed message by normalP add one indirection level:
proc normalPW(ideal id, list #) { return (Normal::normalP(id,#) ); }

ring rng = 7,(x,y), dp;
ideal I = y-3,x-3;
def result = normalPW( I ,"withRing");
def r = result[1][1];
setring r;
norid;
normap;

tst_status(1); $
