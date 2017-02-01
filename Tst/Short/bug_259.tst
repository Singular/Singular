LIB "tst.lib";
tst_init();

LIB("normal.lib");


ring r=0,(x,y),dp;
poly f = x6y4+4x5y3+3x4y3+6x4y2+19/3x3y2+4x3y+3x2y2+11/3x2y+x2+2xy+1/3x+y;

ASSUME(0,  1 == genus(f)       );
ASSUME(0,  0 == genus((x+y)*f) );
ASSUME(0,  0 == genus(x*f)     );
ASSUME(0,  0 == genus(y*f)     );
ASSUME(0, -1 == genus(y*x)     );

kill r;

ring r = 0,(x,y),dp;
poly f = x6y4+4x5y3+3x4y3+6x4y2+19/3x3y2+4x3y+3x2y2+11/3x2y+x2+2xy+1/3x+y;
poly fy = f*y;

ring r3 = 0,(x,y,z),dp;
poly Fy = subst(homog(imap(r,fy),z),x,1);

ring ryzdp = (0),(y,z),(dp(2),C);
poly Fy = imap(r3,Fy);
short = 0; Fy;

ring RXY = 0,(X,Y),dp;
poly Fy = fetch(ryzdp,Fy);
short = 0; Fy;

proc flip(poly f)
{
   return( substitute(   f,maxideal(1),   ideal(var(2),var(1))   ) );
}

def dl  = deltaLoc( Fy,maxideal(1) );

def dlf = deltaLoc( flip(Fy),maxideal(1) );
int i;
 ASSUME(0, size(dl)==size(dlf) );
for (i =1;i<=size(dl);i++)
{
   ASSUME(0, dl[i]==dlf[i] );
}

// Any plane curve consisting of d rational components has genus 1-d.
// We study a nodal cubic, combined with 3 resp. 4 lines.
// The genus then has to be -3 resp. -4
ring rdp = 0,(x,y),dp;
poly NC = (3*x^2*y+4*y^3-x^2-3*y+1);   // nodal cubic
genus(NC);      // 0
poly F = (x-1) * (x+1) * (3*y-1)* NC;  // 3A_5 + 4A_1
genus(F);     // -3
genus(F*y);   // -4
genus(F/(x-1));  // -2
genus(F/(x-1)*y);  // -3
genus(F*(y+1));  // -4
genus(F*(y+x));  // -4

tst_status(1); $

