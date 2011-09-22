LIB "tst.lib"; tst_init();
LIB "mprimdec.lib";
ring r=0,(x,y,z),dp;
ideal i=(x2y,xz2,y2z,z3);
list l=minAssGTZ(i);
list sepL=separator(l);
sepL;
tst_status(1);$
