LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),(dp(1),dp(2));
module M = [-1,x,y],[-7,y,y],[3,x,x];
module N = [1,x,y,x,y],[-2,y,x,y,x],[6,x,y,y,x,y];
list L; L[1] = M; L[2] = N;
lst2str(L);
lst2str(L[1],1);
tst_status(1);$
