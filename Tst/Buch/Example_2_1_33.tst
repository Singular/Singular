LIB "tst.lib";
tst_init();

ring A=0,(x,y,z),(c,ds);
module M=[0,xy-1,xy+1],[y,xz,xz];

prune(M);

tst_status(1);$
