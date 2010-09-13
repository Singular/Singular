LIB "lib";
ring r;
poly p1 = 1x2yz;
poly p2 = 3x2yz5;
poly p3 = p2;
poly p4 =  (2x2yz5)^2;
poly p5 =  (2x2yz5)^2 - 1x;
poly p6 =  (2x2yz5)^2 - p2;
matrix m[2][2] = p1,p2,p3,p6;
fixmat(m,30);
" ";
matrix mm[2][2] = p6,p1,p2,p3;
fixmat(mm,30);
" ";
fixmat(m+mm,30);
" ";
fixmat(m-mm,30);
" ";
"-------------";
m = 1,0,0,1;
fixmat(m,30);
" ";
fixmat(m*mm,30);
matrix t[2][2] = 1x,0,1x,1y;
fixmat(t,30);
fixmat(t*m,30);
//fehler bei Matrix Multiplikation
ring r;
matrix a[9][6]=
-1z,
0,
0,
0,
0,
0,
0,
-1z,
0,
0,
0,
0,
0,
0,
-1z,
0,
0,
0,
0,
0,
-1y,
0,
0,
0,
0,
0,
0,
-1z,
0,
0,
0,
0,
0,
0,
-1z,
0,
0,
0,
0,
0,
-1y,
0,
0,
0,
0,
0,
0,
-1z,
0,
0,
0,
0,
0,
-1y,
0,
0,
0,
0,
0,
-1x;
matrix b[6][1]=
1z2,
1yz,
1y2,
1xz,
1xy,
1x2;
a*b;
// found by GMG (0.8.9g): crash
int k = 6;
ring r6(k) = 32003,v(1..6),ds;

ideal i =
v(4)*v(1)-v(2)^2,
v(4)*v(2)-v(3)^2-v(1)^(k-4),

v(5)*v(1)-v(3)*v(2),
v(5)*v(2)-v(4)*v(3),
v(5)*v(3)-v(4)^2+v(2)*v(1)^(k-5),

v(6)*v(1)-v(4)*v(2),
v(6)*v(2)-v(4)^2,
v(6)*v(3)-v(5)*v(4),
v(6)*v(4)-v(5)^2-v(2)^2*v(1)^(k-6);

matrix M = jacob(i);
M;
ideal m = minor(M,4);
m;
LIB "tst.lib";tst_status(1);$;
