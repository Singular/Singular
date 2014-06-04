LIB "tst.lib";

tst_init();

ring r=0,(x4,x3,x2,x1,x5,y),dp;
poly f2=((y+x1)*(y-2*x4)*(y+x2+x3));
ideal I= x1^2+x3*x1-x1*x4+x2^2-x2+x3*x4-x4^2-x4,x1^2+x2*x1-x1*x4+x2^2+x3*x2+x2+x4^2-x4
          , 1+x1*x2-x1*x3+x1*x4+x1+x2^2-x3*x2+x2*x4-x2+x3^2+x3*x4-x4^2 , x1^2+x1*x2+x3*x1+x1*x4-x2^2+x3*x2-x2+x3^2-x3*x4-x4^2+x4;
ideal J=I,f2;
char_series (J);

kill r;

ring r=0,(x1,x2,x3,x4,x5,y),dp;
poly f3=((y-2*x4^2+x3*x1+x2+1)*(y+x2^2+x3*x4+x1*x3+2));
ideal I=-1-x1^2+x3*x1+x2^2-x3*x2+x3^2-x3*x4+x4^2+x4 , 1+x2*x1+x3*x1+x1-x3*x2-x2*x4+x2-x3^2-x3*x4-x3 , 1+x1*x3+x1*x4+x1+x2^2+x2*x4-x2-x3-x4^2 , 
     x1^2+x1*x2+x3*x1+x1*x4-x1-x2*x4-x3^2+x3*x4-x3+x4^2+x4;
ideal J=I,f3;
char_series (J);

tst_status(1);$
