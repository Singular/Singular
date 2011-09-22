LIB "tst.lib"; tst_init();
LIB "sagbi.lib";
ring r= 0,(x,y),dp;
//The following algebra does not have a finite SAGBI basis.
ideal I=x^2, xy-y2, xy2;
//---------------------------------------------------
//Call with two iterations
def DI = algebraicDependence(I,2);
setring DI; algDep;
// we see that no dependency has been seen so far
//---------------------------------------------------
//Call with two iterations
setring r; kill DI;
def DI = algebraicDependence(I,3);
setring DI; algDep;
map F = DI,x,y,x^2, xy-y2, xy2;
F(algDep); // we see that it is a dependence indeed
tst_status(1);$
