ring s=0,(b,c,d,e,f,g,u,v,w,x,y),dp;
poly p=(2*f*x^2*y)*w^5+(-1*e*v*x^2+8*f*u*y^3-8*f*v*x*y^2)*w^4
+(-4*e*u*v*y^2+4*e*v^2*x*y-8*f*u^2*y^2+12*f*u*v*x*y+4*f*v^2*x^2)*w^3
+(4*e*u^2*v*y-6*e*u*v^2*x+2*f*u^3*y+32*f*u*v^2*y^2-32*f*v^3*x*y)*w^2
+(-1*e*u^3*v-8*e*u*v^3*y+8*e*v^4*x-16*f*u^2*v^2*y+24*f*u*v^3*x)*w
+(4*f*u^3*v^2+32*f*u*v^4*y-32*f*v^5*x);
factorize(p);
factorize(p,0);
factorize(p,1);
factorize(p,2);
ring C=(32003,i),x,dp;
minpoly=i2+1;
poly p=x2+1;
factorize(p);
factorize(p,0);
factorize(p,1);
factorize(p,2);
kill s,C;
ring r=3,(x,y,u,s,t),dp;
poly p=
s^85+(t)*s^84+(t^2)*s^83+(-1*t^18)*s^67+(-1*t^19)*s^66+(t^20)*s^65
+(t^21)*s^64+(t^22)*s^63+(-1*t^24)*s^61+(-1*t^25)*s^60+(t^29)*s^56
+(t^38)*s^47+(-1*t^39)*s^46+(-1*t^40)*s^45+(-1*t^42)*s^43+(-1*t^43)*s^42
+(-1*t^45)*s^40+(-1*t^46)*s^39+(t^47)*s^38+(t^56)*s^29+(-1*t^60)*s^25
+(-1*t^61)*s^24+(t^63)*s^22+(t^64)*s^21+(t^65)*s^20+(-1*t^66)*s^19
+(-1*t^67)*s^18+(t^83)*s^2+(t^84)*s+(t^85);
factorize(p);
LIB "tst.lib";tst_status(1);$

