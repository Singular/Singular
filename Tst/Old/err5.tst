//From:    Trond St|len Gustavsen <stolen@math.uio.no>

ring r = 0, (x(1..5)), ds;
matrix m[2][4]= x(1), x(2), x(3), x(4), x(2), x(3), x(4), x(5);

ideal j = minor(m, 2);
ideal i = std(j);

mres(i, 2, ires);
matrix jaco = jacob(ires(1));
qring s = i;
s;
matrix mat = matrix(fetch(r,ires(2)));
matrix imat = transpose(mat);
matrix jac = fetch(r, jaco);

res(module(imat),3,imatres);

matrix T = lift(imatres(2), module(jac));

module P=std(module(T))+std(imatres(3));

module Q=std(P);
Q;
kbase(Q);
vdim(Q);
degree(Q);

//When executing the kbase(Q) gives 4 elements in Q, but vdim returns -1.
//----------------------------------------------------------------------------
// incorrect kbase and vdim (0.8.9f) (anne)
ring rr=32003,(x,y),dp;
module i=
1*gen(1),
x*gen(2),
y^3*gen(2);
vdim(i);
kbase(i);
$
