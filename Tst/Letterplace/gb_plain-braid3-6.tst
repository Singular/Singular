LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x_1,y_1,z_1,x_2,y_2,z_2,x_3,y_3,z_3,x_4,y_4,z_4,x_5,y_5,z_5,x_6,y_6,z_6),Dp;
int upToDeg = 6;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = y_1*x_2*y_3-z_1*y_2*z_3,
x_1*y_2*x_3-z_1*x_2*y_3,
-y_1*z_2*x_3+z_1*x_2*z_3,
x_1*x_2*x_3+x_1*y_2*z_3+y_1*y_2*y_3+z_1*z_2*z_3,
y_2*x_3*y_4-z_2*y_3*z_4,
y_3*x_4*y_5-z_3*y_4*z_5,
y_4*x_5*y_6-z_4*y_5*z_6,
x_2*y_3*x_4-z_2*x_3*y_4,
x_3*y_4*x_5-z_3*x_4*y_5,
x_4*y_5*x_6-z_4*x_5*y_6,
-y_2*z_3*x_4+z_2*x_3*z_4,
-y_3*z_4*x_5+z_3*x_4*z_5,
-y_4*z_5*x_6+z_4*x_5*z_6,
x_2*x_3*x_4+x_2*y_3*z_4+y_2*y_3*y_4+z_2*z_3*z_4,
x_3*x_4*x_5+x_3*y_4*z_5+y_3*y_4*y_5+z_3*z_4*z_5,
x_4*x_5*x_6+x_4*y_5*z_6+y_4*y_5*y_6+z_4*z_5*z_6;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
