LIB "tst.lib";
tst_init();

// too many identical syzygies
LIB "nctools.lib";
ring A=0,(z,x,y,dz,dx,dy),dp;
def S=Weyl();
setring S;
option(prot);
matrix m[1][6] = x^2*y^8+2*x*y^9+y^10+2*x^5*y^4+2*x^4*y^5+x^8,-z*x*y^8-z*y^9-5*z*x^4*y^4-4*z*x^3*y^5-4*z*x^7,z^2*y^8+8*z^2*x^3*y^4+z*y^8+16*z^2*x^6-4*z*x^3*y^4-12*z*x^2*y^5+4*z*x^6,4*z^2*x*y^7+5*z^2*y^8+16*z^2*x^4*y^3+20*z^2*x^3*y^4+z*y^8+12*z*x^4*y^3+20*z*x^3*y^4,-4*z*x^2*y^7-9*z*x*y^8-5*z*y^9-4*z*x^5*y^3-5*z*x^4*y^4,16*z^2*x^2*y^6+40*z^2*x*y^7+25*z^2*y^8+4*z*x^2*y^6+8*z*x*y^7+5*z*y^8-12*z*x^5*y^2-20*z*x^4*y^3;
matrix s=syz(m);
listvar();
lead(module(s));

tst_status(1);$
