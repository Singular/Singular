//------------------------------------------
ring r;
matrix A[3][4]=1;
module ma=module(A);
ma;
module mi=x*gen(1);
module ml=lift(ma,mi);
ma;
//------------------------------------------
matrix A[3][4]=1;
matrix B[3][2]=0,x,y;
modulo(A,B);
syz(A)+module(lift(A,intersect(A,B)));
$
