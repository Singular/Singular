LIB "tst.lib";
tst_init();

// liftstd with all combinations of optional arguments

ring R = 0,(x,y,z),lp;
matrix A[2][3] = x,x,y,y,z,z;
matrix T;
module S;

print(liftstd(module(A),T,S));
print(T);
print(S);

print(liftstd(module(A),T,"std"));
print(T);

print(liftstd(module(A),T,2));
print(T);

print(liftstd(module(A),T,S,"std"));
print(T);
print(S);

print(liftstd(module(A),T,S,2));
print(T);
print(S);

print(liftstd(module(A),T,"std",2));
print(T);

print(liftstd(module(A),T,S,"std",2));
print(T);
print(S);

tst_status(1);$
