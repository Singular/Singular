LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),(dp);
module M = [1,x,y,x,y],[-2,y,x,y,x],[6,x,y,y,x,y];
mod2str(M);
mod2str(M,1);
tst_status(1);$
