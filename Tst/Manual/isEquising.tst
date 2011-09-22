LIB "tst.lib"; tst_init();
LIB "equising.lib";
ring r = 0,(a,b,x,y),ds;
poly F = (x2+2xy+y2+x5)+ay3+bx5;
isEquising(F);
ideal I = ideal(a);
qring q = std(I);
poly F = imap(r,F);
isEquising(F);
ring rr=0,(A,B,C,x,y),ls;
poly f=x7+y7+(x-y)^2*x2y2;
poly F=f+A*y*diff(f,x)+B*x*diff(f,x);
isEquising(F);
isEquising(F,2);    // computation over  Q[a,b] / <a,b>^2
tst_status(1);$
