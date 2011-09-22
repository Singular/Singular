LIB "tst.lib"; tst_init();
LIB "sing.lib";
ring r   =32003,(x,y,z),ds;
ideal i  = yx3+y,yz3+y3z;
poly f   = x;
is_active(f,i);
qring q  = std(x4y5);
poly f   = x;
module m = [yx3+x,yx3+y3x];
is_active(f,m);
tst_status(1);$
