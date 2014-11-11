LIB "tst.lib";
tst_init();

// crash in primdecSY (remains of a temporary filed extensions)
LIB"primdec.lib";
ring rng = (7,v),(z,t,p,y),(dp(4),C);
minpoly = (v^2+v+3);
ideal I = (2*v+3)*t*p^2+(-3*v-3)*z*t+(v-1)*p*y,
(-3*v-1)*p*y^2+(2*v+2)*t^2,
(3*v-2)*z^2*t+(v-2)*y;
primdecSY (I);

tst_status(1);$
