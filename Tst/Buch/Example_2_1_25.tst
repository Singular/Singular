LIB "tst.lib";
tst_init();

proc contraHom(matrix M,int s)
{
   int n=ncols(M);
   int m=nrows(M);
   int a,b,c;
   matrix R[s*n][s*m];
   for(b=1;b<=m;b++)
   {
      for(a=1;a<=s;a++)
      {
         for(c=1;c<=n;c++)
         {
            R[(a-1)*n+c,(a-1)*m+b]=M[b,c];
         }
      }
   }
   return(R);
}

proc coHom(matrix M,int s)
{
   int n=ncols(M);
   int m=nrows(M);
   int a,b,c;
   matrix R[s*m][s*n];
   for(b=1;b<=s;b++)
   {
      for(a=1;a<=m;a++)
      {
         for(c=1;c<=n;c++)
         {
            R[(a-1)*s+b,(c-1)*s+b]=M[a,c];
         }
      }
   }
   return(R);
}

proc Hom(matrix M, matrix N)
{
  matrix A = contraHom(M,nrows(N));
  matrix B = coHom(N,ncols(M));
  matrix C = coHom(N,nrows(M));
  matrix D = modulo(A,B);
  matrix E = modulo(D,C);
  return(E);
}

ring A=0,(x,y,z),dp;
matrix M[3][3]=1,2,3,
               4,5,6,
               7,8,9;
matrix N[2][2]=x,y,
               z,0;

print(Hom(M,N));


tst_status(1);$
