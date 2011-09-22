LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r = (3,w), (a,b,c,d),dp;
minpoly = w2-w-1;
module M = [a2,b2],[wab,w2c2+2b2],[(w-2)*a2+wab,wb2+w2c2];
module REL = linear_relations(M);
pmat(REL);
pmat(M*REL);
tst_status(1);$
