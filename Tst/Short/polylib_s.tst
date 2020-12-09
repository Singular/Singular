LIB "tst.lib";
tst_init();

LIB "polylib.lib";

example cyclic;

example katsura;

example freerank;

example maxcoef;

example maxdeg;

example maxdeg1;

example mindeg;

example mindeg1;

example normalize;

example rad_con;

example content;

/*example numerator;// core function in SW*/ring r=0,x,dp;number n=3/2;numerator(n);kill r;

/*example denominator;// core function in SW*/ring r=0,x,dp;number n=3/2;denominator(n);kill r;

example mod2id;

example id2mod;

example subrInterred;

tst_status(1);$
