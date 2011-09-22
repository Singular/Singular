LIB "tst.lib"; tst_init();
LIB "sagbi.lib";
ring r= 0,(x,y,z),dp;
//The following algebra does not have a finite SAGBI basis.
ideal A=x,xy-y2,xy2;
//---------------------------------------------------
//Call with two iterations, no tail-reduction is done.
sagbiPart(A,2);
//---------------------------------------------------
//Call with three iterations, tail-reduction and method 0.
sagbiPart(A,3,1,0);
tst_status(1);$
