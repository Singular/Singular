LIB "tst.lib";
tst_init();

LIB "polyclass.lib";
printlevel=0;

 ring R1=0,(x,y),dp;
 Poly f=3*x^2+x*y+1;
 Poly g=2*x+y^3;
 f*g;
 f+g;
 f^3;
 string(f);


 ring R=(0,a),(x,y,z,w),ds;
 minpoly = a^2-2;
 Poly f=x^4+x^2*y^2+a*y^8+z^2-w^2;
 NormalFormEquation F;
 F.vars = ringlist(R)[2];
 F.realCase = 1;
 F.normalFormEquation = f;
 F.modality = 1;
 F.corank = 2;
 F.inertiaIndex = 1;
 F.determinacy = 8;
 F.milnorNumber = milnor(f.value);
 F.parameters = list(list(a*y^8,list(0,2)));
 F.singularityType = "X[13]";
 F;


 ring R2=(0),(x,y,z,w),ds;
 Poly f=x^4+x^2*y^2+7*y^8+z^2-w^2;
 NormalFormEquation F;
 F.vars = ringlist(R2)[2];
 F.realCase = 1;
 F.normalFormEquation = f;
 F.modality = 1;
 F.corank = 2;
 F.inertiaIndex = 1;
 F.determinacy = 8;
 F.milnorNumber = milnor(f.value);
 F.parameters = list(list(7*y^8,list(-6,8)));
 F.singularityType = "X[13]";
 F;


example printNormalForm;
example printPolynomialRightEquivalence;
example printRightEquivalenceChainWithPrecision;

tst_status(1);$

