LIB "tst.lib";
tst_init();

proc fitting(matrix M, int n)
{
  n=nrows(M)-n;
  if(n<=0){return(ideal(1));}
  if((n>nrows(M))||(n>ncols(M))){return(ideal(0));}
  return(std(minor(M,n)));  
}

proc isLocallyFree(matrix M, int n)
{
   ideal F=fitting(M,n);
   ideal G=fitting(M,n-1);
   if((deg(F[1])==0)&&(size(G)==0)){return(1);}
   return(0);
}

ring R=0,(x,y,z),dp;
matrix S[2][3];
S=x-1,y-1,z,y-1,x-2,x;
ideal I=fitting(S,0);
qring Q=I;
matrix S=fetch(R,S);
isLocallyFree(S,1);

isLocallyFree(S,0);

tst_status(1);$
