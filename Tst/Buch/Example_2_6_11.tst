LIB "tst.lib";
tst_init();

proc normalForm(matrix M)
{
   int n=nrows(M);
   int m=ncols(M);
   matrix N[n][m];
   M=transpose(interred(transpose(interred(M))));
   N[1..nrows(M),1..ncols(M)]=M;
   return(N);
}

option(redSB);
ring R=0,(x),(C,dp);

matrix M0[5][5]=1, 1,0,0,0,
               -2,-1,0,0,0,
                0, 0,2,1,0,
                0, 0,0,2,0,
                0, 0,0,0,3;
matrix N[5][5]=1, 1, -1,  1,-1,
               2, 2,  1,  1, 0,
              -1, 2,  2,  1, 1,
              -2, 1,  1, -1, 0,
               1, 2, -2,  1, 1;
matrix M=lift(N,freemodule(nrows(N)))*M0*N-x*freemodule(5);
print(M);

N=normalForm(M);
print(N);

factorize(N[1,1]);

LIB"matrix.lib";

proc extendedNormalForm(matrix M)
{
   intvec v=1..nrows(M); 
   intvec w=nrows(M)+1..nrows(M)+ncols(M); 
   matrix N=concat(freemodule(nrows(M)),M);
   N=transpose(interred(transpose(N)));
   matrix C=submat(N,v,v);
   C=lift(C,freemodule(nrows(C))); //the inverse matrix of C
   matrix D=submat(N,v,w); 
   D=transpose(interred(D)); 
   list Re=C,D;
   return(Re);  
}

matrix M1[2][2]=x2+1,0,
                0,   x-1;
matrix N1[2][2]=1, 1,
                1, 2;
matrix N2[2][2]=0,-1,
                1, 1;
               
M=N1*M1*N2;
print(M);

list L=extendedNormalForm(M);
print(L[1]);
 
print(L[2]);

tst_status(1);$
