LIB "tst.lib"; tst_init();
ring R = 32003,(x,y,z),dp;
R;
x > y;
y > z;
int a,b,c,t = 1,2,-1,4;
poly f = a*x3+b*xy3-c*xz3+t*xy2z2;
f;
ideal i = jacob(f);    // Jacobian Ideal of f
ideal si = std(i);     // compute Groebner basis
int dimi = dim(si);
string s = "The dimension of V(i) is "+string(dimi)+".";
s;
LIB "primdec.lib";     // load library primdec.lib
list L = primdecGTZ(i);
size(L);               // number of prime components
L[1][1];               // first primary component
L[1][2];               // corresponding prime component
ring Rloc = 32003,(x,y,z),ds;   // ds = local monomial ordering
ideal i = imap(R,i);
dim(std(i));
map phi = R, x-2000, y-6961, z-7944;
dim(std(phi(i)));
tst_status(1);$
