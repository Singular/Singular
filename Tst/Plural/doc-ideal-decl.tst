LIB "tst.lib";
tst_init();
ring r=0,(x,y,z),dp;
def S=nc_algebra(-1,0);setring S;
poly s1 = x2;
poly s2 = y3;
poly s3 = z;
ideal i =  s1, s2-s1, 0,s2*s3, s3^4;
i;
size(i);
ncols(i);
tst_status(1);$       
