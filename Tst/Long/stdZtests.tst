LIB "tst.lib";

    ring rng = (integer),(x,y,z),(lp(2),lp(1),C);
int trial = 0;
system("random", 1181381080);
ideal I = 124*x^2+99*x*y,58*x*y+31*y+120*z,65*x^2*z+54*y^2*z+58*y*z+95*y;
ideal J = 52*x*y^3*z+102*y^2,4*x*y^2*z+19*y^3*z-103*y-30;
       ideal gI =  std(I);
       ideal ggI = std(gI);
    size(gI);size(ggI); 
    ideal gJ =  std(J);
       ideal ggJ = std(gJ);
    size(gJ);size(ggJ); 

tst_status(1);
$

