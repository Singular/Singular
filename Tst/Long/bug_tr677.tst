LIB "tst.lib";
tst_init();

LIB"primdec.lib";
ring r = (0,p), (w,x,y,z), dp;
minpoly = p2-p-1;
poly f = 4*(p2x2-y2)*(p2y2-z2)*(p2z2-x2)-(1+2p)*(x2+y2+z2-w2)^2*w2;
ideal I = jacob(f);   // !
list L = primdecGTZ(I);
ideal J = 1;
int i;
for (i = size(L); i > 0; i--) { J = intersect(J, L[i][1]); };
size(reduce(I, std(J)));   // now J should be the same ideal as I, so this should be zero
size(reduce(J, std(I)));   // dito

tst_status(1);$
