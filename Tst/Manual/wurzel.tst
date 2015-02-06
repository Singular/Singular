LIB "tst.lib";
tst_init();

LIB "aksaka.lib";
ring R = 0,x,dp;
def T=wurzel(7629412809180100);
// T is only an approximation, depending on 32/64bit
// and the implementation of nlMapLongR
// check the approx. correctness approx.:
ring RR=real,x,lp;
number T=imap(R,T);
T;
7629412809180100-T*T;

tst_status(1);$

