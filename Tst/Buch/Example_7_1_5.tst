LIB "tst.lib";
tst_init();

LIB"matrix.lib";
proc tensorMaps(matrix M, matrix N)
{
   int r=ncols(M);
   int s=nrows(M);
   int p=ncols(N);
   int q=nrows(N);
   int a,b,c,d;
   matrix R[s*q][r*p];
   for(b=1;b<=p;b++)
   {
      for(d=1;d<=q;d++)
      {
         for(a=1;a<=r;a++)
         {
            for(c=1;c<=s;c++)
            {
               R[(c-1)*q+d,(a-1)*p+b]=M[c,a]*N[d,b];
            }
         }
      }
   }
   return(R);
}
proc tensorMod(matrix Phi, matrix Psi)
{
   int s=nrows(Phi);
   int q=nrows(Psi);
   matrix A=tensorMaps(unitmat(s),Psi);  //Is tensor Psi
   matrix B=tensorMaps(Phi,unitmat(q));  //Phi tensor Iq
   matrix R=concat(A,B);                 //sum of A and B
   return(R);     
}

proc Tor(int i, matrix Ps, matrix Ph)
{
   if(i==0){return(module(tensorMod(Ps,Ph)));}  
                                  //the tensor product
   list Phi   = mres(Ph,i+1);     // a resolution of Ph;
   module Im  = tensorMaps(unitmat(nrows(Phi[i])),Ps);
   module f   = tensorMaps(matrix(Phi[i]),unitmat(nrows(Ps)));
   module Im1 = tensorMaps(unitmat(ncols(Phi[i])),Ps);
   module Im2 = tensorMaps(matrix(Phi[i+1]),unitmat(nrows(Ps)));
   module ker = modulo(f,Im);            
   module tor = modulo(ker,Im1+Im2);
   tor        = prune(tor);
   return(tor);
}

ring A=0,(x,y),dp;
matrix  Ps[1][2]=x2,y;
matrix  Ph[1][1]=x;

Tor(1,Ps,Ph);


ideal I1=intersect(ideal(Ps),ideal(Ph));
ideal I2=ideal(Ps)*ideal(Ph);

modulo(I1,I2);


prune(modulo(I1,I2));    

Tor(2,Ps,Ph);

Tor(0,Ps,Ph);    


tst_status(1);$
