LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring R = 0,(x(1..4)),dp; // global!
def ER = superCommutative(); // the same as Exterior (b = 1, e = N)
setring ER; ER;
"Alternating variables: [", AltVarStart(), ",", AltVarEnd(), "].";
kill R; kill ER;
ring R = 0,(x(1..4)),(lp(1), dp(3)); // global!
def ER = superCommutative(2); // b = 2, e = N
setring ER; ER;
"Alternating variables: [", AltVarStart(), ",", AltVarEnd(), "].";
kill R; kill ER;
ring R = 0,(x, y, z),(ds(1), dp(2)); // mixed!
def ER = superCommutative(2,3); // b = 2, e = 3
setring ER; ER;
"Alternating variables: [", AltVarStart(), ",", AltVarEnd(), "].";
x + 1 + z + y; // ordering on variables: y > z > 1 > x
std(x - x*x*x);
std(ideal(x - x*x*x, x*x*z + y, z + y*x*x));
kill R; kill ER;
ring R = 0,(x, y, z),(ds(1), dp(2)); // mixed!
def ER = superCommutative(2, 3, ideal(x - x*x, x*x*z + y, z + y*x*x )); // b = 2, e = 3
setring ER; ER;
"Alternating variables: [", AltVarStart(), ",", AltVarEnd(), "].";
tst_status(1);$
