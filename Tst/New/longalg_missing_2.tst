LIB "tst.lib"; tst_init();

ring SNB = (0,a), x, (lp,C);
minpoly = (a^2+1);

SNB;
poly f = x4-2*x+1; f;

poly ff = 3*a-7; ff;

poly gg = 11*a; gg;

poly g = a*f + f + a; g;

ring SNR = 0, (x, a), (lp, C);

option(Imap);

imap(SNB, f);// 
fetch(SNB, f);// 


imap(SNB, ff);// ?
fetch(SNB, ff);// 


imap(SNB, g);// 
fetch(SNB, g);// 

imap(SNB, gg);// 
fetch(SNB, gg);// 

tst_status(1);$
