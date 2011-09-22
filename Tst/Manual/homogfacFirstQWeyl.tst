LIB "tst.lib"; tst_init();
LIB "ncfactor.lib";
ring R = (0,q),(x,d),dp;
def r = nc_algebra (q,1);
setring(r);
poly h = q^25*x^10*d^10+q^16*(q^4+q^3+q^2+q+1)^2*x^9*d^9+
q^9*(q^13+3*q^12+7*q^11+13*q^10+20*q^9+26*q^8+30*q^7+
31*q^6+26*q^5+20*q^4+13*q^3+7*q^2+3*q+1)*x^8*d^8+
q^4*(q^9+2*q^8+4*q^7+6*q^6+7*q^5+8*q^4+6*q^3+
4*q^2+2q+1)*(q^4+q^3+q^2+q+1)*(q^2+q+1)*x^7*d^7+
q*(q^2+q+1)*(q^5+2*q^4+2*q^3+3*q^2+2*q+1)*(q^4+q^3+q^2+q+1)*(q^2+1)*(q+1)*x^6*d^6+
(q^10+5*q^9+12*q^8+21*q^7+29*q^6+33*q^5+31*q^4+24*q^3+15*q^2+7*q+12)*x^5*d^5+
6*x^3*d^3+24;
homogfacFirstQWeyl(h);
tst_status(1);$
