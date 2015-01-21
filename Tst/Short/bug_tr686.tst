LIB "tst.lib";
tst_init();

// incorrect radical entry in a primdecGTZ() decomposition (686)
LIB"primdec.lib";
system("random",1904600441);
ring rng = (3,v),(x,y,z,w),(lp,C);
minpoly = (v^2-v-1);
ideal I = (v-1)*x^2*y+(-v-1)*y^2,v*x^3+(v-1)*x-v,(-v-1)*x^2*y+(-v-1)*z^3+(v-1);
list L= primdecGTZ (I);

list C2 = L[2];
idealsEqual( C2[2], radical (C2[2]) );

tst_status(1);$

