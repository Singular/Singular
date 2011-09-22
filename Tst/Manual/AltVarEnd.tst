LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring R = 0,(x(1..4)),dp; // global!
def ER = superCommutative(2); // (b = 2, e = N)
setring ER; ER;
"Alternating variables: [", AltVarStart(), ",", AltVarEnd(), "].";
setring R;
"Alternating variables: [", AltVarStart(), ",", AltVarEnd(), "].";
kill R, ER;
//////////////////////////////////////////////////////////////////
ring R = 2,(x(1..4)),dp; // the same in char. = 2!
def ER = superCommutative(2); // (b = 2, e = N)
setring ER; ER;
"Alternating variables: [", AltVarStart(), ",", AltVarEnd(), "].";
setring R;
"Alternating variables: [", AltVarStart(), ",", AltVarEnd(), "].";
tst_status(1);$
