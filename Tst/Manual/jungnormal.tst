LIB "tst.lib"; tst_init();
LIB "resjung.lib";
//Computing a resolution of singularities of the variety z2-x3-y3
ring r = 0,(x,y,z),dp;
ideal I = z2-x3-y3;
//The ideal is in noether position
list l = jungnormal(I,1);
def R1 = l[1];
def R2 = l[2];
setring R1;
QIdeal;
BMap;
setring R2;
QIdeal;
BMap;
tst_status(1);$
