LIB "tst.lib";
tst_init();

ring R=0,(x,y,z),(c,dp);
module M=[-z,-y,x+y,x],[yz+z2,yz+z2,-xy-y2-xz-z2];
vector v=[-xz-z2,-xz+z2,x2+xy-yz+z2];
reduce(v,std(M));

v=M[1]-x5*M[2];
v;

reduce(v,std(M));

tst_status(1);$
