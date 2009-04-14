LIB "tst.lib";
tst_init();

LIB "normal.lib";
ring R =32003,(x,y,z),dp;
ideal I=zy2-zx3-x6;
list pr=normalC(I);
def S  =pr[1][1];       
setring S;         
norid;

ring A =0,(x,y,z),dp;
ideal I=z2+yx2;
LIB"surf.lib";
//plot(I);

list nor=normalC (I);
def R   =nor[1][1]; setring R;
norid; normap;

tst_status(1);$
