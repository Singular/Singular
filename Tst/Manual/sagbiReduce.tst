LIB "tst.lib"; tst_init();
LIB "sagbi.lib";
ring r=0,(x,y,z),dp;
ideal A=x2,2*x2y+y,x3y2;
poly p1=x^5+x2y+y;
poly p2=x^16+x^12*y^5+6*x^8*y^4+x^6+y^4+3;
ideal P=p1,p2;
//---------------------------------------------
//SAGBI reduction of polynomial p1 by algebra A.
//Default call, that is, no tail-reduction is done.
sagbiReduce(p1,A);
//---------------------------------------------
//SAGBI reduction of set of polynomials P by algebra A,
//now tail-reduction is done.
sagbiReduce(P,A,1);
tst_status(1);$
