LIB "tst.lib"; tst_init();
LIB "sing.lib";
ring r   = 32003,(x,y,z),ds;
ideal i  = xy,xz,yz;
matrix T = deform(i);
print(T);
print(deform(x3+y5+z2));
tst_status(1);$
