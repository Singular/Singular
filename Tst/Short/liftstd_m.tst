LIB "tst.lib";
tst_init();

// liftstd with all combinations of optional arguments

ring R = 0,(x,y,z),lp;
matrix A[2][3] = x,x,y,y,z,z;
module A1=module(A);
module A2=A1[3];
A1=A1[1..2];
matrix T;
module S;

print(liftstd(module(A),T,S));
print(T);
print(S);

print(liftstd(module(A),T,"std"));
print(T);

print(liftstd(module(A),T,S,"std"));
print(T);
print(S);

// relative versions

// the three-argument relative version is not implemented and would overwrite A2
// print(liftstd(A1,T,A2));
// print(T);

print(liftstd(A1,T,S,A2));
print(T);
print(S);

print(liftstd(A1,T,"std",A2));
print(T);

print(liftstd(A1,T,S,"std",A2));
print(T);
print(S);
S=modulo(A1,A2,T,"std");
print(T);
print(S);

// with liftstd: G=liftStd(M1,T,S, M2): S=modulo(M1,M2,T)
tst_status(1);$
