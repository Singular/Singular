LIB "tst.lib";
tst_init();

// crash in primdecSY (remains of a temporary filed extensions)
LIB"primdec.lib";
ring rng = (7,v),(x,y,z,u),(dp(4),C);
minpoly = (v^2+v+3);
ideal I =
(-v)*x^2*y+(2*v-2),(-3*v)*x*u+(v-2)*z,(v+1)*y^2*u+(3*v-3)*x*u+2*u;

minAssGTZ (I);

tst_status(1);$
