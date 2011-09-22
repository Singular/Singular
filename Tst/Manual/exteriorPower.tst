LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r = (0),(a, b, c, d, e, f), dp;
r; "base ring:";
module B = a*gen(1) + c*gen(2) + e*gen(3),
b*gen(1) + d*gen(2) + f*gen(3),
e*gen(1) + f*gen(3);
print(B);
print(exteriorPower(B, 2));
print(exteriorPower(B, 3));
def g = superCommutative(); setring g; g;
module A = a*gen(1), b * gen(1), c*gen(2), d * gen(2);
print(A);
print(exteriorPower(A, 2));
module B = a*gen(1) + c*gen(2) + e*gen(3),
b*gen(1) + d*gen(2) + f*gen(3),
e*gen(1) + f*gen(3);
print(B);
print(exteriorPower(B, 2));
print(exteriorPower(B, 3));
tst_status(1);$
