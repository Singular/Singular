LIB "tst.lib"; tst_init();
LIB "rootsur.lib";
ring r = 0,x,dp;
isparam(2x3-56x+2);
ring s = (0,a,b,c),x,dp;
isparam(2x3-56x+2);
isparam(2x3-56x+abc);
tst_status(1);$
