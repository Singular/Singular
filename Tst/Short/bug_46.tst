LIB "tst.lib";
tst_init();
// error in absPrimdecGTZ
LIB "primdec.lib";
ring rr=0,(x,y,z),dp;
ideal i=x2+1,y2+y+1,z3+z2+z+1;
def RR=absPrimdecGTZ(i);
setring RR;absolute_primes;
kill RR;

ring r=0,(a,b,c,d),dp;
ideal I=a2+1,b2+b+1,c3+c2+c+1;
def S=absPrimdecGTZ(I);
setring S; absolute_primes;
kill S,r;

ring r=0,(a,b,c,d),dp;
ideal i=d2+1/3,c+1/2+1/2*d,b-1/2+3/2*d,a+3/4+3/4*d;
def R=absPrimdecGTZ(i);
setring R;
absolute_primes;
kill R,r;

ring r=0,(x,y),dp;
ideal i=x2,xy;
def S=absPrimdecGTZ(i);
setring S; absolute_primes;

tst_status(1);$
