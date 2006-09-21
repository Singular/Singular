LIB "tst.lib";
tst_init();
// error in groebner: std with hilb in strage orderings
ring r = 0,(x1,x2),(a(1),dp);
number a = 200;
number b = 400;
number c = 200;
number d = 800;
number e = 800;
number f = 200;
number g = 2;
number h = 6;
number i = 3;
number j = 5;
number A = a/c;
number B = b/c;
number D = d/f;
number E = e/f;
number G = g/j;
number H = h/j;
number I = i/j;
poly p1 = A + B*x1 + x1^2;
poly p2 = D + E*x2 + x2^2;
poly p3 = G + H*x1 + I*x2 + x1*x2;
poly q1 = p1 - p2;
poly q2 = -p2 + p3^2;
option(prot);
option(mem);
ideal eq = q1,q2;
eq = interred(eq);
ideal jeq = groebner(eq);
jeq;
std(eq);
slimgb(eq);
// returns one generator 
// jeq[1]=20*x1*x2^2+64*x1*x2+48*x1-25*x2^4-160*x2^3-335*x2^2-236*x2-12
// instead of 3 (which gives e.g. std or slimgb)


tst_status(1);$
