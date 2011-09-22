LIB "tst.lib"; tst_init();
LIB "general.lib";
number_pi(11);"";
ring r = (real,10),t,dp;
number pi = number_pi(11); pi;
tst_status(1);$
