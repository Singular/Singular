LIB "tst.lib"; tst_init();

ring SNB = (0,a), x, (lp,C);
minpoly = (a^2+1);

SNB;
poly f = x4-2*x+1;
f;

ring SNR = 0, (x, a), (lp, C);
imap(SNB, f);// 

tst_status(1);$
