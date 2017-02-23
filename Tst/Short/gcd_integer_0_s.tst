//
// gcd_integer_0_s.tst - short tests for gcd calculations in Z.
//
//
//
//
LIB("poly.lib");
LIB "tst.lib";
tst_init();


proc gcdEqual(gcda,gcdb)
{
   return( (gcda==gcdb) || (-gcda==gcdb) );
}


//test based on gcd() properties
proc testGcdProperties(poly a, poly b, poly c)
{
       poly   ab, bc, gcd_a_c, gcd_a_b, gcd_b_c, gcd_ab_bc, b_times_gcd_a_c;
       int m;

       ab = a*b;
       bc = b*c;
       gcd_a_b = gcd(a,b);
       gcd_b_c = gcd(b,c);
       gcd_a_c = gcd(a,c);
       gcd_ab_bc = gcd(ab,bc);
       b_times_gcd_a_c = b*gcd_a_c;
       m = random(-500,500);     

       ASSUME(0, gcdEqual(  b_times_gcd_a_c, gcd_ab_bc)    );
       ASSUME(0, gcdEqual(  gcd(a, gcd_b_c ) ,  gcd( gcd_a_b, c)  ));
       ASSUME(0, gcdEqual(  gcd_a_b ,  gcd( b, a))  );
       ASSUME(0, gcdEqual(  gcd_ab_bc ,  gcd( bc, ab) )  );

       ASSUME(0, gcdEqual(  gcd_a_b ,   gcd(a + m*b, b) )  );
       ASSUME(0, gcdEqual(  gcd_ab_bc ,   gcd(ab + m*bc, bc) ) );
}

//
// - ring r1=0,x,dp.
//

tst_ignore( "ring r1=integer,x,dp;" );
ring r1=integer,x,dp;

gcd(297,366);
gcd(366,324);
testGcdProperties(297,366,324);



gcd(297, lcm(366, 324)) == lcm(gcd(297, 366), gcd(297, 324));
lcm(297, gcd(366, 324)) == gcd(lcm(297, 366), lcm(297, 324));



poly f=(-9554*x^4-12895*x^3-10023*x^2-6213*x);
poly g;
testGcdProperties(f, g, f*g);
testGcdProperties(f, f, f*f);

// some trivial examples
gcd(0, 0);
testGcdProperties(0, 0, 0);
gcd(0, 3123);
testGcdProperties(0, 3123, 0);
gcd(4353, 0);
testGcdProperties(4353, 0, 0);

gcd(0, f);
gcd(f, 0);
testGcdProperties(f, 0, f);

gcd(23123, f);
testGcdProperties(23123, f, 23123*f);
gcd(f, 13123);


f=11*47894*19*(-9554*x^4-12895*x^3-10023*x^2-6213*x);

gcd(0, 3123*123456);
testGcdProperties(0, 3123*123456, 3123*123456);
gcd(4353*8798, 0);
testGcdProperties(4353*8798, 0, 0);

gcd(0, f);
gcd(f, 0);
testGcdProperties(f, 0, f);

gcd(23123*3, f);
gcd(f, 13123*2);
testGcdProperties(23123*3, f, 13123*2);

f=(2*x^3+2*x^2+2*x);
g=(x^3+2*x^2+2*x+1);
gcd(f, g);
testGcdProperties(f, g, f*g);

f=(-x^9-6*x^8-11*x^7-17*x^6-14*x^5-14*x^4-8*x^3-6*x^2-2*x-1);
g=(-x^9-2*x^8-2*x^7-3*x^4-6*x^3-6*x^2-3*x-1);
gcd(f, g);
testGcdProperties(f*g, g, f);

f=(4*x^9+12*x^8+29*x^7+42*x^6+54*x^5+48*x^4+35*x^3+17*x^2+6*x+1);
g=(x^11+6*x^10+11*x^9+17*x^8+14*x^7+14*x^6+8*x^5+6*x^4+2*x^3+x^2);
gcd(f, g);
testGcdProperties(f, g*f, g);
gcd(1*13*f, 1*4*g);
testGcdProperties(1*13*f, 1*4*g, f*g);

