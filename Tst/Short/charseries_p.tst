LIB "tst.lib";
tst_init();

ring r=2,(t,b,a,y),dp;
ideal i=a2+tb,b4+b2+t;
poly f=(y2+tb)*(y+b3+b2)*(y2+tb+t2);
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,a,b,y),dp;
ideal i=a2+tb,b4+b2+t;
poly f=(y2+tb)*(y+b3+b2)*(y2+tb+t2);
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,a,b,c,y),dp;
ideal i=a2+tb,c2+c+b,b4+b2+t;
poly f=(y2+tb)*(y2+b3+c+b)*(y+c+t)*(y2+tb+t2);
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,a,b,c,y),dp;
ideal i=a2+tb,c2+c+b,b4+b2+t;
poly f=(y2+a)^8*(y2+b3+c+b)*(y+c+t)*(y2+tb+t2);
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,a,b,c,y),dp;
ideal i=a2+tb,c2+c+b,b4+b2+t;
poly f=(y3+a)^8*(y2+b3+c+b)*(y+c+t)*(y2+tb+t2);
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,b,a,c,y),dp;
ideal i=a2+tb,c2+c+b,b4+b2+t;
poly f=(y2+tb)*(y2+b3+c+b)*(y+c+t)*(y2+tb+t2);
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,b,c,a,y),dp;
ideal i=a2+tb,c2+c+b,b4+b2+t;
poly f=(y2+tb)*(y2+b3+c+b)*(y+c+t)*(y2+tb+t2);
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,a,c,b,y),dp;
ideal i=a2+tb,c2+c+b,b4+b2+t;
poly f=(y2+tb)*(y2+b3+c+b)*(y+c+t)*(y2+tb+t2);
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,c,a,b,y),dp;
ideal i=a2+tb,c2+c+b,b4+b2+t;
poly f=(y2+tb)*(y2+b3+c+b)*(y+c+t)*(y2+tb+t2);
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,c,b,a,y),dp;
ideal i=a2+tb,c2+c+b,b4+b2+t;
poly f=(y2+tb)*(y2+b3+c+b)*(y+c+t)*(y2+tb+t2);
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,u,a,b,c,x,y,z),dp;
ideal i=a2+tb, b4+b2+c, c2+tu;
poly f=x8+t2x4+t4c;
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,u,a,c,b,x,y,z),dp;
ideal i=a2+tb, b4+b2+c, c2+tu;
poly f=x8+t2x4+t4c;
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,u,c,a,b,x,y,z),dp;
ideal i=a2+tb, b4+b2+c, c2+tu;
poly f=x8+t2x4+t4c;
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,u,b,a,c,x,y,z),dp;
ideal i=a2+tb, b4+b2+c, c2+tu;
poly f=x8+t2x4+t4c;
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,u,b,c,a,x,y,z),dp;
ideal i=a2+tb, b4+b2+c, c2+tu;
poly f=x8+t2x4+t4c;
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,u,c,b,a,x,y,z),dp;
ideal i=a2+tb, b4+b2+c, c2+tu;
poly f=x8+t2x4+t4c;
ideal I=i,f;
char_series (I);

kill r;

ring r=2,(t,u,a,b,x),dp;
ideal i=a2+t, b4+u;
poly f=x8+(t+u)*x4+tu;
ideal J= i,f;
char_series (J);

kill r;

ring r=2,(t,u,b,a,x),dp;
ideal i=a2+t, b4+u;
poly f=x8+(t+u)*x4+tu;
ideal J= i,f;
char_series (J);

kill r;

ring r=3,(x1,x2,x3,x4,x5,y),lp;
poly f6= ((y+x1*x3+x2+1)*(y+x3*x4+x1*x3)*(y^3+x3*x4+x1*x3));
ideal I=x1^2+1, x2^2+x1+1, x3^2+x2+1, x4^2+x3+x2+2;
ideal K=I,f6;
char_series (K);

kill r;

