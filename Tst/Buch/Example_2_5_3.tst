LIB "tst.lib";
tst_init();

ring R=0,(x,y,z),(c,dp);
ideal I=xy,yz,xz;
module M=syz(I); 
M;

module T=[xy,1,0,0],[yz,0,1,0],[xz,0,0,1];
module N=std(T);
N; 

tst_status(1);$
