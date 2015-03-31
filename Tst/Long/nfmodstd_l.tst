LIB "tst.lib";
LIB "nfmodstd.lib";
tst_init();
proc tst_test_nfmodstd(ideal I)
{
   ideal Jtst = nfmodStd(I);
   Jtst;
}

ring r=(0,a),(x,y,z,w,u),dp;
minpoly = a^2+1;
ideal HJa=[(2*a+3)*x*y^4*z^2 + (a+2)*x^2*y^3*z*w-x^2*y^3*z*w + 2*x*y*z^2*w^3 + (7*a-1)*y^3*w^4,
	  2*x^2*y^4*z + x^2*y*z^2*w^2-a*x*y^2*z^2*w^2 + (a+11)*x^2*y*z*w^3 -12*x*w^6 + 12*y*w^6,
	  2*y^5*z + x^2*y^2*z*w-x*y^3*z*w -x*y^3*w^2 +a*y^4*w^2 + 2*y^2*z*w^3,
	  3*x*y^4*z^3 + x^2*y^2*z*w^3-x*y^3*z*w^3 +(4+a)*y^3*z^2*w^3 +3*x*y*z^3*w^3 +4*a*z^2*w^6
	  -x*w^7+y*w^7];

 tst_test_nfmodstd(HJa);
kill r;
ring r=(0,a),(x,y,z,w,u,v,n),dp;
minpoly = a^4+a^3+a^2+a+1;
ideal I1=[(8/a^2)*x^2*y^2+5*x*y^3+(3-a^2)*x^3*z + x^2*y*z,
	  x^5+ 2*y^3*z^2+ 13*y^2*z^3+ 5*y*z^4,
	  8*x^3+(12*a+a^2)*y^3 +x*z^2+3,
	  (7*a^2-a)*x^2*y^4+18*y^3*z^2+y^3*z^3];
tst_test_nfmodstd(I1);

ideal HUa = [a*x+(a-1)*y+(a+2)*w+u,x*y+(a-1)*y*z+z*w + (a+2)*x*w+a*w*u,
	    a*x*y*z+(a+5)*y*z*w+a*x*y*u+(a+2)*x*w*u+a*z*w*u,(a-11)*x*y*z*w+(a+5)*x*y*z*u+a*x*y*w*u +
	    a*x*z*w*u+a*y*z*w*u,(a + 3)*x*y*z*w*u -(a+23)*v^5];
tst_test_nfmodstd(HUa);
kill r;
ring r=(0,a),(x,y,z,w,u,v,n),dp;
minpoly = a^3 + 5a + 7;
ideal I6a=[a*x+(a+2)*y+z+w+u+v,x*y+y*z+z*w+w*u+(a+3)*x*v+u*v,x*y*z+y*z*w
	  + (a+1)*z*w*u+x*y*v+x*u*v+w*u*v,(a-1)*x*y*z*w + y*z*w*u +x*y*z*v +x*y*u*v+ x*w*u*v+ z*w*u*v,
	  x*y*z*w*u+(a+1)*x*y*z*w*v+x*y*z*u*v+x*y*w*u*v+x*z*w*u*v+y*z*w*u*v,x*y*z*w*u*v-a+2];
tst_test_nfmodstd(I6a);
kill r;
ring r=(0,a),(x,y,z,w,u,v,n),dp;
minpoly = a^3+2;
ideal I= [(2*w*y-2*a+3)*x-z*y^2-z,2*z*x^3 + (4*w*y + 5)*x^2 + (4*z*y^2 + 4*z)*x +
	    2*w*y^3-10*y^2 - 10*w*y,(w^2-a^2+2*a)*x + 11*w*y*z-2*z, (-z^2 + 4*w^2+a^2+2)*x*z
	    + (4*w*z^2 + 2*w^3-10*w)*y + 4*z^2-10*w^2 + a^2+a];
