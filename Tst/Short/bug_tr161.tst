LIB "tst.lib";
tst_init();

// sign got lost in map
ring r=real, (x,y), dp;
poly f= -1+2x2+3y3-2x-3y;
poly g= -1-2x2-3y3+x+3y;
ideal m=f,g;
m;
ring t=0,(x,y),dp;
ideal n=imap(r,m);
n;
poly newf=imap(r,f); poly newg=imap(r,g); newf; newg;

tst_status(1);$
