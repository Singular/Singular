LIB "tst.lib";
tst_init();

LIB"primdec.lib";

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

proc binom (int n, int k)
{
   int l;
   int r=1;
   if ( k > n-k )
   { k = n-k;
   }
   if ( k<=0 or k>n )               //trivial cases
   { r = (k==0)*r;
   }
   for (l=1; l<=k; l++ )
   {
      r=r*(n+1-l) div l;
   }
   return(r);
}
proc basisNumber(int n,intvec v)
{
   int p=size(v);
   if(p==1){return(v[1]);}
   int j=n-1;
   int b;
   while(j>=n-v[1]+1)
   {
      b=b+binom(j,p-1);
      j--;
   }
   intvec w=v-v[1];
   w=w[2..size(w)];
   b=b+basisNumber(n-v[1],w);
   return(b);
}


proc basisElement(int n,int p,int N)
{
   if(p==1){return(N);}
   int s,R;
   while(R<N)
   {
      s++;
      R=R+binom(n-s,p-1);
   }
   R=N-R+binom(n-s,p-1);
   intvec v=basisElement(n-s,p-1,R);
   intvec w=s,v+s;
   return(w);
}

proc KoszulMap(ideal x,int p)
{
  int n=size(x);
  int a=binom(n,p-1);
  int b=binom(n,p);
  matrix M[a][b];
  if(p==1){M=x;return(M);}
  int j,k;
  intvec v,w;
  for(j=1;j<=b;j++)
  {
     v=basisElement(n,p,j);
     w=v[2..p];
     M[basisNumber(n,w),j]=x[v[1]];
     for(k=2;k<p;k++)
     {
        w=v[1..k-1],v[k+1..p];
        M[basisNumber(n,w),j]=(-1)^(k-1)*x[v[k]];
     }
     w=v[1..p-1];
     M[basisNumber(n,w),j]=(-1)^(p-1)*x[v[p]];
  }
  return(M);
}

proc KoszulHomology(ideal x, module M, int p)
{
   int n      = size(x);
   int a      = binom(n,p-1);
   int b      = binom(n,p);
   matrix N   = matrix(M);
   module ker = freemodule(nrows(N));
   if(p!=0)
   {
      module im  = tensorMaps(unitmat(a),N);
      module f   = tensorMaps(KoszulMap(x,p),unitmat(nrows(N)));
      ker        = modulo(f,im);
   }
   module im1 = tensorMaps(unitmat(b),N);
   module im2 = tensorMaps(KoszulMap(x,p+1),unitmat(nrows(N)));
   module hom = modulo(ker,im1+im2);
   hom        = prune(hom);
   return(hom);
}

proc depth(module M)
{
   ideal m=maxideal(1);
   int n=size(m);
   int i;
   while(i<n)
   {
     i++;
     if(size(KoszulHomology(m,M,i))==0){return(n-i+1);}
   }
   return(0);
}

proc CohenMacaulayTest(module M)
{
  return(depth(M)==dim(std(Ann(M))));
}


ring R=0,(x,y,z),ds;
ideal I=xz,yz;
module M=I*freemodule(1);
CohenMacaulayTest(M);

I=x2+y2,z7;
M=I*freemodule(1);
CohenMacaulayTest(M);

tst_status(1);$
