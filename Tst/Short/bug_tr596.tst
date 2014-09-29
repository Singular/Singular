LIB "tst.lib";
tst_init();

LIB("normal.lib");

ring rng = 5,(x,y),dp;

ideal I = 0;

normalP(I);
def result = normalP(I,"withRing");

def r = result[1][1];
setring r;
normap;
norid;

setring rng;

I = x, x+1;

normalP(I);
result = normalP(I,"withRing");
r = result[1][1];
setring r;
normap;
norid;

//test that redefine warnings are gone:
proc testRedefineInNormalP()
{
    ring rng = (3),(xx,yy,zz),(dp(2),dp(1),C);
    ideal J = xx^2*yy*zz-yy^2*zz^2;
    intvec v=1,1,1;
    ideal component = xx^2-yy*zz;
    def L1 = normalP(component,"withRing");
    intvec w = norTest(component, L1);
    ASSUME(0, v == norTest(component, L1) );
}

testRedefineInNormalP();


tst_status(1);$
