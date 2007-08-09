LIB "tst.lib";
tst_init();

// std and groebner in nc-qrings

LIB "nctools.lib";
option(redSB);
ring r = (0),(x,y,z),(dp(3),C);
def br = SuperCommutative(3,3);
setring br;
ideal i = 169y21+151xyz10+x2y,3z24+xz14+6x2y4,2y20z10+y10z20+5xy10z10+11x3;
ideal j = groebner(i);
ideal j1= std(i);
j;
j1;


tst_status(1);$
