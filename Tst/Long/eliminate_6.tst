LIB "tst.lib"; tst_init(); option(prot);

ring rh15=32003,(w,x,y,z),dp;
poly p1= x2y *(47x5y7z3+28xy5z8+63+91x5y3z7);
poly p2= xyz *(57y6+21x2yz9+51y2z2+15x2z4);
poly p3= xy4z *(74y+32x6z7+53x5y2z+17x2y3z);
poly p4= y3z *(21x2z6+32x10y6z5+23x5y5z7+27y2);
poly p5= xz *(36y2z2+81x9y10+19x2y5z4+79x4z6);
ideal j= p1,p2,p3,p4,p5;
ideal i=homog(j,w);
eliminate(i,yz);

tst_status(1);$
