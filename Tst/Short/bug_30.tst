LIB "tst.lib";
tst_init();

ring r=0,(x,y,z,t(1..8)),(c,ws(-1,-1,-1,100,100,100,100,100,100,100,100));
poly p=
z*t(7)^2*t(8)^2-1/27*x*z*t(6)*t(7)^2*t(8)-1/27*y*z*t(7)^3*t(8)
-1/27*z*t(4)*t(7)^2*t(8);
coef(p,xyz);

tst_status(1);$