tst_test_nfmodstd(I);
ideal I1= [(2*w*y-(2*a+3)*z^2)*x-z*y^2-z^2*x,
            2*z*x^2*w + (4*w*y + 5*z^2)*x^2 + (4*z*y^2 + 4*z^2*w)*x + 2*w*y^3-10*x^2*y^2 - 10*x*w*y*z,
            (w^2-(a^2+a)*y*z)*x + 11*w*y*z-2*z^3,
            (-z^2 + 4*w^2 + (a^2+2)*y^2)*x*z + (4*w*z^2 + 2*w^3-10*w^3)*y + 4*z^2*x*y-10*w^2*x*z + (a^2+2)*y^3*w];
            tst_test_nfmodstd(I1);
kill r;
ring r=(0,a),(x,y,z,w,u),dp;
minpoly = a^3 + 2a + 11;
ideal Ua = [(a-x)^2 + (a-1)^2*y + z + (a+2)*w + u,x*y + (a-1)*y*z + z*w +
	  (a + 2)*x*w + a*w*u,a*x*y*z + (a + 5)*y*z*w + a*x*y*u + (a+2)*x*w*u + a*z*w*u,
	  (a-11)*x*y*z*w + (a + 5)*x*y*z*u + a*x*y*w*u + a*x*z*w*u + a*y*z*w*u,(a + 3)*x*y*z*w*u + a+23];
tst_test_nfmodstd(Ua);
kill r;

proc tst_test_chinrempoly(list l, list m)
{
   ideal Jtst2 = simplify(chinrempoly(l,m),2);
   Jtst2;
}

ring r=0,x,dp;
poly f1 = 21x4+12x3+3x2+23x+8;
poly f2 = 2x3+7x2+13x+26;
poly f3 = 21x5+27x4+11x3+8x2+22x+21;
poly f4 = 20x11+8x10+26x9+4x8+7x7+18x6+2x5+16x4+12x3+5x2+19x+9;
poly f5 = 13x9+30x8+15x7+x6+17x5+7x4+13x3+12x2+20x+18;
poly f6 = 21x8+13x7+28x6+25x5+8x4+21x3+24x2+19x+26;
poly f7 = 18x8+7x7+15x6+22x5+8x4+5x3+3x2+27x+26;
poly f8 = 15x6+10x5+7x4+26x3+13x2+19x+25;
poly f9 = 12x6+25x5+29x4+8x3+20x2+19x+20;
poly f10 =  5x8+15x7+14x6+29x5+27x4+4x3+10x2+17x+16;
poly f11 = 23x4+19x3+28x2+5x+22;
poly f12 = 25x7+22x6+22x5+29x4+11x3+29x2+15x+28;
poly f13 = 28x11+8x10+14x9+23x8+18x7+10x6+8x5+16x4+28x3+20x2+3x+4;
poly f14 = 12x8+20x7+17x6+19x5+19x4+15x3+7x2+14x+5;
poly f15 =  27x7+2x6+6x5+20x4+4x3+24x2+28x+25;

list m=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15;

poly g1 = 7x3+25x2+2x+21;
poly g2 = 17x2+29x+18;
poly g3 = 26x2+30x+28;
poly g4 = 26x9+28x8+2x7+21x6+19x5+17x4+15x3+27x2+13x+25;
poly g5 = 6x6+27x5+x4+26x3+25x2+13x+19;
poly g6 = 28x7+14x6+11x5+18x4+11x3+21x2+9x+3;
poly g7 = 18x5+22x4+30x3+6x2+2x+14;
poly g8 = 16x3+21x2+4x+26;
poly g9 = 10x5+11x4+4x3+3x2+18x+19;
poly g10 = 15x5+8x4+5x3+8x2+x+13 ;
poly g11 = 2x+15;
poly g12 = 15x6+25x5+3x4+12x3+9x2+2x+21;
poly g13 = 25x6+28x5+18x4+28x3+24x2+x+29;
poly g14 = 28x6+19x5+30x4+19x3+15x2+21x+7;
poly g15 = 10x4+11x3+10x2+29x+23;

list l=g1,g2,g3,g4,g5,g6,g7,g8,g9,g10,g11,g12,g13,g14,g15;

tst_test_chinrempoly(l,m);
tst_status(1);$
