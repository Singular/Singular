LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r=0,(x,y,z),dp;
ideal i=x^2+x*y^2,x*y+x^2*y,z;
ideal j=x^2+x*y^2,x*y,z;
ideal mon=x^2,z,x*y;
intvec iv=1,3;
subrInterred(mon,i,iv);
subrInterred(mon,j,iv);
tst_status(1);$
