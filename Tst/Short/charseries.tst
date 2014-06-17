LIB "tst.lib";

tst_init();

ring r=0,(x1,x2,x3,x4,x5,y),lp;
poly f1=((y+x1*x3+x2+x1)*(y-2*x2^2+x3^2+1)*(y^2+x1*x2+x3));
ideal I=(x1^2-x2*x1+x3*x1-x1-x2,x1^2-x3*x1+x1-x2^2-x3*x2-x2+x3^2,-1+x1^2+x3*x1+x1-x2^2-x2+x3^2-x3);
ideal J=I,f1;
char_series (J);

kill r;

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

ring r=0,(a,b,c,d,e,f,g,h,j),dp;
ideal i= d*b-e*a, c*e-f*b, g*e-h*d, h*f-j*e;
char_series (i);

kill r;

// examples from J. Kroeker
ring r=0,(x,y,z),dp;
ideal I = -3*y^2-3, -3*x^2+1, -x*y+2*z^2;
char_series (I);

kill r;

ring r=0,(z,y,x),dp;
ideal I = -3*y^2-3, -3*x^2+1, -x*y+2*z^2;
char_series (I);


tst_status(1);$
