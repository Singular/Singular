LIB "tst.lib";
tst_init();

proc fitting(matrix M, int n)
{
  n=nrows(M)-n;
  if(n<=0){return(ideal(1));}
  if((n>nrows(M))||(n>ncols(M))){return(ideal(0));}
  return(std(minor(M,n)));  
}

proc flatteningStrat(matrix M)
{ 
   list l;
   int v,w;
   ideal F;
   while(1)
   {
      F=interred(fitting(M,w));
      if(F[1]==1){return(l);}
      if(size(F)!=0){v++;l[v]=F;}
      w++;
   }
   return(l);
}

ring R=0,x(0..4),dp;
matrix M[2][4]=x(0),x(1),x(2),x(3),
               x(1),x(2),x(3),x(4);

flatteningStrat(M);

tst_status(1);$
