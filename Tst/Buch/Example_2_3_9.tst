LIB "tst.lib";
tst_init();

ring A=0,(x,y,z),(c,dp);
module I=[x,y,1],[xy,z,z2];
vector f=[zx,y2+yz-z,y];
reduce(f,I);

reduce(f,std(I));

tst_status(1);$
