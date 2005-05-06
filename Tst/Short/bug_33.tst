LIB "tst.lib";
tst_init();


ring r=(0,a),(x,y),dp;
minpoly=a2+1;
factorize(x4+y4);

ring r2=(0,v),u,dp;
minpoly=v^4+v^3+v^2+v+1;
poly f=(192*u^6*v^4-240*u^5*v^5+384*u^5*v^3+48*u^4*v^6+96*u^4*v^4-576*u^4*v^2-48*u^3*v^5+144*u^3*v^3);
poly g=(1536*u^6*v^4-768*u^5*v^5-1536*u^5*v^3+96*u^4*v^6+768*u^4*v^4-96*u^3*v^5);
// sollte sein: (16v3+16v2+20v+16)*u5+(16v3-v2)*u4+(4v3+4v2+5v+4)*u3
poly r=gcd(f,g);r;
f-(f/r)*r;
g-(g/r)*r;


f=(-256*u^3*v+128*u^2*v^2-16*u*v^3);
g=(-64*u^3+48*u^2*v^2+32*u^2*v-192*u^2-12*u*v^3-4*u*v^2+48*u*v);
r=gcd(f, g);r; // sollte sein://4*u2+(-v)*u
f-(f/r)*r;
g-(g/r)*r;

//
tst_status(1);$
