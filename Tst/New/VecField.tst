LIB "tst.lib"; tst_init();

LIB "VecField.lib";
ring r = 0, (x, y, z),ds;
poly f = x3 + y3 + z3;
ideal J = std(jacob(f));
map phi1 = r, x - y2 + z3, y + z2 + y3, z;
map phi2 = r, x - zy + 2x3y + y5 + zxy3, y - x3z + xy4, z + y5 - xz3y + z5;


map invphi1 = invertAlgebraMorphism(phi1, 4);
map invphi2 = invertAlgebraMorphism(phi2, 4);

ideal J1 = std(invphi1(J));
ideal J2 = std(invphi2(J));

//All appearing vectorfields are logarithmic derivations of either
//f = x^3 + y^3 + z^3 or of the jacobian ideal J of f.
//f is 3 - determined.
//The degree of the highest corner of J is 3, so the fourth power
//of the maximal ideal lies in J.

//First we check whether the vectorfields send f resp. J into
// themselves


///////Computations for f////////////////////////////////////////

///////Check for f///////////////////////////////////////////////

vector v = -x*gen(1) - y*gen(2) - z*gen(3);

VecField V = v;
V.precision = 4;

print("V = "); print(V);
print("V*f/f= "); print((V*f)/f);

///////Decompose transforms of V ///////////////////////////////

VecField V1 = changeCoordinates(V, phi1);
print("V1 = "); print(V1);

VecField V2 = changeCoordinates(V, phi2);
print("V2 = "); print(V2);

VecField W1 = SaitoBase(V1);
print("W1 = "); print(W1);

VecField W2 = SaitoBase(V2);
print("W2 = "); print(W2);

///////Computations for J///////////////////////////////////////
///////Check for J//////////////////////////////////////////////

vector v1 = - y*gen(2) - z4*gen(1);
vector v2 = - y*gen(2) - y4*gen(1) - x4*gen(3);
vector v3 = - x*gen(1) - x4*gen(3) - x4*gen(2);

kill V1,V2,W1,W2;
VecField V1 = v1;
VecField V2 = v2;
VecField V3 = v3;

V1.precision = 4;
V2.precision = 4;
V3.precision = 4;

print(" V1*J mod J: "); print(reduce(V1*J,J));
print(" V2*J mod J: "); print(reduce(V1*J,J));
print(" V3*J mod J: "); print(reduce(V1*J,J));

///////Part 1: Decompose certain vectorfields

list L = V1, V2, V3;
list l = decomposeVecField(L);

//Decompose vectorfields into semisimple and nilpotent part:

print("V1_semisimple = "); print(l[1]);
print("V1_nilpotent = "); print(l[2]);
print("V2_semisimple = "); print(l[3]);
print("V2_nilpotent = "); print(l[4]);
print("V3_semisimple = "); print(l[5]);
print("V3_nilpotent = "); print(l[6]);

//Check that they still send J into J

print(" l[1]*J mod J: "); print(reduce(l[1]*J,J));
print(" l[2]*J mod J: "); print(reduce(l[2]*J,J));
print(" l[3]*J mod J: "); print(reduce(l[3]*J,J));
print(" l[4]*J mod J: "); print(reduce(l[4]*J,J));
print(" l[5]*J mod J: "); print(reduce(l[5]*J,J));
print(" l[6]*J mod J: "); print(reduce(l[6]*J,J));

//Apply coordinate changes and decompose again

VecField U1 = changeCoordinates(V1, phi1);
print("U1 = "); print(U1);
VecField U2 = changeCoordinates(V2, phi1);
print("U2 = "); print(U2);
VecField U3 = changeCoordinates(V3, phi1);
print("U3 = "); print(U3);

VecField W1 = changeCoordinates(V1, phi2);
print("W1 = "); print(W1);
VecField W2 = changeCoordinates(V2, phi2);
print("W2 = "); print(W2);
VecField W3 = changeCoordinates(V3, phi2);
print("W3 = "); print(W3);

list L1 = U1, U2, U3;
list L2 = W1, W2, W3;

list l1 = decomposeVecField(L1);
list l2 = decomposeVecField(L2);