ring r=32003,(a,b,c,d,e,f,g,h),dp;
ideal i= -1*a-1*d+e+f+h,
-2*a^2+2*a*d+2*c*e+2*b*f-1*a-1,
3*a^3-3*a^2*d+3*c^2*e+3*b^2*f+3*a^2+4*a-1*d,
6*a^3-6*a^2*d+6*b*e*g+6*a^2-3*a*d+4*a-1*d,
-4*a^4+4*a^3*d+4*c^3*e+4*b^3*f-6*a^3-10*a^2+4*a*d-1*a-1,
-8*a^4+8*a^3*d+8*b*c*e*g-12*a^3+4*a^2*d-14*a^2+4*a*d-3*a-1,
-12*a^4+12*a^3*d+12*b^2*e*g-18*a^3+12*a^2*d-14*a^2+8*a*d-1*a-1,
24*a^4-24*a^3*d+36*a^3-24*a^2*d+26*a^2-8*a*d+7*a+1;
char_series (i);

kill r;

ring r=31991,(a,b,c,d),dp;
ideal i= a*b*c-b*c*d-c*d*a-d*a*b, a^3-b^3-a^3*b^3+c^3+d^3, a*b+b*c+c*d+d*a,
a^2, c*a*b+a*b+a+b;
char_series (i);

kill r;

ring r=31991,(x,y,z),dp;
ideal i= x^3-y^2, x^3-z^2, y^3-z^2;
char_series (i);

kill r;

ring r=31991,(a,b,c,d,e,f),dp;
ideal i=a*b*c+a*b+a*e,a-b*d*e,d*e*f+f,f-e,a-a*b;
char_series (i);

kill r;

ring r=31991,(x,y,z),dp;
ideal i=x^3-x^2+y^2, x-y^3, x*y*z+z^3;
char_series (i);

kill r;

ring r=31991,(x,y,z,t),dp;
ideal i= t^2+x*t^2-y*t-x*y*t+x*y+3*y, x*t+z-x*y, z*t-2*y^2-x*y-1;
char_series (i);

kill r;

ring r=31991,(d,c,b,a),dp;
ideal i= a*b*c-b*c*d-c*d*a-d*a*b, a^3-b^3-a^3*b^3+c^3+d^3,
a*b+b*c+c*d+d*a, a^2, c*a*b+a*b+a+b;
char_series (i);

kill r;

ring r=31991, (a,b,c,d,e,f,g,h,k,j), dp;
ideal I= -j+a^2*b^2*c^2*d^2*e+a^2*b^2*c^2*d^2*f+a^2*b^2*c^2*d*e^2+2*a^2*b^2*c^2*d*e*f+a^2*b^2*c^2*d*f^2+a^2*b^2*c^2*e^2*f+a^2*b^2*c^2*e*f^2+a^2*d*h-b^2*f*g*c^2*e*k*g-h-k,
2*a*d*h+2*a*b^2*c^2*e*f^2+2*a*b^2*c^2*d*f^2+2*a*b^2*c^2*e^2*f+4*a*b^2*c^2*d*e*f+2*a*b^2*c^2*d^2*f+2*a*b^2*c^2*d*e^2+2*a*b^2*c^2*d^2*e,
-2*b*c^2*e*f*g^2*k+2*a^2*b*c^2*e*f^2+2*a^2*b*c^2*d*f^2+2*a^2*b*c^2*e^2*f+4*a^2*b*c^2*d*e*f+2*a^2*b*c^2*d^2*f+2*a^2*b*c^2*d*e^2+2*a^2*b*c^2*d^2*e,
-2*b^2*c*e*f*g^2*k+2*a^2*b^2*c*e*f^2+2*a^2*b^2*c*d*f^2+2*a^2*b^2*c*e^2*f+4*a^2*b^2*c*d*e*f+2*a^2*b^2*c*d^2*f+2*a^2*b^2*c*d*e^2+2*a^2*b^2*c*d^2*e,
a^2*h+a^2*b^2*c^2*f^2+2*a^2*b^2*c^2*e*f+2*a^2*b^2*c^2*d*f+a^2*b^2*c^2*e^2+2*a^2*b^2*c^2*d*e,
-b^2*c^2*f*g^2*k+a^2*b^2*c^2*f^2+2*a^2*b^2*c^2*e*f+2*a^2*b^2*c^2*d*f+2*a^2*b^2*c^2*d*e+a^2*b^2*c^2*d^2,
-b^2*c^2*e*g^2*k+2*a^2*b^2*c^2*e*f+2*a^2*b^2*c^2*d*f+a^2*b^2*c^2*e^2+2*a^2*b^2*c^2*d*e+a^2*b^2*c^2*d^2,
-2*b^2*c^2*e*f*g*k,
a^2*d-1,
-b^2*c^2*e*f*g^2-1;
char_series (I);

