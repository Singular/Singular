LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring r=0,(x,y,z),ls;
spoly(2x2+x2y,3y3+xyz);
ring s=0,(x,y,z),(c,dp);
spoly(2x2+x2y,3y3+xyz);
spoly(2x2+x2y,3y3+xyz,1);             //symmetric s-poly without division
spoly([5x2+x2y,z5],[x2,y3,y4]);       //s-poly for vectors
tst_status(1);$
