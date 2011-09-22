LIB "tst.lib"; tst_init();
LIB "sagbi.lib";
ring r= 0,(x,y,z),dp;
ideal A=x2,y2,xy+y;
//Default call, no tail-reduction is done.
sagbi(A);
//---------------------------------------------
//Call with tail-reduction and method specified.
sagbi(A,1,0);
tst_status(1);$
