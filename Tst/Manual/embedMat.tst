LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring r = 0,(a,b,c,d),dp;
matrix M[2][3]; M[1,1]=a; M[1,2]=b;M[2,2]=d;M[1,3]=c;
print(M);
print(embedMat(M,3,4));
matrix N = M; N[2,2]=0;
print(embedMat(N,3,4));
tst_status(1);$
