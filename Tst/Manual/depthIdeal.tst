LIB "tst.lib"; tst_init();
LIB "mregular.lib";
ring r=0,(x,y,z,t,w),dp;
ideal i=y2t,x2y-x2z+yt2,x2y2,xyztw,x3z2,y5+xz3w-x2zw2,x7-yt2w4;
depthIdeal(i);
depthIdeal(lead(std(i)));
// Additional information is displayed if you change printlevel (=1);
tst_status(1);$
