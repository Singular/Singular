LIB "tst.lib";
tst_init();
option(prot);

ring s=32003,(x,y,z,u),ds;
int a=7;
poly f =xyzu*(x+y+z+u) +(x+y+z+u)^2 +x^a+y^a+z^a+u^a;
ideal i= jacob(f);
ideal j=std( i);
tst_status(1);$


