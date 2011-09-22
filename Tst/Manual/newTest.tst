LIB "tst.lib"; tst_init();
LIB "atkins.lib";
ring r = 0,x,dp;
list L=8976,-223456,556,-778,3,-55603,45,766677;
number D=-55603;
newTest(L,D);
tst_status(1);$
