LIB "tst.lib";
tst_init();

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

option(redSB);
ring R=0,(x),(C,dp);
LIB"matrix.lib";

matrix M[5][5]=1, 1,0,0,0,
              -2,-1,0,0,0,
               0, 0,2,1,0,
               0, 0,0,2,0,
               0, 0,0,0,2;

matrix N[5][5]=1,2, 2, 2,-1,
               1,1, 2, 1, 1,
              -1,1, 2,-1, 2,
              -1,1, 1,-1, 2,
               1,2,-1, 2, 1;
M=lift(N,freemodule(nrows(N)))*M*N;
print(M);

matrix A=M-x*freemodule(5);

list L= extendedNormalForm(A);

print(L[1]);

print(L[2]); 

matrix V1[5][4]=concat(L[1][1],M*L[1][1],M*M*L[1][1],M*M*M*L[1][1]);
matrix V2[5][1]=L[1][2]; 

list F=factorize(L[2][1,1]);
F;

proc polyOfEndo(matrix B,poly p)
{
  int i;
  int d=nrows(B);
  matrix A=coeffs(p,var(1));
  matrix E[d][d]=freemodule(d);
  matrix C[d][d]=A[1,1]*E;
  for(i=2;i<=nrows(A);i++)
  {
     E=E*B;
     C=C+A[i,1]*E;
  }
  return(C);
}

matrix S=polyOfEndo(M,F[1][2]^2);
matrix V11=std(S*V1);
print(V11);

S=polyOfEndo(M,F[1][3]); 
matrix V12=std(S*V1);
print(V12);

matrix B=concat(V11,V12,V2);
det(B);                 

reduce(M*V11,std(V11));  
reduce(M*V12,std(V12));
reduce(M*V2,std(V2));

matrix C=lift(B,M*B);  
print(C);   

matrix v[5][1]=V12[1];   
B=concat(V11,M*v-2*v,v,V2);
C=lift(B,M*B);
print(C);  

tst_status(1);$
