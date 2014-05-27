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



tst_status(1); $
