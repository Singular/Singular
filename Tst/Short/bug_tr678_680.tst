LIB "tst.lib";
tst_init();

LIB"primdec.lib";
ring rng = (7,v),(x,y,z,u),(dp,C);
minpoly = v^2+v+3;
ideal I = (2*v+3)*y^2*z+(2*v+1)*y^2+(v-2),(-3*v-1)*x*z^2+(v)*x^2+(-3*v-1)*z^2,(3*v)*x^3+(-v);
def L1 = primdecSY (I);
def L2 = primdecSY ( std(I) );
size(L1);
size(L2);

kill rng;
system("random",1947043918);
ring rng = (13931,v),(x,y,z,w),dp;
minpoly = v^2-13;
ideal I = 5531*x*z^2+(1016*v),(169*v)*y^2*w+2197*x,-3044*x^2*w+1362;
list L1 = primdecGTZ(I);
list L2 = primdecSY(I);

L1;
L2;

tst_status(1);$
