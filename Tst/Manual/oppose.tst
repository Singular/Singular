LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def r = makeUsl2();
setring r;
matrix m[3][4];
poly   p  = (h^2-1)*f*e;
vector v  = [1,e*h,0,p];
ideal  i  = h*e, f^2*e,h*f*e;
m         = e,f,h,1,0,h^2, p,0,0,1,e^2,e*f*h+1;
module mm = module(m);
def b     = opposite(r);
setring b; b;
// we will oppose these objects: p,v,i,m,mm
poly P    = oppose(r,p);
vector V  = oppose(r,v);
ideal  I  = oppose(r,i);
matrix M  = oppose(r,m);
module MM = oppose(r,mm);
def c = opposite(b);
setring c;   // now let's check the correctness:
// print compact presentations of objects
print(oppose(b,P)-imap(r,p));
print(oppose(b,V)-imap(r,v));
print(matrix(oppose(b,I))-imap(r,i));
print(matrix(oppose(b,M))-imap(r,m));
print(matrix(oppose(b,MM))-imap(r,mm));
tst_status(1);$
