LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring r=0,(x,y),ls;
poly f=x5+y3;
is_NND(f);
poly g=(x-y)^5+3xy5+y6-y7;
is_NND(g);
// if already computed, one should give the Minor number and Newton polygon
// as second and third input:
int mu=milnor(g);
list NP=newtonpoly(g);
is_NND(g,mu,NP);
tst_status(1);$
