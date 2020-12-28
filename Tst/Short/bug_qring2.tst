LIB "tst.lib";
tst_init();

/************************************************************************************
Env.sing        First step in creating non-magma calculations

11/29/15        Got quotient ring created, basic options set.
                Using reverse lexicographic order (or weighted rev.)
01/04/16        First step in constructing computations.

************************************************************************************/
//        do not print all the "redefined" warnings when we load the LIBraries.
option(noredefine) ;
//        load the stuff we need.
LIB "polylib.lib" ;
LIB "nctools.lib" ;
LIB "ncalg.lib" ;

//        reduce all ideals and Groebner bases
option(redSB) ;                 option(redTail) ;
//        simplify all expressions in any quotient ring - i.e. put in normal form
option(qringNF) ;

//        start with the least value of nu; check higher ones later.
int nu = 2 ;

//        int N = 5 ;
//        cycle through come calculable values for N
for (int N=3; N<=11; N=N+1)
{

//        N can't divide nu^2
if ( nu^2 mod N == 0 ) { newline, "Skipping N = ", N, newline ;  N= N+1 ; continue ; }

//        r is the order of q^nu
int r = lcm( N, nu^2 ) div nu^2 ;

//        Create the commutative ring over the coefficients of augmented Q
//        ring QN = (0,w), (a,b,x,y), lp ;                         // lexicographic order violates ordering condition
//        ring QN = (0,w), (a,b,x,y), rp ;                         // reverse lexicographic allows really unbalanced a & b powers.
//        ring QN = (0,w), (a,b,x,y), Dp ;                         // degree lexicographic violates ordering condition
ring QN = (0,w), (a,b,x,y), wp(1,1,N,N) ;        // keeps a & b exponents closer.

//        extend Q by Nth root of unity
minpoly = rootofUnity(N) ;

//        q is what we use in all the formulas
number q = w^nu ;

//        display all exponents with a '^' symbol;
//        must be done whenever a new ring gets declared if results are going to be printed
short = 0 ;

//        Procedure to get (k)q_nu as defined in the Radford paper.
proc subq_nu (int k)
{
        number retval = 1;
        for (int m=1; m<k; m=m+1)  { retval = retval + q^m ; }
        return( retval ) ;
}
//        G is the value of a loop
poly G = b^(nu)  ;        newline, "Loop value =", G ;

newline, "N =", N, "before creating the G-algebra." ;        //        debug info

//        Create a pair of 4x4 zero matrices and populate according to the specs for G-algebras.
matrix C[4][4] ;        C[1,2] = 1 ;        C[1,3] = q ;        C[1,4] = q^(-1) ;
                                                C[2,3] = q^(-1) ;        C[2,4] = q ;
                                                                        C[3,4] = q^(-nu) ;
matrix D[4][4] ;                                                        D[3,4] = a^(2*nu) - 1 ;

newline, "Matrix C:" ;                print (C) ;
newline, "Matrix D:" ;         print (D) ;
newline;

//        Create the basic non-commutative G-algebra, without the "exotic" relations.
def U = nc_algebra(C,D) ;                setring U ;
"The N =", N, " G-algebra has:" ;
U ;

//        display all exponents with a '^' symbol;
//        must be done whenever a new ring gets declared if results are going to be printed
short = 0 ;

matrix C = fetch( QN, C ) ;                //        Convert matrices to current ring
matrix D = fetch( QN, D ) ;

newline, "Matrix C:" ;          print (C) ;
newline, "Matrix D:" ;         print (D) ;
newline, "N =", N, "before nilpotent and order relations." ;        //        debug info

for (int m = 1; m<=N; m= m+1)
{
        newline, "(x*a)^", m, " = ", (x*a)^m, "                (y*a)^", m, " = ", (y*a)^m ;
        newline, "(y*x)^", m, " = ", (y*x)^m ;
        newline, "(y*x*a)^", m," = ", (y*x*a)^m ;
        newline, "(y*x*a)^", m, "-- (y*x)^", m, " = ", (y*x*a)^m - (y*x)^m ;
}
newline;

/*        Create the ideal of the remaining relations, get the reduced basis,
and mod out to get the quotient ring we want.                */

ideal i =         x^r, y^r, a^N-1, a*b-1 ;
qring U_q_nu = twostd(i) ;                setring U_q_nu ;
"The N =", N, " U_q_nu algebra has:" ;
U_q_nu ;

//        display all exponents with a '^' symbol;
//        must be done whenever a new ring gets declared if results are going to be printed
short = 0 ;

matrix C = fetch( U, C ) ;                //        Convert matrices to current ring
matrix D = fetch( U, D ) ;
number q = w^nu ;                        //        Need to reassign since it's a parameter.

//        Either of the next two lines gives a segmentation fault - see last lines before *** END N=...
//        Can't "fetch" it ---
//        poly G = fetch( QN, G ) ;        newline, "Loop value =", G ;
//        Can't redefine it ---
//        poly G = b^(nu) ;         newline, "Loop value =", G ;

newline, "Matrix C:" ;
print (C) ;
newline, "Matrix D:" ;
print (D) ;
newline, "N =", N, "after nilpotent and order relations." ;        //        debug info
if ( r != N ) { "Note that N =", N, "but r =", r ; }

for (int m = 1; m<=N; m= m+1)
{
        newline, "(x*a)^", m, " = ", (x*a)^m, "                (y*a)^", m, " = ", (y*a)^m ;
        newline, "(y*x)^", m, " = ", (y*x)^m ;
        newline, "(y*x*a)^", m, " = ", (y*x*a)^m ;
        newline, "(y*x*a)^", m, "-- (y*x)^", m, " = ", (y*x*a)^m - (y*x)^m ;
}

newline, newline, "a^2*x^3*y^4 + b^3*y^4*x^4 =", a^2*x^3*y^4 + b^3*y^4*x^4 ;
newline, newline, "(a^2*x^3*y^4 + b^3*y^4*x^4)^2 - (a^2*x^3*y^4 + b^3*y^4*x^4) =",
        ( a^2*x^3*y^4 + b^3*y^4*x^4 )^2 - ( a^2*x^3*y^4 + b^3*y^4*x^4) ;
newline;

//        Next line crashes Singular with a segmentation fault; notice it's the same one as that printed above.
//        As a matter of fact, *any* polynomial declaration does.

poly h = a^2*x^3*y^4 + b^3*y^4*x^4;  h^2 - h;
poly k = h^2 - h ; k;

"******************************************  END N =", N, " ******************************************" ;
"*************************************************************************************************" ;
}

tst_status(1);$
