LIB "tst.lib";
tst_init();

ring R=0,(x,y,z),(c,dp);
module T=[xy,1,0,0],[yz,0,1,0],[xz,0,0,1];
module N=std(T);
N; 

tst_status(1);$
