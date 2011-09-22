LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r = (0),(a, b, c, d), dp; r;
module B = a*gen(1) + c* gen(2), b * gen(1) + d * gen(2); print(B);
// symmetric power over a commutative K-algebra:
print(symmetricPower(B, 2));
print(symmetricPower(B, 3));
// symmetric power over an exterior algebra:
def g = superCommutative(); setring g; g;
module B = a*gen(1) + c* gen(2), b * gen(1) + d * gen(2); print(B);
print(symmetricPower(B, 2)); // much smaller!
print(symmetricPower(B, 3)); // zero! (over an exterior algebra!)
tst_status(1);$
