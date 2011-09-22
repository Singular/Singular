LIB "tst.lib"; tst_init();
int i;int N=2;
ring r=0,(x(1..N),d(1..N),q(1..N)),Dp;
matrix D[3*N][3*N];
for (i=1;i<=N;i++)
{ D[i,N+i]=q(i)^2; }
def W=nc_algebra(1,D); setring W;
// this algebra is a kind of homogenized Weyl algebra
W;
ideal I = x(1),x(2),d(1),d(2),q(1),q(2);
option(redSB);
option(redTail);
resolution R = mres(I,0);
// thus R will be the full length minimal resolution
print(betti(R),"betti");
tst_status(1);$
