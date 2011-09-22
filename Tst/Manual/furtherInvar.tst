LIB "tst.lib"; tst_init();
LIB "ainvar.lib";
ring r=0,(x,y,z,u),dp;
matrix m[4][1];
m[2,1]=x;
m[3,1]=y;
m[4,1]=z;
ideal id=localInvar(m,z,y,x),localInvar(m,u,y,x);
ideal karl=id,x;
list in=furtherInvar(m,id,karl,x);
in;
tst_status(1);$
