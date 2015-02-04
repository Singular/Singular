LIB "tst.lib";
tst_init();

// K in Primdec::algeDeco has at pos 1 the minpoly or 0 or 1
// and the rest in K[2..]. In exceptional cases K[2..} do not exist
LIB"primdec.lib";

ring r1=(0,a),(x,y,z),dp;
ideal i1=1;
ideal i2=xy,z3;
minAssGTZ(i1);
minAssGTZ(i2);

ring r2=(0,a),(x,y,z),dp;
minpoly=a2-2;
ideal i1=1;
ideal i2=xy,z3;
minAssGTZ(i1);
minAssGTZ(i2);

tst_status(1);$
