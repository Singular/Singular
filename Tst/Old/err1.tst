// v 0.9.2a, G. Pfister
ring r1=0,(y(1..3)),dp;
ring r=0,(x,y,z,u),dp;
ideal null;
ideal su=y2,y3,0;
map phi=r1,su;
setring r1;
ideal ker=preimage(r,phi,null);
ker;
kill r,r1;
// v 0.9.2c, A. Heydtmann
ring r=0,(x,y,z),dp;
ideal i=x2+y2;
ideal si=std(i);
qring Qring=si;
x2+y2==0;       // sollte diese Abfrage nicht auch mal '1' liefern?
setring r;
ring Q=si;
// Ja hier kommt eine Fehlermeldung
qring Q=si;
x2+y2==0;
// Aber das duerfte doch Singular nicht so durcheinander bringen?
$
