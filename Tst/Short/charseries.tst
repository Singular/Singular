LIB "tst.lib";

tst_init();

ring r=0,(x1,x2,x3,x4,x5,y),lp;
poly f1=((y+x1*x3+x2+x1)*(y-2*x2^2+x3^2+1)*(y^2+x1*x2+x3));
ideal I=(x1^2-x2*x1+x3*x1-x1-x2,x1^2-x3*x1+x1-x2^2-x3*x2-x2+x3^2,-1+x1^2+x3*x1+x1-x2^2-x2+x3^2-x3);
ideal J=I,f1;
char_series (J);

kill r;

//ring r=0,(x4,x3,x2,x1,x5,y),dp;
//poly f2=((y+x1)*(y-2*x4)*(y+x2+x3));
//ideal I= x1^2+x3*x1-x1*x4+x2^2-x2+x3*x4-x4^2-x4,x1^2+x2*x1-x1*x4+x2^2+x3*x2+x2+x4^2-x4
//          , 1+x1*x2-x1*x3+x1*x4+x1+x2^2-x3*x2+x2*x4-x2+x3^2+x3*x4-x4^2 , x1^2+x1*x2+x3*x1+x1*x4-x2^2+x3*x2-x2+x3^2-x3*x4-x4^2+x4;
//ideal J=I,f2;
//char_series (J);

//kill r;

//ring r=0,(x1,x2,x3,x4,x5,y),dp;
//poly f3=((y-2*x4^2+x3*x1+x2+1)*(y+x2^2+x3*x4+x1*x3+2));
//ideal I=-1-x1^2+x3*x1+x2^2-x3*x2+x3^2-x3*x4+x4^2+x4 , 1+x2*x1+x3*x1+x1-x3*x2-x2*x4+x2-x3^2-x3*x4-x3 , 1+x1*x3+x1*x4+x1+x2^2+x2*x4-x2-x3-x4^2 , 
//     x1^2+x1*x2+x3*x1+x1*x4-x1-x2*x4-x3^2+x3*x4-x3+x4^2+x4;
//ideal J=I,f3;
//char_series (J);

//kill r;

ring r=0,(x1,x2,x3,x4,x5,y),lp;
poly f4= ((y^2+(x1-x2)*y+x2)*(y+x2)*(y-x4*x3));
ideal I= x1^2+1, x2^2+x1, x3^2+x2, x4^2+x3;
ideal J= I,f4;
char_series (J);

kill r;

ring r=0,(x1,x2,x3,x4,x5,y),lp;
poly f5= ((y^2+(x1-x2)*y+x2)^4*(y+x2)^3*(y-x4*x3));
ideal I=x1^2+1, x2^2+x1, x3^2+x2+x1, x4^2+x3+x2+x1;
ideal J= I,f5;
char_series (J);

kill r;

ring r=0,(x1,x2,x3,x4,x5,y),lp;
poly f6= ((y+x1*x3+x2+1)*(y+x3*x4+x1*x3)*(y^3+x3*x4+x1*x3));
ideal I=x1^2+1, x2^2+x1, x3^2+x2, x4^2+x3+x2;
ideal K=I,f6;
char_series (K);

kill r;

ring r=0,(x1,x2,x3,x4,x5,y),lp;
poly f6= (y+x1*x3+x2+1)*(y+x1*x3+x2+1)*(y+x3*x4+2*x1*x3);
ideal I=x1^2+1, x2^2+x1, x3^2+x2, x4^2+x3+x2;
ideal K=I,f6;
char_series (K);

kill r;

ring r=0,(t,x1,x2,x3,x4,x5,y),lp;
poly f6= (y+x1*x3+x2+t)*(y+x1*x3+x2+t)*(y+x3*x4+t*x1*x3);
ideal I=x1^2+t, x2^2+x1, x3^2+x2, x4^2+x3+x2;
ideal K=I,f6;
char_series (K);

kill r;

ring r=0,(t,x1,x2,x3,x4,x5,y),lp;
poly f6= (y+x1*x3+x2+t)*(y+x3*x4+t*x1*x3);
ideal I=x1^2+t, x2^2+x1, x3^2+x2, x4^2+x3+x2;
ideal K=I,f6;
char_series (K);

kill r;

ring r=3,(x1,x2,x3,x4,x5,y),lp;
poly f6= ((y+x1*x3+x2+1)*(y+x3*x4+x1*x3)*(y^3+x3*x4+x1*x3));
ideal I=x1^2+1, x2^2+x1+1, x3^2+x2+1, x4^2+x3+x2+2;
ideal K=I,f6;
char_series (K);

tst_status(1);$