kill r;

ring r= 31991, (a,b,c,d,e,f,g,h,i,j),dp;
ideal I=
-6*b*c^2*e*f*g^2+2*a^2*b*c^2*e*f^2+2*a^2*b*c^2*d*f^2+2*a^2*b*c^2*e^2*f+4*a^2*b*c^2*d*e*f+2*a^2*b*c^2*d^2*f+2*a^2*b*c^2*d*e^2+2*a^2*b*c^2*d^2*e,
-j-b^2*c^2*e*f*g^2-1+a^2*d*h-h+a^2*b^2*c^2*e*f^2+a^2*b^2*c^2*d*f^2+a^2*b^2*c^2*e^2*f+2*a^2*b^2*c^2*d*e*f+a^2*b^2*c^2*d^2*f+a^2*b^2*c^2*d*e^2+a^2*b^2*c^2*d^2*e,
-8*b^2*c*e*f*g^2+2*a^2*b^2*c*e*f^2+2*a^2*b^2*c*d*f^2+2*a^2*b^2*c*e^2*f+4*a^2*b^2*c*d*e*f+2*a^2*b^2*c*d^2*f+2*a^2*b^2*c*d*e^2+2*a^2*b^2*c*d^2*e,
-16*b^2*c^2*e*f*g,
-7*b^2*c^2*e*g^2+2*a^2*b^2*c^2*e*f+2*a^2*b^2*c^2*d*f+a^2*b^2*c^2*e^2+2*a^2*b^2*c^2*d*e+a^2*b^2*c^2*d^2,
-6*b^2*c^2*f*g^2+a^2*b^2*c^2*f^2+2*a^2*b^2*c^2*e*f+2*a^2*b^2*c^2*d*f+2*a^2*b^2*c^2*d*e+a^2*b^2*c^2*d^2,
a^2*d-1,
-b^2*c^2*e*f*g^2-1,
2*a*d*h+2*a*b^2*c^2*e*f^2+2*a*b^2*c^2*d*f^2+2*a*b^2*c^2*e^2*f+4*a*b^2*c^2*d*e*f+2*a*b^2*c^2*d^2*f+2*a*b^2*c^2*d*e^2+2*a*b^2*c^2*d^2*e,
a^2*h+a^2*b^2*c^2*f^2+2*a^2*b^2*c^2*e*f+2*a^2*b^2*c^2*d*f+a^2*b^2*c^2*e^2+2*a^2*b^2*c^2*d*e;
char_series (I);

kill r;

ring r=31991,(a,b,c,x,y,z),dp;
ideal I=a*(x+y+z)-(x-y-z),
b*(x+z)-(x+y-z),
c*(x-y+z)-(x+y);
char_series (I);

kill r;

