LIB "tst.lib"; tst_init();
LIB "ainvar.lib";
ring q=0,(x,y,z,u,v,w),dp;
poly f=2xz-y2;
matrix m[6][1] =x,y,0,u,v;
derivate(m,f);
vector v = [2xz-y2,u6-3];
derivate(m,v);
derivate(m,ideal(2xz-y2,u6-3));
tst_status(1);$
