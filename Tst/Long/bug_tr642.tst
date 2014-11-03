LIB "tst.lib";
tst_init();

LIB"normal.lib";
ring rng = 0, (T(1),T(2),T(3),y,x ),(dp(3),dp(2) );
ideal I = x^9+y*x^5-y^5+y^3,T(3)*x^2-y^3+y,T(3)*y^2-x^7-y*x^3,T(2)*y-x^4,T(2)*x^3-T(3)*y+x^3,T(1)-T(3)*x,T(3)^2+T(2)*x-y*x^5-y^2*x+x,T(2)*T(3)-y^2*x^2+x^2,T(2)^2+T(2)-T(3)*x;
I = std(I);
dim(I);
def J =  equidim(std(I))[1];
J;
genus(J,"pri");
genus(J);
genus(J,"nor");

tst_status(1); $

