// $Id: fglmquot1_s.tst,v 1.1 1999-11-24 13:32:29 wichmann Exp $

//
// fglm_s.tst - short tests for fglm and stdfglm
//              fglm: reco1, omdi, cyclic5 in char 32003 and 0
//           stdfglm: caprasse in char 32003 and 0
//


LIB "tst.lib";
tst_init();
tst_ignore("CVS: $Id: fglmquot1_s.tst,v 1.1 1999-11-24 13:32:29 wichmann Exp $");

option(redSB);

// first the easy cases
ring r=0,(x,y,z),lp;
ideal i=x2,y2,z2; i=std(i);
fglmquot(i,0);
fglmquot(i,1);
fglmquot(i,x);
// check if it is tested whether the polynomial is reduced
fglmquot(i,x2);

// now a real world ex
kill r;
ring r=0,(x5,x4,x3,x2,x1),lp;
poly  f1= x1^2 + x1 + x2 + x3 + x4 + x5 - 2*x1 - 4;
poly  f2= x2^2 + x1 + x2 + x3 + x4 + x5 - 2*x2 - 4;
poly  f3= x3^2 + x1 + x2 + x3 + x4 + x5 - 2*x3 - 4;
poly  f4= x4^2 + x1 + x2 + x3 + x4 + x5 - 2*x4 - 4;
poly  f5= x5^2 + x1 + x2 + x3 + x4 + x5 - 2*x5 - 4;
ideal i=f1,f2,f3,f4,f5;
ideal j=stdfglm(i);
poly q=4*x1^6-12*x1^5-4*x1^4+44*x1^3-48*x1^2+16*x1;
ideal j1=fglmquot(j,q);
j1;
kill r;

tst_status(1);$
