LIB "tst.lib";
tst_init();

ring R=0,(x,y,z),(c,dp);
module I=[xy,xz],[yz,xy];
module J=[y,z],[z,y];
ideal K=quotient(I,J);
K;

reduce(K*J,std(I));  

qring S=std(K);
module Null;
module J=[xy,xyz-x2y2],[xy2,y2x];
ideal Ann=quotient(Null,J); 
Ann;

tst_status(1);$
