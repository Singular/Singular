LIB "tst.lib";
tst_init();

proc fitting(matrix M, int n)
{
  n=nrows(M)-n;
  if(n<=0){return(ideal(1));}
  if((n>nrows(M))||(n>ncols(M))){return(ideal(0));}
  return(std(minor(M,n)));  
}

ring R=0,x(0..4),dp;
matrix M[2][4]=x(0),x(1),x(2),x(3),x(1),x(2),x(3),x(4);
print(M);

fitting(M,-1);

fitting(M,0);

fitting(M,1);

fitting(M,2);

tst_status(1);$
