LIB "tst.lib";
tst_init();

// missing branch, tau_es diff.
LIB "hnoether.lib";
LIB "equising.lib";
ring r = 0,(x,y),ds;
poly f = (x2+y2)*(y2-(x-y)^3)*(x^3+y9);
tau_es(f);
f = ((x+y)^2+y2)*(y2-x3)*((x+y)^3+y9);
tau_es(f);

f = (y2-(x-y)^3)*(x3+y9);     //4 Zweige
poly g=subst(f,x,x+y);
tau_es(g);          // sollte 24 sein
list K=esIdeal(g);  
vdim(std(K[1]));    // sollte mit tau_es(g) uebereinstimmen

tst_status(1);$
