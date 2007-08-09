LIB "tst.lib";
tst_init();

// interpolation test:

list l;
intvec v;
interpolation(l,v);
int n=100;
int m=10;
int j;
ring r=2,x(1..n),dp;
ideal p;
for(int i=1;i<m;i++)
{
    for(j=1;j<=n;j++)
    {
      p[j]=x(j)+random(0,1);
    }
    l[i]=p;
    v[i]=1;
}
interpolation(l,v);

tst_status(1);$
