// $Id$

//
// fglmquot1_l.tst - long tests for fglmquot command
//

LIB "tst.lib";
tst_init();
tst_ignore("CVS: $Id$");

option(redSB);

ring r=0,(x9,x8,x7,x6,x5,x4,x3,x2,x1),lp;
poly  f1= x1^2 + x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8 + x9 - 2*x1 - 8;
poly  f2= x2^2 + x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8 + x9 - 2*x2 - 8;
poly  f3= x3^2 + x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8 + x9 - 2*x3 - 8;
poly  f4= x4^2 + x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8 + x9 - 2*x4 - 8;
poly  f5= x5^2 + x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8 + x9 - 2*x5 - 8;
poly  f6= x6^2 + x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8 + x9 - 2*x6 - 8;
poly  f7= x7^2 + x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8 + x9 - 2*x7 - 8;
poly  f8= x8^2 + x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8 + x9 - 2*x8 - 8;
poly  f9= x9^2 + x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8 + x9 - 2*x9 - 8;
ideal i=f1,f2,f3,f4,f5,f6,f7,f8,f9;
ideal j=stdfglm(i);
poly q=(8*x1^14-56*x1^13-280*x1^12+2856*x1^11-2856*x1^10-28392*x1^9+97656*x1^8-81096*x1^7-170688*x1^6+513856*x1^5-601216*x1^4+380800*x1^3-129024*x1^2+18432*x1);
fglmquot(j,q);

tst_status(1);$
