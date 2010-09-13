LIB "tst.lib";
tst_init();
//
// test script for lift command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;
"-------------------------------";
poly s1=y7+x3+xyz+z2;
ideal i1=jacob(s1);
ideal i2=std(i1);
ideal i=3x2+yz,7y6+2x2y+5xz;
i;
i1;
i2;
s1;
matrix T=lift(i2,i);
T;
matrix(i)-matrix(i2)*T;
"-----------------------------";
poly s2=x4+y3+z6+2xyz;
ideal i3=jacob(s2);
ideal i4=std(i3);
s2;
i3;
i4;
matrix T1=lift(i4,i);
T1;
matrix(i)-matrix(i4)*T1;
"-------------------------------";
listvar(all);
kill r1;
//From:    Trond St|len Gustavsen <stolen@math.uio.no>

ring r = 0, (x(1..5)), ds;
matrix m[2][4]= x(1), x(2), x(3), x(4), x(2), x(3), x(4), x(5);

ideal j = minor(m, 2);
ideal i = std(j);

list ires = mres(i, 2);
def ires(1..2) = ires[1..2];
kill ires;
matrix jaco = jacob(ires(1));
qring s = i;
s;
matrix mat = matrix(fetch(r,ires(2)));
matrix imat = transpose(mat);
matrix jac = fetch(r, jaco);

list ll = nres(module(imat),3);
def imatres(1..3) = ll[1..3];

matrix T = lift(imatres(2), module(jac));
T;
kill r;
tst_status(1);$;
