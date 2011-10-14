LIB "tst.lib";
tst_init();

// testing matrix dimensions
ring r=0,x,dp;
matrix(8,1,1);    
print(matrix(8,3,4));
matrix A = 8;
A;
matrix(A,1,1);   // everythin if fine up to here but ...
//... the following the non-postive second and third args should be rejected
matrix(A,1,0);    // already this bug may lead to crash if used as input  
matrix B = _;
B;                // what is B ?
size(B);
ncols(B);
nrows(B);   // mismatch with size(B)
matrix(A,0,1);
matrix(A,-1,1);
matrix(1,-1,1);                       //  compare with the beginning

tst_status(1);$
