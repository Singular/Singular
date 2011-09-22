LIB "tst.lib"; tst_init();
LIB "nctools.lib";
/////////////////////////////////////////////////////////////////////
ring R = 0,(x(1..4)),dp; // commutative
if(IsSCA())
{ "Alternating variables: [", AltVarStart(), ",", AltVarEnd(), "]."; }
else
{ "Not a super-commutative algebra!!!"; }
kill R;
/////////////////////////////////////////////////////////////////////
ring R = 0,(x(1..4)),dp;
def S = nc_algebra(1, 0); setring S; S; // still commutative!
if(IsSCA())
{ "Alternating variables: [", AltVarStart(), ",", AltVarEnd(), "]."; }
else
{ "Not a super-commutative algebra!!!"; }
kill R, S;
/////////////////////////////////////////////////////////////////////
ring R = 0,(x(1..4)),dp;
list CurrRing = ringlist(R);
def ER = ring(CurrRing);
setring ER; // R;
matrix E = UpOneMatrix(nvars(R));
int i, j; int b = 2; int e = 3;
for ( i = b; i < e; i++ )
{
for ( j = i+1; j <= e; j++ )
{
E[i, j] = -1;
}
}
def S = nc_algebra(E,0); setring S; S;
if(IsSCA())
{ "Alternating variables: [", AltVarStart(), ",", AltVarEnd(), "]."; }
else
{ "Not a super-commutative algebra!!!"; }
kill R, ER, S;
/////////////////////////////////////////////////////////////////////
ring R = 0,(x(1..4)),dp;
def ER = superCommutative(2); // (b = 2, e = N)
setring ER; ER;
if(IsSCA())
{ "This is a SCA! Alternating variables: [", AltVarStart(), ",", AltVarEnd(), "]."; }
else
{ "Not a super-commutative algebra!!!"; }
kill R, ER;
tst_status(1);$
