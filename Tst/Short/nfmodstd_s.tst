LIB "tst.lib";
LIB "nfmodstd.lib";
tst_init();

proc tst_test_nfmodstd(def I)
{
   // input type ideal or module
   def Jtst = nfmodStd(I);
   Jtst;
}

ring r=(0,a),(x,y,z,w,u),dp;
minpoly = a^2+1;
ideal HIa=(8+a)*x^2*y^2+5*x*y^3+(3-a)*x^3*z +x^2*y*z,
	  x^5+2*y^3*z^2+13*y^2*z^3+5*y*z^4,
	  8*x^3+(12+a)*y^3+x*z^2+3,
	  (7-a)*x^2*y^4+18*y^3*z^2+y^3*z^3;
tst_test_nfmodstd(HIa);
ideal HJa=(2*a+3)*x*y^4*z^2 + (a+2)*x^2*y^3*z*w-x^2*y^3*z*w + 2*x*y*z^2*w^3 + (7*a-1)*y^3*w^4,
	  2*x^2*y^4*z + x^2*y*z^2*w^2-a*x*y^2*z^2*w^2 + (a+11)*x^2*y*z*w^3 -12*x*w^6 + 12*y*w^6,
	  2*y^5*z + x^2*y^2*z*w-x*y^3*z*w -x*y^3*w^2 +a*y^4*w^2 + 2*y^2*z*w^3,
	  3*x*y^4*z^3 + x^2*y^2*z*w^3-x*y^3*z*w^3 +(4+a)*y^3*z^2*w^3 +3*x*y*z^3*w^3 +4*a*z^2*w^6
	  -x*w^7+y*w^7;

 tst_test_nfmodstd(HJa);
ideal HBa =x+(a/2147483647)*x*y+(a/2147483646)*y*z,
	  x^2*z + (a/2147483647)*y^3 + (a+1)*y*z^2,
	  (2*a*x)/3-(7*a*y)/23 + (9*z)/7;

  tst_test_nfmodstd(HBa);
kill r;
ring r=(0,a),(x,y,z,w,u),dp;
minpoly =a^3+a+5;
ideal ka=(5+a)*x^3*y^2*z + (a+3)*y^3*x^2*z + (a+7)*x*y^2*z^2,
	(a+3)*x*y^2*z^2 + (a+1)*x^5 + (a-11)*y^2*z^2,
	(a+2)*x*y*z + (a+7)*x^3 + 12*y^3 + 1,3*x^3 +(a- 4)*y^3 + y*z^2;
tst_test_nfmodstd(ka);

ideal Hka=(a+5)*x^3*y^2*z + (a-3)*y^3*x^2*z + (a+7)*x*y^2*z^2,
	  (a+3)*x*y^2*z^2 + a*x^5 + (a+11)*y^2*z*w^2,
	  4*x*y*z + (a+7)*x^3 + 12*y^3 + a*z^3,
	  3*x^3 +(a- 4)*y^3 + y*z^2;
tst_test_nfmodstd(Hka);

ideal Ua = a*x + (a-1)*y + z + (a+2)*w + u,x*y + (a-1)*y*z + z*w +
	  (a + 2)*x*w + a*w*u,a*x*y*z + (a + 5)*y*z*w + a*x*y*u + (a+2)*x*w*u + a*z*w*u,
	  (a-11)*x*y*z*w + (a + 5)*x*y*z*u + a*x*y*w*u + a*x*z*w*u + a*y*z*w*u,(a + 3)*x*y*z*w*u + a+23;
tst_test_nfmodstd(Ua);
kill r;
ring r = 0,(x,y,z),dp;
module K = [x2z+x-7*y,z2+9*x], [y2+7*z+7,10*z3-7*x2], [-xz+52*yz,xy+49*z],
           [52*xyz+yz,7*xy+y+z], [10*xy+z2,xz+x+7*z];
tst_test_nfmodstd(K);
kill r;
ring r = (0,a),(x,y),(c,dp);
    minpoly = a^3+2a+7;
    module M = [(a/2+1)*x^2+2/3y, 3*x-a*y+ a/7+2], [ax, y];
tst_test_nfmodstd(M);
kill r;
proc tst_test_chinrempoly(list l, list m)
{
   def Jtst = chinrempoly(l,m);
   Jtst;
}

ring r=97,x,dp;
poly f=x^7-7*x + 3;
ideal J=factorize(f,1);
list m=J[1..ncols(J)];
list l= x^2+2*x+3, x^2+5, x^2+7;
tst_test_chinrempoly(l, m);
kill r;

ring r = 0, x,dp;
poly f1 = 30x7+5x6+23x5+27x4+5x3+13x2+29x+7;
poly f2 = 8x4+24x3+6x2+28x+5;
poly f3 = 14x4+8x3+2x2+11x+9;
poly f4 = 8x2+22x+4;
poly f5 = 7x4+9x3+12x2+x+11;
poly f6 = 26x2+x+9;
poly f7 = 19x3+4x2+2x+24;
poly f8 = x4+27x3+4x2+23x+30;
poly f9 = 20x3+11x2+16x+29;
poly f10 = 5x2+22x+10;
poly f11 = 12x5+16x4+8x3+28x2+x+26;
poly f12 = 3x11+6x10+21x9+7x8+6x7+20x6+25x5+19x4+10x3+6x2+18x+5;
poly f13 =19x2+18x+21;
poly f14 = 21x10+12x9+x8+29x7+14x6+8x5+21x4+14x3+4x2+28x+18;
poly f15 = 23x4+13x3+26x2+26x+14;

list m=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15;

poly g1 = 24x5+25x4+11x3+x2+3x+9;
poly g2 = 8x3+17x2+19x+25;
poly g3 = 26x3+27x2+14x+27;
poly g4 = 24x+23;
poly g5 =7x3+10x2+9x+22;
poly g6 = 30x+6;
poly g7 = 24x2+26x+2;
poly g8 = 16x+9;
poly g9 =17x3+25x2+11x+20;
poly g10 =x +15;
poly g11 = 14x3+x2+6x+16;
poly g12 = 8x9+22x8+15x7+7x6+23x5+15x4+5x3+21x2+x+6;
poly g13 =4x +3;
poly g14 = 19x5+11x4+22x3+27x2+x+13;
poly g15 = 22x2+15x+5;

list l=g1,g2,g3,g4,g5,g6,g7,g8,g9,g10,g11,g12,g13,g14,g15;

tst_test_chinrempoly(l, m);
tst_status(1);$
