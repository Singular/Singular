LIB "tst.lib";
tst_init();
//
// test script for lift/division command
//
ring r;
ideal i=x2,y2,z2;
ideal ii=maxideal(2);
ideal j=maxideal (3);
"i";
print(matrix(i));
"j";
print(matrix(j));
matrix U;
"lift(i,j)";
print(lift(i,j));
"lift(i,j,U)";
print(lift(i,j,U));
"U";
print(U);
"division(i,j)";
list L=division(j,i);
print(L[1]);
print(L[2]);
print(L[3]);
"ii";
print(matrix(ii));
"j";
print(matrix(j));
matrix U;
"lift(ii,j)";
print(lift(ii,j));
"lift(ii,j,U)";
print(lift(ii,j,U));
"U";
print(U);
"division(ii,j)";
list L=division(j,ii);
print(L[1]);
print(L[2]);
print(L[3]);
kill r;
ring r;
module H=
x2*gen(1),
x2*gen(2),
x2*gen(3),
x2*gen(4),
x2*gen(5),
x2*gen(7),
x2*gen(8),
x2*gen(9),
x2*gen(10),
x2*gen(11),
x*gen(6);

module H0=
11/10x2*gen(1)-175/32x3*gen(9)+625/32x3*gen(1),
-1/4x2*gen(8)+9/10x2*gen(2)-3125/64x3*gen(8)+875/64x3*gen(2),
13/10x2*gen(3)+15/16x3*gen(10)+1875/32x3*gen(3),
-25/4x2*gen(6)+x2*gen(4)+15/16x3*gen(11)-734375/512x3*gen(6)+125/8x3*gen(4),
-75/16x2*gen(7)+7/10x2*gen(5)-234375/256x3*gen(7)+375/64x3*gen(5),
13/10x2*gen(7)+1875/32x3*gen(7)-3/8x3*gen(5),
11/10x2*gen(8)+625/32x3*gen(8)-175/32x3*gen(2),
9/10x2*gen(9)-1/4x2*gen(1)+875/64x3*gen(9)-3125/64x3*gen(1),
7/10x2*gen(10)+15/8x2*gen(3)+375/64x3*gen(10)+46875/128x3*gen(3),
-1/2x*gen(6)+1/2x2*gen(11),
1/2x*gen(6)+5875/64x2*gen(6)-x2*gen(4);

matrix A=matrix(lift(H,H0));
print(A);
print(matrix(H)*A-matrix(H0));
"jetzt division";
list I=division(H0,H);
"I[1]";
print(I[1]);
"I[2]";
print(I[2]);
"I[3]";
print(I[3]);
tst_status(1);$;
