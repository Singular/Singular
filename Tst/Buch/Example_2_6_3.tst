LIB "tst.lib";
tst_init();

option(redSB);
ring R=0,(x),(C,dp);

proc normalForm(matrix M)
{
  int n=nrows(M);
  int m=ncols(M);
  matrix N[n][m];
  M=transpose(interred(transpose(interred(M))));
  N[1..nrows(M),1..ncols(M)]=M;
  return(N);
}

matrix M[2][3]=(x2+1)^2,0,     0,
                0,      x3-x-1,0;
matrix N1[2][2]=1, 1,
                2,-2;
matrix N2[3][3]=1,2, 3, 
                4,5, 6,
                7,8,-1;
M=N1*M*N2;
print(M);

normalForm(M);

matrix M0[5][5]=1, 1,0, 0,0,
                3,-1,0, 0,0,
                0, 0,1, 1,0,
                0, 0,3,-1,0,
                0, 0,0, 0,2;
matrix N[5][5]=1, 1, -1,  1,-1,
               2, 2,  1,  1, 0,
              -1, 2,  2,  1, 1,
              -2, 1,  1, -1, 0,
               1, 2, -2,  1, 1;

M=lift(N,freemodule(nrows(N)))*M0*N-x*freemodule(5);
print(M);

print(normalForm(M));
option(noredSB);

tst_status(1);$
