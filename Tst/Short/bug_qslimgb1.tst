LIB "tst.lib";
tst_init();


// std and groebner in nc-qrings:

option(redSB);
ring r = (0),(x,y,z),(dp(3),C);
qring q=std(z2);
ideal i = 169y21+151xyz10+x2y,3z24+xz14+6x2y4,2y20z10+y10z20+5xy10z10+11x3;
ideal j = groebner(i);
ideal j1= std(i);
j;
j1;


tst_status(1);$
