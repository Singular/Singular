LIB "tst.lib"; tst_init();
LIB "realrad.lib";

// issue #1272: GeneralPos left the linear-relation loop bound at its default
// value 0, so the normalization path was skipped.  Safely enabling it also
// requires a basis in the current variable order, an invertible random map,
// and restoration of affine relations after mapping back.
system("--random",1272);
ring r=0,(x,y,z,t),dp;
ideal j=x^2-t,y^2+t,z*t-1;
ideal rr=realrad(j);
rr;
ASSUME(0, size(rr)==1 && rr[1]==1);

// Removing a linear variable from the random map would make the map
// singular.  Check that an affine relation and its nonempty real variety
// survive the coordinate change and the inverse map.
system("--random",1272);
ring s=(0,a),(x,y,z),dp;
ideal k=x^2-2,y^2-3,z-a;
ideal rk=realzero(k);
rk;
ASSUME(0, idealsEqual(std(rk),std(k)));

// Recompute the basis after prepare_max maps a permuted lex basis back to
// this ring; otherwise lead() can select the wrong variable in z-y.
system("--random",1272);
ideal h=x^2-2,y^2-3,z-y;
ideal rh=realzero(h);
rh;
ASSUME(0, idealsEqual(std(rh),std(h)));

tst_status(1);$
