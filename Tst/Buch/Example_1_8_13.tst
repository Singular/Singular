LIB "tst.lib";
tst_init();

ring A=0,(x,y,z),dp;
ideal I1=x,y;
ideal I2=y2,z;
quotient(I1,I2);      

ideal J1=intersect(I1,ideal(I2[1]));
ideal J2=intersect(I1,ideal(I2[2]));
J1;
J2; 

ideal K1=J1[1]/I2[1];
ideal K2=J2[1]/I2[2],J2[2]/I2[2];
intersect(K1,K2);

tst_status(1);$
