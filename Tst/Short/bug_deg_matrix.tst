LIB "tst.lib";
tst_init();

ring r=0,(x,y),dp;
matrix M[3][2];

// Matrix degree must inspect every entry, not only the first row.
M[3,2]=x2;
deg(M);
deg(M,intvec(1,1));

// The maximum may occur in another row and column.
M[2,1]=x3+y3;
deg(M);
deg(M,intvec(2,1));

matrix Z[4][3];
deg(Z);
deg(Z,intvec(1,1));

tst_status(1);$
