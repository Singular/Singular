LIB "tst.lib";
LIB "nfmodsyz.lib";
tst_init();

proc tst_test_nfmodsyz(def I)
{
   // input type ideal or module
   def Jtst = nfmodSyz(I);
   Jtst;
}

ring r = (0,a),(x,y,z,w),(c,dp);
minpoly = a6 +a2+5;
module M = (a+3)*x^2*z+y^2*w+xw + zy, x^2+11*a*x*z+(a)*z^2+y*w,
             ax2 + a4y2+ w3, x2yzw +aw2, x+y+w;
tst_test_nfmodsyz(M);
kill r;

ring r = 0,(x,y,z), dp;
module M = [x2z+x-7*y,z2+9*x], [y2+7*z+7,10*z3-7*x2], [-xz+52*yz,xy+49*z],
           [52*xyz+yz,7*xy+y+z], [10*xy+z2,xz+x+7*z];
tst_test_nfmodsyz(M);
kill r;

ring r = (0,a),(x,y),(dp,c);
minpoly = a^3+2a+7;
module M = [(a/2+1)*x^2+2/3y, 3*x-a*y+ a/7+2], [ax, y];
tst_test_nfmodsyz(M);
kill r;

tst_status(1);$
