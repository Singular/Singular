//---------------------------------------------
// an error of 0.8.8f (duco)
ring  r11=5,(s,x(1..5),y(1..5)),(dp(1),dp(10));
map taylor=r11,s,1+s*x(1),1+s*x(2),1+s*x(3),1+s*x(4),1+s*x(5),1+s*y(1),1+s*y(2),1+s*y(3),1+s*y(4),1+s*y(5);
poly form=x(1)*x(2)*x(3)*x(4)*x(5)-y(1)*y(2)*y(3)*y(4)*y(5);
poly ontw=taylor(form);
ontw;
matrix ce=coef(ontw,s);
ce;  //ergebnis ist falsch; mit char=7 aber richtig.
kill r11;
//-------------------------------------------------
//an error of 0.8.8c (martin)
ring r=32003,(x,y),ds;
poly f=1+444x+xy+x3y;
coef(f,x);
kill r;
LIB "tst.lib";tst_status(1);$
