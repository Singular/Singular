// $Id: paramet.tst,v 1.1 1998-08-07 11:58:32 obachman Exp $

// 
// paramet.tst -- short test fopr paramet.lib
//
LIB "tst.lib";
tst_init();
tst_ignore("$Id: paramet.tst,v 1.1 1998-08-07 11:58:32 obachman Exp $");
LIB "paramet.lib";

/// Examples for parametrize

/// Example 1


ring r=0,(x,y),dp;
ideal i=x^2-y^3;
parametrize(i);

/// Example 2

ring r=0,(x,y,z),dp;
ideal i=x2-y2z2-y3;
parametrize(i);

/// Example 3

ring r=0,(x,y,z),dp;
ideal i=z2-x2y;
parametrize(i);

/// Example 4

ring r=0,(x,y,z),dp;
ideal i=z2-x2y;
parametrize(i);

/// Example 5

ring r=0,(x,y,z),dp;
ideal i=x2-y3;
parametrize(i);

/// Example 6

ring r=0,(x,y,z),dp;
ideal i=y2-xz,z2-x2y,x3-yz;
parametrize(i);

/// Example 7 - ideal is not prime

ring r=0,(x,y),dp;
ideal i=xy;
parametrize(i);

/// Example 8 - you get a parametrization of the reduced ideal

ring r=0,(x,y),dp;
ideal i=x2;
parametrize(i);

/// Example 9 - wrong ordering

//ring r=0,(x,y),ls;
//ideal i=x2-y3;
//parametrize(i);





/////////////////////////////////////////////////////////////////////

/// Examples for parametrizepd

/// Example 1  
ring r=0,(x,y,z),dp;
ideal i=y2z5-x2y2z2+y2z4-z6-z5+x4-x2z2,-y3z3+yz4+x2yz;
parametrizepd(i);

/// Example 2

ring r=0,(x,y,z),dp;
ideal i=z^2-x^2*y,y^2-x*z;
parametrizepd(i);

/// Example 3

ring r=0,(x,y,z),dp;
ideal i=y^5*z^2-x^2*y^6-x*y^3*z^3+x^3*y^4*z-x^4*y^2*z^2+x^6*y^3+x^5*z^3-x^7*y*z,y^6*z^2-x^2*y^7-2*x^4*y^3*z^2+2*x^6*y^4+x^8*z^2-x^(10)*y;
parametrizepd(i);

/// Example 4

ring r=0,(x,y,z),dp;
ideal i=y^6*z^2-x^2*y^7-x*y^4*z^3+x^3*y^5*z-x^3*y^2*z^2+x^5*y^3+x^4*z^3-x^6*y*z,y^8*z^2-x^2*y^9-2*x^3*y^4*z^2+2*x^5*y^5+x^6*z^2-x^8*y;
parametrizepd(i);

/// Example 5 - gives a parametrization which is not suitable for plotting reasons

ring r=0,(x,y,z,u),dp;
ideal i=x-zu,y2-zu2;
parametrizepd(i);

/// Example 6 - one component has no parametrization

ring r=0,(x,y,z),dp;
ideal i=-x2y3+x3yz+y2z2-xz3,y2z-xz2;
parametrizepd(i);

/// Example 7 - wrong ordering!

//ring r=0,(x,y,z),ls;
//ideal i=x2-y2z2-y3;
//parametrizepd(i);



///////////////////////////////////////////////////////////////////

/// Examples for parametrizesing

/// Example 1

ring r=0,(x,y),ls;
poly f=x^2-y^3;
parametrizesing(f);

/// Example 2

ring r=0,(x,y),ls;
poly f=x^3+y^3-3*x*y;
parametrizesing(f);

/// Example 3

ring r=0,(x,y),ls;
poly f=y*x^2-y^8;
parametrizesing(f);

/// Example 4

ring r=0,(x,y),ls;
poly f=-x6-x5+2x3y2+x2y2-y4;
parametrizesing(f);

/// Example 5

ring r=0,(x,y),ls;
poly f=x6y4-x8y+x5y4-2x3y6-x7y+2x5y3-x2y6+y8+x4y3-x2y5;
parametrizesing(f);

/// Example 6 - wrong number of variables

ring r=0,(x,y,z),dp;
poly f=x2-y2z2-y3;
parametrizesing(f);

/// Example 7 - wrong ring ordering

ring r=0,(x,y),lp;
poly f=x2-y3;
parametrizesing(f);
tst_status(1);$



