LIB "tst.lib";
tst_init();

LIB"primdec.lib";
ring B=0,(x,y,z),dp;
ideal PB=x,z2-xz-1;
list pr=minAssGTZ(PB);
pr;

tst_status(1);$
