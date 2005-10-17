LIB "tst.lib";
tst_init();
ring R = (0, a), (u,v,w), dp; minpoly = a^2 + 4/27;
poly p = 
u^4+3*u^3*v+25/4*u^2*v^2+6*u*v^3+4*v^4+2*u^3*w+8*u^2*v*w+23/2*u*v^2*w+10*v^3*w+u
^2*w^2+5*u*v*w^2+25/4*v^2*w^2-1/3*u^2+1/3*u*v+1/3*u*w+1/3*v*w+3/4*w^2+1/9;
list L=factorize(p);
L;
L[1][2]*L[1][3]-p;

//-------------------------------------------

ring S=(0,a),(y),dp;
minpoly = 8a4+188a2-2217;
poly f=4915089/64-104199/8*y2-2*y4+47*y6+y8;
// all real factors have exponents 2,
// S 3-0-0 had some factors twice
//[1]:
//   _[1]=1
//   _[2]=y+(-a)
//   _[3]=y+(a)
//   _[4]=y2+(a2+47/2)
//   _[5]=y+(a)
//   _[6]=y+(-a)
//[2]:
//   1,1,1,2,1,1

factorize(f);
//
tst_status(1);$
