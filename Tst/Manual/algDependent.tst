LIB "tst.lib"; tst_init();
LIB "algebra.lib";
int p = printlevel; printlevel = 1;
ring R = 0,(x,y,z,u,v,w),dp;
ideal I = xyzu2w-1yzu2w2+u4w2-1xu2vw+u2vw2+xyz-1yzw+2u2w-1xv+vw+2,
x-w, u2w+1, yz-v;
list l = algDependent(I);
l[1];
def S = l[2]; setring S;
ker;
printlevel = p;
tst_status(1);$
