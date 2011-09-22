LIB "tst.lib"; tst_init();
LIB "kskernel.lib";
int p=5; int q=14;
int i=2; int j=9;
list L;
ring r=0,x,dp;
number c;
L[1]=3; L[2]=1; L[3]=3; L[4]=2;
KScoef(i,j,p,q,L);
c=imap(RC,C);
c;
L[1]=3; L[2]=1; L[3]=2; L[4]=3;
KScoef(i,j,p,q,L);
c=c+imap(RC,C);
c; // it is a coefficient of T1*T2*T3^2 in C[2,9] for x^5+y^14
tst_status(1);$
