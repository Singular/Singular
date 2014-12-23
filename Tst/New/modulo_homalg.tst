LIB "tst.lib"; tst_init();

ring R = 0,(x,y,z), (dp, C);

module M1;
M1[30]=x^4*gen(5)+x^3*z*gen(10)+x^2*z*gen(20)-x*z*gen(25);
M1[1]=x*y*gen(1)+y*z*gen(6)+z*gen(11);
M1[2]=x*y*gen(2)+y*z*gen(7)+z*gen(12);
M1[3]=x*y*gen(3)+y*z*gen(8)+z*gen(13);
M1[4]=x*y*gen(4)+y*z*gen(9)+z*gen(14);
M1[5]=x*y*gen(5)+y*z*gen(10)+z*gen(15);
M1[6]=x^2*gen(21)+x*y*gen(11)-y^2*gen(16)-gen(21);
M1[7]=x^2*gen(22)+x*y*gen(12)-y^2*gen(17)-gen(22);
M1[8]=x^2*gen(23)+x*y*gen(13)-y^2*gen(18)-gen(23);
M1[9]=x^2*gen(24)+x*y*gen(14)-y^2*gen(19)-gen(24);
M1[10]=x^2*gen(25)+x*y*gen(15)-y^2*gen(20)-gen(25);
M1[11]=x^2*z*gen(11)-x*y*z*gen(16)+y*z*gen(21);
M1[12]=x^2*z*gen(12)-x*y*z*gen(17)+y*z*gen(22);
M1[13]=x^2*z*gen(13)-x*y*z*gen(18)+y*z*gen(23);
M1[14]=x^2*z*gen(14)-x*y*z*gen(19)+y*z*gen(24);
M1[15]=x^2*z*gen(15)-x*y*z*gen(20)+y*z*gen(25);
M1[16]=x^2*y*gen(11)-x*y^2*gen(16)-x^2*gen(11)+x*y*gen(16)+y^2*gen(21)-y*gen(21);
M1[17]=x^2*y*gen(12)-x*y^2*gen(17)-x^2*gen(12)+x*y*gen(17)+y^2*gen(22)-y*gen(22);
M1[18]=x^2*y*gen(13)-x*y^2*gen(18)-x^2*gen(13)+x*y*gen(18)+y^2*gen(23)-y*gen(23);
M1[19]=x^2*y*gen(14)-x*y^2*gen(19)-x^2*gen(14)+x*y*gen(19)+y^2*gen(24)-y*gen(24);
M1[20]=x^2*y*gen(15)-x*y^2*gen(20)-x^2*gen(15)+x*y*gen(20)+y^2*gen(25)-y*gen(25);
M1[21]=x^3*z*gen(1)+x^2*z^2*gen(6)+x*z^2*gen(16)-z^2*gen(21);
M1[22]=x^3*z*gen(2)+x^2*z^2*gen(7)+x*z^2*gen(17)-z^2*gen(22);
M1[23]=x^3*z*gen(3)+x^2*z^2*gen(8)+x*z^2*gen(18)-z^2*gen(23);
M1[24]=x^3*z*gen(4)+x^2*z^2*gen(9)+x*z^2*gen(19)-z^2*gen(24);
M1[25]=x^3*z*gen(5)+x^2*z^2*gen(10)+x*z^2*gen(20)-z^2*gen(25);
M1[26]=x^4*gen(1)+x^3*z*gen(6)+x^2*z*gen(16)-x*z*gen(21);
M1[27]=x^4*gen(2)+x^3*z*gen(7)+x^2*z*gen(17)-x*z*gen(22);
M1[28]=x^4*gen(3)+x^3*z*gen(8)+x^2*z*gen(18)-x*z*gen(23);
M1[29]=x^4*gen(4)+x^3*z*gen(9)+x^2*z*gen(19)-x*z*gen(24);

module M2;
M2[30]=x^4*gen(21)+x^3*z*gen(22)+x^2*z*gen(24)-x*z*gen(25);
M2[1]=x*y*gen(1)+y*z*gen(2)+z*gen(3);
M2[2]=x^2*gen(5)+x*y*gen(3)-y^2*gen(4)-gen(5);
M2[3]=x^2*z*gen(3)-x*y*z*gen(4)+y*z*gen(5);
M2[4]=x^2*y*gen(3)-x*y^2*gen(4)-x^2*gen(3)+x*y*gen(4)+y^2*gen(5)-y*gen(5);
M2[5]=x^3*z*gen(1)+x^2*z^2*gen(2)+x*z^2*gen(4)-z^2*gen(5);
M2[6]=x^4*gen(1)+x^3*z*gen(2)+x^2*z*gen(4)-x*z*gen(5);
M2[7]=x*y*gen(6)+y*z*gen(7)+z*gen(8);
M2[8]=x^2*gen(10)+x*y*gen(8)-y^2*gen(9)-gen(10);
M2[9]=x^2*z*gen(8)-x*y*z*gen(9)+y*z*gen(10);
M2[10]=x^2*y*gen(8)-x*y^2*gen(9)-x^2*gen(8)+x*y*gen(9)+y^2*gen(10)-y*gen(10);
M2[11]=x^3*z*gen(6)+x^2*z^2*gen(7)+x*z^2*gen(9)-z^2*gen(10);
M2[12]=x^4*gen(6)+x^3*z*gen(7)+x^2*z*gen(9)-x*z*gen(10);
M2[13]=x*y*gen(11)+y*z*gen(12)+z*gen(13);
M2[14]=x^2*gen(15)+x*y*gen(13)-y^2*gen(14)-gen(15);
M2[15]=x^2*z*gen(13)-x*y*z*gen(14)+y*z*gen(15);
M2[16]=x^2*y*gen(13)-x*y^2*gen(14)-x^2*gen(13)+x*y*gen(14)+y^2*gen(15)-y*gen(15);
M2[17]=x^3*z*gen(11)+x^2*z^2*gen(12)+x*z^2*gen(14)-z^2*gen(15);
M2[18]=x^4*gen(11)+x^3*z*gen(12)+x^2*z*gen(14)-x*z*gen(15);
M2[19]=x*y*gen(16)+y*z*gen(17)+z*gen(18);
M2[20]=x^2*gen(20)+x*y*gen(18)-y^2*gen(19)-gen(20);
M2[21]=x^2*z*gen(18)-x*y*z*gen(19)+y*z*gen(20);
M2[22]=x^2*y*gen(18)-x*y^2*gen(19)-x^2*gen(18)+x*y*gen(19)+y^2*gen(20)-y*gen(20);
M2[23]=x^3*z*gen(16)+x^2*z^2*gen(17)+x*z^2*gen(19)-z^2*gen(20);
M2[24]=x^4*gen(16)+x^3*z*gen(17)+x^2*z*gen(19)-x*z*gen(20);
M2[25]=x*y*gen(21)+y*z*gen(22)+z*gen(23);
M2[26]=x^2*gen(25)+x*y*gen(23)-y^2*gen(24)-gen(25);
M2[27]=x^2*z*gen(23)-x*y*z*gen(24)+y*z*gen(25);
M2[28]=x^2*y*gen(23)-x*y^2*gen(24)-x^2*gen(23)+x*y*gen(24)+y^2*gen(25)-y*gen(25);
M2[29]=x^3*z*gen(21)+x^2*z^2*gen(22)+x*z^2*gen(24)-z^2*gen(25);

R;
M1;
M2;

modulo(M1,M2);

tst_status(1);$
