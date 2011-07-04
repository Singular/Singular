LIB "tst.lib";
tst_init(); option(Imap);

ring r0 = 0, (a,b,x,y), lp;

ring s0 = 32003, (a,b,x,y), lp;

ring t0 = 7, (a,b,x,y), lp;

ring f0 = (0, a) , (b, x, y), lp; minpoly = a2 + 1;
poly p = 1 + a + a2 + b + ab + ax + ay + bx + x + y + xy; p;
p;

setring r0;
imap(f0, p);

setring s0;
imap(f0, p);

setring t0;
imap(f0, p);

ring R = (0,U),(x1(0),x1(1),x1(2),x1(3),x1(4),x2(0),x2(1),x2(2),x2(3),x2(4),t1(1),t1(2),t1(3),t2(1),t2(2),t2(3)),(dp(16),C); minpoly = (U2+1) ;

ideal I = 
x1(3)-x1(4)+(-U)*x2(3)+(U)*x2(4)+(43/200*U-539/500)*t1(3)+(539/500*U+43/200)*t2(3)+(967/500*U-153/500);

ring S=0, (x1(0),x1(1),x1(2),x1(3),x1(4),x2(0),x2(1),x2(2),x2(3),x2(4),t1(1),t1(2),t1(3),t2(1),t2(2),t2(3)),dp;

ideal I=imap(R,I); I; setring R; I;
ring R2 = (0,U),(x,y),dp; minpoly = (U2+1) ;

ideal I = 
(-U)*x+y+(2+U)*xy;

ring S2=0, (x,y),dp;

ideal I=imap(R2,I); I; setring R2; I;

tst_status(1);$
