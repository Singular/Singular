LIB "tst.lib";
tst_init();

proc invers(poly p, int k)
{
    poly q=1/p[1];
    poly re=q;
    p=q*(p[1]-jet(p,k));
    poly s=p;
    while(p!=0)
    {
       re=re+q*p;
       p=jet(p*s,k);
     }
     return(re);
}

proc Weierstrass(poly f,poly g,int k)
{
   int i;
   int n=nvars(basering);
   poly p=f;
   for(i=1;i<=n-1;i++)
   {
      p=subst(p,var(i),0);
   }
   if(p==0)
   {
      "the polynomial is not regular";
      return(0);
   }
   int m=ord(p);
   poly hf=f/var(n)^m;
   poly rf=f-var(n)^m*hf;
   poly invhf=invers(f/var(n)^m,k);
   poly w=-invhf*rf;
   poly u=g/var(n)^m;
   poly v=u;
   poly H=jet((w*u)/var(n)^m,k);
   while(H!=0)
   {
      v=v+H;
      H=jet((w*H)/var(n)^m,k);
   }
   poly q=v*invhf;
   return(q);
}

ring R=0,(x,y),ds;
poly f=y4+xy+x2y6+x7;
poly g=y4;
poly q=Weierstrass(f,g,10);
poly w=jet(q*f,10); 
ring S=(0,x),y,ds;
poly w=imap(R,w);
w;

setring R;
q=Weierstrass(f,g,15);
w=jet(q*f,15);  
setring S;
w=imap(R,w);
w;


tst_status(1);$
