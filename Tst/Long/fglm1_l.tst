// $Id$

//
// fglm_s.tst - long tests for fglm 
//              fglm: cassou-mod1, lamm1 in char 0
//


LIB "tst.lib";
tst_init();
tst_ignore("CVS: $Id$");

option(redSB);

// =========================
// cassou-mod1 in char 0
ring r=0,(a,b,c,d), dp;
ideal i= 15a4bc2+6a4b3+21a4b2c-144a2b-8a2b2d-28a2bcd-648a2c+36c2d+9a4c3-120, 30b3a4c-32cd2b-720ca2b-24b3a2d-432b2a2+576db-576cd+16ba2c2d+16c2d2+16d2b2+9b4a4+5184+39c2a4b2+18c3a4b-432c2a2+24c3a2d-16b2a2cd-240b, 216ca2b-162c2a2-81b2a2+5184+1008db-1008cd+15b2a2cd-15b3a2d-80cd2b+40c2d2+40d2b2, 261+4ca2b-3c2a2-4b2a2+22db-22cd;
ideal is=std(i);
ring s=0,(a,b,c,d), lp;
fglm(r,is);
kill r,s;

// =============================================
// lamm in char 0, with permutation of variables
ring r=0,(x,y), dp;
ideal i=45x8+3x7+39x6+30x5+13x4+41x3+5x2+46x+7, 49x7y+35xy7+37xy6+9y7+4x6+6y6+27x3y2+20xy4+31x4+33x2y+24x2+49y+43;
ideal is=std(i);
ring s=0,(y,x), lp;
fglm(r,is);
kill r,s;

tst_status(1);$

