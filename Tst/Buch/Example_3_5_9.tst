LIB "tst.lib";
tst_init();

proc d(ideal I)
{
   int n=nvars(basering);
   int j,b,a;
   I=simplify(I,2);  //cancels zeros in the generators of I
   if(size(I)==0) {return(n);}  //size counts the generators 
                                //not equal to 0
   for(j=1;j<=n;j++)    
   {
     if(I[1]/var(j)!=0)
     {
        a=d(subst(I,var(j),0))-1;
        //we need -1 here because we stay in the basering
        if(a>b) {b=a;}
     }
   }
   return(b);
}

ring R=0,(x,y,z),dp;
ideal I=yz,xz;

d(I);

dim(std(I));

tst_status(1);$
