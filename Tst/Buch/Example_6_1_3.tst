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

ring R=0,(x,y),ds;
poly p=2+x+y2;
poly q=invers(p,4);
q;

jet(p*q,4);

tst_status(1);$
        