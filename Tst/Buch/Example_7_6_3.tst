LIB "tst.lib";
tst_init();


proc isReg(ideal f, module N)
{
  int n=nrows(N);
  int i;
  while(i<size(f))
  {
     i++;
     N=std(N);
     if(size(reduce(quotient(N,f[i]),N))!=0){return(0);}
     N=N+f[i]*freemodule(n);
  }
  return(1);
}

ring R=0,(x,y,z),dp;
ideal f=x*(y-1),y,z*(y-1);
module N=0;
isReg(f,N);

f=x*(y-1),z*(y-1),y; 
isReg(f,N);


tst_status(1);$
