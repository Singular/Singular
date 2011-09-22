LIB "tst.lib"; tst_init();
LIB "sing.lib";
int p      = printlevel;
printlevel = 1;
ring  r    = 32003,(x,y),(c,dp);
ideal j    = x6-y4,x6y6,x2y4-x5y2;
module T   = T_2(j);
vdim(T);
hilb(T);"";
ring r1    = 0,(x,y,z),dp;
ideal id   = xy,xz,yz;
list L     = T_2(id,"");
vdim(L[1]);                           // vdim of T_2
print(L[3]);                          // syzygy module of id
printlevel = p;
tst_status(1);$
