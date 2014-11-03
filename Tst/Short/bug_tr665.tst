LIB "tst.lib";
tst_init();

// wrong range in ideal/module (missed special case in primaryTest)
LIB"primdec.lib";
ring rng = (7,v),(x,y,z,u),(dp(4),C);
minpoly = (v^2+v+3);
ideal I =
(-v)*x^2*y+(2*v-2),(-3*v)*x*u+(v-2)*z,(v+1)*y^2*u+(3*v-3)*x*u+2*u;

minAssGTZ (I); // should not fail

tst_status(1);$
