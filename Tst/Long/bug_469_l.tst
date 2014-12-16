LIB "tst.lib";
tst_init();


LIB("normal.lib");
ring rng = 0, (T(2),T(3),y,x ),(dp(2),dp(2) );
ideal I = T(3)*x^2-y^3+y,T(3)^2+T(2)*x-y*x^5-y^2*x+x,T(2)^2+T(2)-T(3)*x,x^9+y*x^5-y^5+y^3,T(3)*y^2-x^7-y*x^3,T(2)*y-x^4,T(2)*x^3-T(3)*y+x^3,T(2)*T(3)-y^2*x^2+x^2;
I = std(I);
dim(I);
def J =  equidim(std(I))[1];
int g1,g2,g3;
g1 = genus(J);
g1;
g2 = genus(J,"pri");
g2;
g3 = genus(J,"nor");
g3;


ASSUME(0, g1==g2 && g2==g3 );

/* needs too long...

LIB("normal.lib");
ring  r = 0,(y,x),dp;
ideal I = y^5-y^3-y*x^5-x^9;
dim(std(I) );
g1 = genus(I);
g1;
g2 = genus(I,"pri");
g2;
g3 = genus(I,"nor");
g3;


ASSUME(0, g1==g2 && g2==g3 );

list nor = normal(I);

def R=nor[1][1];
setring R;
ideal s=std(norid);
g1 = genus(s);
g1;
g2 = genus(s,"pri");
g2;
g3 = genus(s,"nor");
g3;


ASSUME(0, g1==g2 && g2==g3 );


LIB("normal.lib");
ring rng = 0, (T(1),T(2),T(3),y,x ),(dp(3),dp(2) );
ideal I = x^9+y*x^5-y^5+y^3,T(3)*x^2-y^3+y,T(3)*y^2-x^7-y*x^3,T(2)*y-x^4,T(2)*x^3-T(3)*y+x^3,T(1)-T(3)*x,T(3)^2+T(2)*x-y*x^5-y^2*x+x,T(2)*T(3)-y^2*x^2+x^2,T(2)^2+T(2)-T(3)*x;
I = std(I);
def J =  equidim(std(I))[1];
g1 = genus(J);
g1;
g2 = genus(J,"pri");
g2;
g3 = genus(J,"nor");
g3;


ASSUME(0, g1==g2 && g2==g3 );


kill rng;

*/



tst_status(1); $

