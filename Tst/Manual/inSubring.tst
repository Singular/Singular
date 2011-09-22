LIB "tst.lib"; tst_init();
LIB "algebra.lib";
ring q=0,(x,y,z,u,v,w),dp;
poly p=xyzu2w-1yzu2w2+u4w2-1xu2vw+u2vw2+xyz-1yzw+2u2w-1xv+vw+2;
ideal I =x-w,u2w+1,yz-v;
inSubring(p,I);
tst_status(1);$
