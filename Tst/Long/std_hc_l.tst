LIB "tst.lib";
tst_init();
option(prot);
ring R = 0,(x,y,z),ds;
poly F = xyz*(x+y+z)^2 +(x+y+z)^3 +x^15+y^15+z^15;
ideal I = jacob(F);
ideal J=std(I);
//-----------------------------------------
F  = x3y3+x5y2+2x2y5+x2y2z3+xy7+z9+y13+x25;
I = jacob(F),F;
J=std(I);
//----------------------------------------
LIB"random.lib";
option(prot);
ring R2=0,(x,y,z,w),ds;
system("random",100);       //um mit festem random Ideal zu rechnen
ideal I=randomid(maxideal(5)+maxideal(7)+maxideal(10),5,99);
ideal J=std(I);

tst_status(1);$
