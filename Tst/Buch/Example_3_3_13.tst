LIB "tst.lib";
tst_init();

ring A=0,(u,v,w,x,y,z),dp;
ideal I=wx,wy,wz,vx,vy,vz,ux,uy,uz,y3-x2;
I=std(I);
dim(I);

tst_status(1);$
