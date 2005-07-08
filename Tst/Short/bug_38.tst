LIB "tst.lib";
tst_init();
ring R = (0, a), (u,v,w), dp; minpoly = a^2 + 4/27;
poly p = 
u^4+3*u^3*v+25/4*u^2*v^2+6*u*v^3+4*v^4+2*u^3*w+8*u^2*v*w+23/2*u*v^2*w+10*v^3*w+u
^2*w^2+5*u*v*w^2+25/4*v^2*w^2-1/3*u^2+1/3*u*v+1/3*u*w+1/3*v*w+3/4*w^2+1/9;
list L=factorize(p);
L;
L[1][2]*L[1][3]-p;
//
tst_status(1);$