ring rr=31991,(a,b,c,d,e,f,g,h,j,k,l,m,n,o,p,q,r),dp;
ideal I=d*k,
e*g+d*j+c*k+l,
b*h,
e*l,
b*f+b*g+a*h+d*h+b*k+h+o,
a*f+d*f+a*g+d*g+c*h+e*h+b*j+a*k+d*k+b*l+f+g+k+m+n+q,
c*f+e*f+c*g+e*g+a*j+d*j+c*k+e*k+a*l+d*l+j+l+p+r-1,
c*j+e*j+c*l+e*l,
e*j+c*l+2*e*l,
e*f+e*g+d*j+c*k+2*e*k+a*l+2*d*l+l+r,
d*f+d*g+e*h+a*k+2*d*k+b*l+k+q,
d*h+b*k,
e*k+d*l,
2*c*j+e*j+c*l,
c*f+2*c*g+e*g+a*j+d*j+c*k+2*j+l+p,
a*g+d*g+c*h+b*j+f+2*g+k+n,
b*g+h,
e*j+c*l,
d*g+k;
char_series (I);

kill rr;

ring rr=31991,(r,s,t,u,v,w,x,y,z),dp;
ideal i=
s*u+r*v,
t*u+r*w,
t*v+s*w,
s*x+r*y,
t*x+r*z,
t*y+s*z,
v*x+u*y,
w*x+u*z,
w*y+v*z;
char_series (i);

kill rr;

ring r=31991,(a,b,c,d,e,f),dp;
ideal i=
a*d*e*f+3/2*b*e^2*f-1/2*c*e*f^2,
    a*d^2*f+5/4*b*d*e*f+1/4*c*d*f^2,
    a*d^2*e+3/4*b*d*e^2+7/4*c*d*e*f,
    a*c*d*e+3/2*b*c*e^2-1/2*c^2*e*f,
    a*c*d^2+5/4*b*c*d*e+1/4*c^2*d*f,
    a*b*d*f+3/2*b^2*e*f-1/2*b*c*f^2,
    a*b*d*e+3/4*b^2*e^2-a*c*d*f+1/2*b*c*e*f-1/4*c^2*f^2,
    a*b*d^2+3/4*b^2*d*e+7/4*b*c*d*f,
    a*b*c*d+3/2*b^2*c*e-1/2*b*c^2*f,
    a^2*d*f+5/4*a*b*e*f+1/4*a*c*f^2,
    a^2*d*e+3/4*a*b*e^2+7/4*a*c*e*f,
    a^2*d^2-9/16*b^2*e^2+2*a*c*d*f-9/8*b*c*e*f+7/16*c^2*f^2,
    a^2*c*d+5/4*a*b*c*e+1/4*a*c^2*f,
    a^2*b*d+3/4*a*b^2*e+7/4*a*b*c*f,
    b*c^3*d+1/4*c*d^3*e,
    b^2*c^2*e-1/3*b*c^3*f+2/3*c*d^2*e*f,
    b^2*c^2*d-1/2*a*d^4-3/8*b*d^3*e-1/8*c*d^3*f,
    b^3*c*e-1/3*b^2*c^2*f+2/3*b*d^2*e*f,
    b^3*c*d+3/4*b*d^3*f,
    a*c^3*e-1/3*c*d*e^3,
    a*c^3*d-3/4*b*c^3*e-1/2*c*d^2*e^2+1/4*c^4*f,
    a*b*c^2*f-c*d*e*f^2,
    a*b*c^2*e-c*d*e^2*f,
    a*b^2*c*f-b*d*e*f^2,
    a*b^3*f-3*b*d*f^3,
    a*b^3*d+3/4*b^4*e-1/4*b^3*c*f-3/2*b*d^2*f^2,
    a^2*c^2*e-2/3*a*d*e^3-1/2*b*e^4+1/6*c*e^3*f,
    a^2*b*c*f+3/2*b*e^2*f^2-1/2*c*e*f^3,
    a^2*b*c*e+3/2*b*e^3*f-1/2*c*e^2*f^2,
    a^2*b^2*f-2*a*d*f^3+3/2*b*e*f^3-1/2*c*f^4,
    a^3*c*e+4/3*a*e^3*f,
    a^3*b*f+4*a*e*f^3,
    a^4*d+3/4*a^3*b*e+1/4*a^3*c*f-2*a*e^2*f^2;
char_series (i);

kill r;

ring r=31991,(x,y),dp;
ideal i= x^2, x*y;
char_series (i);

tst_status(1); $