print("U1_semisimple = "); print(l1[1]);
print("U1_nilpotent = "); print(l1[2]);
print("U2_semisimple = "); print(l1[3]);
print("U2_nilpotent = "); print(l1[4]);
print("U3_semisimple = "); print(l1[5]);
print("U3_nilpotent = "); print(l1[6]);

print("W1_semisimple = "); print(l2[1]);
print("W1_nilpotent = "); print(l2[2]);
print("W2_semisimple = "); print(l2[3]);
print("W2_nilpotent = "); print(l2[4]);
print("W3_semisimple = "); print(l2[5]);
print("W3_nilpotent = "); print(l2[6]);

//Check, if they are still logarithmic

print(" l1[1]*J1 mod J1: "); print(reduce(l1[1]*J1,J1));
print(" l1[2]*J1 mod J1: "); print(reduce(l1[2]*J1,J1));
print(" l1[3]*J1 mod J1: "); print(reduce(l1[3]*J1,J1));
print(" l1[4]*J1 mod J1: "); print(reduce(l1[4]*J1,J1));
print(" l1[5]*J1 mod J1: "); print(reduce(l1[5]*J1,J1));
print(" l1[6]*J1 mod J1: "); print(reduce(l1[6]*J1,J1));

print(" l2[1]*J2 mod J2: "); print(reduce(l2[1]*J2,J2));
print(" l2[2]*J2 mod J2: "); print(reduce(l2[2]*J2,J2));
print(" l2[3]*J2 mod J2: "); print(reduce(l2[3]*J2,J2));
print(" l2[4]*J2 mod J2: "); print(reduce(l2[4]*J2,J2));
print(" l2[5]*J2 mod J2: "); print(reduce(l2[5]*J2,J2));
print(" l2[6]*J2 mod J2: "); print(reduce(l2[6]*J2,J2));


///////Part 2: Diagonalize vectorfields simultaneously

//Diagonalize the semisimple parts simultaneously after coordinate change:

vector u1 = x*gen(1);
vector u2 = y*gen(2);
vector u3 = z*gen(3);

VecField X1 = u1;
VecField X2 = u2;
VecField X3 = u3;

X1.precision = 4;
X2.precision = 4;
X3.precision = 4;

//Check that all vectorfields commute pairwisely:

print("X1*X2 = "); print(X1*X2);
print("X1*X3 = "); print(X1*X3);
print("X2*X3 = "); print(X2*X3);

//Diagonalize them simultaneously:
kill U1,U2,U3,W1,W2,W3;
VecField U1 = changeCoordinates(X1, phi1);
print("U1 = "); print(U1);
VecField U2 = changeCoordinates(X2, phi1);
print("U2 = "); print(U2);
VecField U3 = changeCoordinates(X3, phi1);
print("U3 = "); print(U3);

VecField W1 = changeCoordinates(X1, phi2);
print("W1 = "); print(W1);
VecField W2 = changeCoordinates(X2, phi2);
print("W2 = "); print(W2);
VecField W3 = changeCoordinates(X3, phi2);
print("W3 = "); print(W3);

list L3 = U1, U2, U3;
list L4 = W1, W2, W3;

list l3 = diagonalizeVecField(L3);
print("l3 = " ); print(l3);

map phi3 = l3[1].coordinates;
map invphi3 = invertAlgebraMorphism(phi3,4);
ideal J3 = std(invphi3(J1));

print(" l3[1]*J3 mod J3: "); print(reduce(l3[1]*J3,J3));
print(" l3[2]*J3 mod J3: "); print(reduce(l3[2]*J3,J3));
print(" l3[3]*J3 mod J3: "); print(reduce(l3[3]*J3,J3));


list l4 = diagonalizeVecField(L4);
print("l4 = " ); print(l4);

map phi4 = l4[1].coordinates;
map invphi4 = invertAlgebraMorphism(phi4,4);

ideal J4 = std(invphi4(J2));

print(" l4[1]*J4 mod J4: "); print(reduce(l4[1]*J4,J4));
print(" l4[2]*J4 mod J4: "); print(reduce(l4[2]*J4,J4));
print(" l4[3]*J4 mod J4: "); print(reduce(l4[3]*J4,J4));

tst_status(1);$

