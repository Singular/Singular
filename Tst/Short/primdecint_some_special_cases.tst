LIB "tst.lib";
tst_init();
echo = 1;

LIB("primdecint.lib");

proc addIdealZ(ideal I,ideal J)
{
    int i;
    for(i=1; i<=ncols(J); i++)
    {
        I[ncols(I)+1] = J[i];
    }
    return(I);
}


proc coefPrimeZW(ideal I)
{
   return(Primdecint::coefPrimeZ(I));
}
proc specialPowerZW(ideal I, int m)
{
    return(Primdecint::specialPowerZ(I,m));
}
proc quotientZW(I,J)
{
   return(Primdecint::quotientZ(I,J));
}
proc reduceZW(poly f,ideal I)
{
   return(Primdecint::reduceZ(f,I));
}

ring rng = integer,(x,y),dp;
ideal I = 0,5*x-3;
ideal J = 0,y-5;
ideal sumIJ = addIdealZ(I,J);
I+J;

ASSUME(0, idealsEqual(I+J, sumIJ) );

ideal P = 0,5;

def res = coefPrimeZW(P);
ASSUME(0, size(res)>0 );


ASSUME(0, idealsEqual( specialPowerZW(I,2), I^2 ) );


I = 0,x-3;
J = 0,x;

ASSUME(0, idealsEqual( quotientZW(I,J), x-3 ) );

reduceZW( x,ideal(0,x) );  // would produce an error if reduceZ is not patched.


tst_status(1); $
