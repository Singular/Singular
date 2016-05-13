LIB "tst.lib";
LIB "nfmodsyz.lib";
tst_init();

proc tst_test_nfmodsyz(def I)
{
   // input type ideal or module
   def Jtst = nfmodSyz(I);
   Jtst;
}

ring r = 0,(x,y,z),dp;
module M = [x2z+x-7*y,z2+9*x], [y2+7*z+7,10*z3-7*x2], [-xz+52*yz,xy+49*z],
           [52*xyz+yz,7*xy+y+z], [10*xy+z2,xz+x+7*z];
tst_test_nfmodsyz(M);
kill r;

ring r = (0,a),(x,y),(dp,c);
minpoly = a^3+2a+7;
module M = [(a/2+1)*x^2+2/3y, 3*x-a*y+ a/7+2], [ax, y];
tst_test_nfmodsyz(M);
kill r;

ring r = (0,a),(x,y,z),(c,dp);
minpoly = a3+a+1;
module M = [x2z+x+(-a)*y, z2+(a+2)*x],
           [y2+(a)*z+(a), (a+3)*z3+(-a)*x2],
           [-xz+(a2+3)*yz, xy+(a2)*z];
tst_test_nfmodsyz(M);
kill r;

ring r = (0,a), (x,y,z), (c,dp);
minpoly = a^3+7a-5;
module M = [(a+8)*x2y2+5*xy3,(-a2+3)*x3z+x2yz], [x5+2*y3z2,13*a*y2z3+5*yz4],
           [8*x3+(a2+12)*y3,xz2+(3a)], [(-a+7)*x2y4+y3z3,18*y3z2];
tst_test_nfmodsyz(M);
kill r;
     
ring r=(0,a),(x,y,z,w,u),(c,dp);
minpoly = a^3+a+5;
module M = (5+a)*x^3*y^2*z + (a+3)*y^3*x^2*z + (a+7)*x*y^2*z^2,
	(a+3)*x*y^2*z^2 + (a+1)*x^5 + (a-11)*y^2*z^2,
	(a+2)*x*y*z + (a+7)*x^3 + 12*y^3 + 1,3*x^3 +(a- 4)*y^3 + y*z^2;
tst_test_nfmodsyz(M);
kill r;

ring r=(0,a),(x,y,z,w,u),(c,dp);
minpoly = a^2+1;
module M = x+(a/2147483647)*x*y+(a/2147483646)*y*z,
	  x^2*z + (a/2147483647)*y^3 + (a+1)*y*z^2,
	  (2*a*x)/3-(7*a*y)/23 + (9*z)/7;
tst_test_nfmodsyz(M);
kill r;
tst_status(1);$
