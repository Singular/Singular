LIB "tst.lib";
tst_init();

LIB "poly.lib";

example cyclic;
killall();

example katsura;
killall();

example freerank;
killall();

example maxcoef;
killall();

example maxdeg;
killall();

example maxdeg1;
killall();

example mindeg;
killall();

example mindeg1;
killall();

example normalize;
killall();

example rad_con;
killall();

example content;
killall();

/*example numerator;// core function in SW*/ring r=0,x,dp;number n=3/2;numerator(n);kill r;

/*example denominator;// core function in SW*/ring r=0,x,dp;number n=3/2;denominator(n);kill r;

example mod2id;

example id2mod;

example subrInterred;

tst_status(1);$
