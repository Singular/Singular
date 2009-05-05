LIB "tst.lib";
tst_init();

// attribute which depend on a ring
ring r;
ideal i;
attrib(i,"a",x);
attrib(i);
attrib(i,"a");
ring rr=0,a,dp;
kill r;

tst_status(1);$
