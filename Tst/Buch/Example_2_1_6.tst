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

ring A=0,(x,y,z),dp;
matrix M[3][3]=1,2,3,
               4,5,6,
               7,8,9;

print(contraHom(M,2));

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

print(coHom(M,2));

tst_status(1);$