f=(1412500*x^6+6218750*x^5-6910000*x^4-1201250*x^3-19470000*x^2-27277500*x);
g=(156600000*x^5-1363865625*x^4+2627604000*x^3-2727731250*x^2+4628808000*x);
gcd(f, g);
testGcdProperties(f*g, g, g*g);


tst_ignore( "ring r2=(integer),(a,x),dp;" );
ring r2=(integer),(a,x),dp;

poly f=(-9554*x^4-12895*x^3-10023*x^2-6213*x);
poly g;
testGcdProperties(f*g, g, g*g);

gcd(0, 0);
testGcdProperties(0,0,1);
gcd(0, 3123*a);
testGcdProperties(0, 3123*a, a);
gcd(4353, 0);
testGcdProperties(4353, 0, a);

gcd(0, f);
gcd(f, 0);
testGcdProperties(0,f,0);

gcd(23123, f);
gcd(f, 13123);
testGcdProperties(23123,f,13123);

f=11*47894*19*(-9554*x^4-12895*x^3-10023*x^2-6213*x);

gcd(0, 3123*(123456*a));
gcd(4353*8798, 0);
testGcdProperties( 4353*8798, 0, 3123*(123456*a) );

gcd(0, f*a);
gcd(f, 0);
testGcdProperties(f,0,f*a);

gcd(23123*(3*a), f);
testGcdProperties(23123*(3*a), f, 23123*(3*a)*f);
gcd(f, 13123*2);
testGcdProperties(f, 13123*2, f* 13123*2);

f=(2*x^3+2*x^2+2*x);
g=(x^3+2*x^2+2*x+1);
gcd(f, g);
testGcdProperties(f,g,f*g);


tst_ignore( "ring r3=integer,(u,v),dp;" );
ring r4=integer,(u,v),dp;

poly f;
poly g;

f=(-8*u^2*v+8*u*v^2-24*u);
g=(-64*u^2*v+16*u*v^2);
gcd(f, g);
testGcdProperties(f,g,f*g);
f=(192*u^6*v^4-240*u^5*v^5+384*u^5*v^3+48*u^4*v^6+96*u^4*v^4-576*u^4*v^2-48*u^3*v^5+144*u^3*v^3);
g=(1536*u^6*v^4-768*u^5*v^5-1536*u^5*v^3+96*u^4*v^6+768*u^4*v^4-96*u^3*v^5);
gcd(f, g);
testGcdProperties(f,g,f*g);
f=(-256*u^3*v+128*u^2*v^2-16*u*v^3);
g=(-64*u^3+48*u^2*v^2+32*u^2*v-192*u^2-12*u*v^3-4*u*v^2+48*u*v);
gcd(f, g);
testGcdProperties(f,g,f*g);

ring r=integer,(x,y),dp;

poly f=4*(xy+x);
poly g=2*(xy+x);
gcd(f,g); // -> 2xy+2x 
testGcdProperties(f,g,f*g);

f=4*(xy);
g=2*(xy);
gcd(f,g); // -> 2xy
testGcdProperties(f,g,f*g);

f = 3*x^2*(x+y);
g = 9*x*(y^2 - x^2);
gcd(f,g);
testGcdProperties(f,g,f*g);

f = (x^3 - x + 1)*(x + x^2); 
g = (x^3 - x + 1)*(x + 1);
gcd(f,g);
testGcdProperties(f,g,f*g);

poly f1; poly f2;
for (int i=0; i<100; i++)
{
    f1=f1+x^(1000-i)*y^i;
    f2=f2+y^(1000-i)*x^i;
}
testGcdProperties(f1,f2,f2);
tst_status();
poly p=gcd(f1,f2);
tst_status();
ideal I = f1,f2;
matrix M=syz(I);
tst_status();
poly q=f2/M[1,1];
tst_status();

simplify(p,1)-simplify(q,1);

tst_status(1);$
