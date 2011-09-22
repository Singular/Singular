LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = (0,a),(x,y3,z(1)),dp;
vector v = [-7,x,y3^4,x2,z(1)^3];
vct2str(v);
vct2str(v,1);
vector w = [-7a^5+6a,x,y3,y3,x,z(1),z(1)];
vct2str(w);
vct2str(w,1);
tst_status(1);$
