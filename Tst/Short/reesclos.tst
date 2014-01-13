LIB "tst.lib";
tst_init();

LIB "reesclos.lib";

ring r=0,(x,y),dp;
poly f=x^5+x^2*y^2+y^5;
ideal I=x*diff(f,x),y*diff(f,y);
list JJ=normalI(I);
JJ;

kill r;

ring r=0,(x,y),dp;
poly f=(x^2-y^3)^2+x*y^4;
ideal I=x*diff(f,x),y*diff(f,y);
list JJ=normalI(I);
JJ;

kill r;

ring r=0,(x,y),dp;
ideal I=x^5+x^5+y^3*x+x^2, y^6+y^5*x+x^6+y^5+y^4*x;
list JJ=normalI(I);
JJ;

kill r;

ring r=0,(x,y,z),dp;
ideal I =x^2*y,y^2*z,z^2*x;
list JJ=normalI(I);
JJ;

tst_status(1);
$
