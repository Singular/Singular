LIB "tst.lib";
tst_init();

//input script that crashes Singular
//  neumann@ms.unimelb.edu.au (Walter D. Neumann)
ring r=0,(x,y,z,u,v,a,b),lp;
option(prot);
poly f=x6y4-x-a;poly g=x9y6-x-b;
poly s=z-ux-vy;
ideal id=s,f;
poly f1=eliminate(id,y)[1];f1;
ideal id=s,g; 
poly g1=eliminate(id,y)[1];g1;
poly h=resultant(f1,g1,x);h;
factorize(h);
poly zpol=_[1][2];
int i; int j; int k;
for (i=6; i>=0; i=i-1)
{
for( j=0; j<=6-i; j=j+1)
{
k=6-i-j;
i,j,k;
factorize(zpol/(z^i*u^j*v^k));
}
}

tst_status(1);$
