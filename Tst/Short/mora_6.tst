LIB "tst.lib";
tst_init();
option(prot);
ring r=32003,(w,x,y,z),ds;
int a=7;
poly f =xyz*(x+y+z)^2 +(x+y+z)^3 +x^a+y^a+z^a;
ideal i= jacob(f);
i=i,f;
ideal j=std( i);
tst_status(1);$


