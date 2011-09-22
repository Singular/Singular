LIB "tst.lib"; tst_init();
LIB "latex.lib";
ring r0=0,(x,y,z),dp;
poly f = -1x^2 + 2;
texpoly("",f);
ring rr= real,(x,y,z),dp;
texpoly("",2x2y23z);
ring r7= 7,(x,y,z),dp;
poly f = 2x2y23z;
texpoly("",f);
ring rab =(0,a,b),(x,y,z),dp;
poly f = (-2a2 +b3 -2)/a * x2y4z5 + (a2+1)*x + a+1;
f;
texpoly("",f);
texpoly("",1/(a2+2)*x+2/b);
tst_status(1);$
