LIB "tst.lib";
tst_init();
ring r=0,(x,y,z),dp;
ncalgebra(-1,0);
poly s1 = x2;
poly s2 = y3;
poly s3 = z;
ideal i =  s1, s2-s1, 0,s2*s3, s3^4;
i;
size(i);
ncols(i);
tst_status(1);$       
