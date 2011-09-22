LIB "tst.lib"; tst_init();
LIB "ainvar.lib";
ring q=0,(x,y,z),dp;
matrix m[3][1];
m[2,1]=x;
m[3,1]=y;
poly in=localInvar(m,z,y,x);
in;
tst_status(1);$
