LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def Usl2 = makeUsl2(); // this algebra is U(sl_2)
setring Usl2;
option(redSB);
option(redTail);
poly  C  = 4*e*f+h^2-2*h; // the central element of Usl2
ideal I  = e^3,f^3,h^3-4*h;
ideal J  = twostd(I);
 // print a compact presentation of J:
print(matrix(ideal(J[1..5]))); // first 5 generators
print(matrix(ideal(J[6..size(J)]))); // last generators
ideal QC = twostd(C-8);
qring Q  = QC;
ideal QJ = fetch(Usl2,J);
QJ = std(QJ);
// thus QJ is the image of I in the factor-algebra QC
print(matrix(QJ)); // print QJ compactly
tst_status(1);$
