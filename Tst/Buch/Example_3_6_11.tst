LIB "tst.lib";
tst_init();

LIB"primdec.lib";
ring A=0,(x,y,z),dp;
ideal I=zy2-zx3-x6;
ideal sing=I+jacob(I);
ideal J=radical(sing);
qring R=std(I);
ideal J=fetch(A,J);
ideal p=J[1];
ideal re=quotient(p,quotient(p*J,J));
re;

tst_status(1);$
