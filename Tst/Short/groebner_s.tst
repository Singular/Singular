LIB "tst.lib";
tst_init();

ring rs = (32003),(x,y,z),(lp(3),C);
ideal gls;
gls[1]=x4+6x3y+2x3z+3x3+12x2y2+12x2yz+57xy5+36xyz+7xz3+29y2+12yz3+25yz2+2yz-3y+3z4+7z3-3z;
gls[2]=x4+16x3+6x2y3+5x2z3+10xy2+11y2z+25y2+7yz2+40yz+z3+15z2+7;
gls[3]=9x3+10y2+5z3+6;
option(prot);
groebner(gls);

//----------------------------------------------
ring r=0,(x,y),lp;
ideal i=x+1;
qring q=std(i);
ideal j=x,y;
groebner(j);
kill r;

tst_status(1);$
