LIB "tst.lib";
tst_init();

proc fitting(matrix M, int n)
{
  n=nrows(M)-n;
  if(n<=0){return(ideal(1));}
  if((n>nrows(M))||(n>ncols(M))){return(ideal(0));}
  return(std(minor(M,n)));  
}

proc isFlat(matrix M)
{
   if(size(ideal(M))==0){return(1);}
   int w;
   ideal F=fitting(M,0);
   while(size(F)==0)
   {
      w++;
      F=fitting(M,w);
   }
   if(deg(std(F)[1])==0){return(1);}
   return(0);
}

ring A=0,(x,y),dp;
matrix M[3][3]=x-1,y,x,x,x+1,y,x2,xy+x+1,x2+y;
print(M);
qring B=std(x2+x-y);   
matrix M=fetch(A,M);
isFlat(M);
setring A;
qring C=std(x2+x+y);      
matrix M=fetch(A,M);
isFlat(M);

proc flatLocus(matrix M)
{
   if(size(ideal(M))==0){return(ideal(1));}
   int v,w;
   ideal F=fitting(M,0);
   while(size(F)==0)
   {
      w++;
      F=fitting(M,w);
   }
   if(typeof(basering)=="qring")
   {
      for(v=w+1;v<=nrows(M);v++)
      {
         F=F+intersect(fitting(M,v),quotient(ideal(0),
         fitting(M,v-1)));
      }
   }
   return(interred(F));
}

ring R=0,(x,y,z),dp;
qring S=std(xyz);
matrix M[3][3]=x,y,z,0,0,0,xyz,x3,z3;

flatLocus(M);

tst_status(1);$
