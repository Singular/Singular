LIB "tst.lib";
tst_init();

// ======================================= example B.6.12 ==================

ring R=0,(x,y),dp;
poly f=3x2y+4xy2+y3+3x2+6xy+y2+3x;
poly g=2x2y+xy2-y3-2x2-y2-2x;

ring S=(0,y),x,dp;
poly f=imap(R,f); poly g=imap(R,g);
f;
g;

ring T1=5,x,dp;
map phi=R,x,0;
poly f=phi(f); poly g=phi(g);
f;
g;

ring T2=7,x,dp;
map psi=R,x,0;
poly f=psi(f); poly g=psi(g);
f;
g;

setring T1;
phi=R,x,2;
f=phi(f); g=phi(g);   //we substitute y by 2
gcd(f,g);

setring T2;
psi=R,x,2;
f=psi(f); g=psi(g);
gcd(f,g);

setring R;
gcd(f,g);     //the SINGULAR built-in function

tst_status(1);$

