LIB "tst.lib";
tst_init();

// tr.679,683: bug in factorize
LIB"primdec.lib";

proc isSubset(ideal I,ideal J)
{
  if ( attrib(J,"isSB") == 0) { J = groebner(J); }
  for(int i=ncols(I); i>0; i--)
  {
    if(reduce(I[i],J,1)!=0)
    { return(0); }
  }
  return(1);
}
proc primDecsAreEquivalent(list L, list K)
{
  int s1 = size(L);
  int s2 = size(K);
  if(s1!=s2) { return(0); }
  list L1, K1;
  int i,j,t;
  list N;
  for(i=1; i<=s1; i++)
  {
    L1[i]=std(L[i][2]);
    K1[i]=std(K[i][2]);
  }
  for(i=1; i<=s1; i++)
  {
    for(j=1; j<=s1; j++)
    {
      if(isSubset(L1[i],K1[j]))
      {
        if(isSubset(K1[j],L1[i]))
        {
          for(t=1; t<=size(N); t++)
          {
            if(N[t]==j) { return(0); }
          }
          N[size(N)+1]=j;
        }
      }
    }
  }
  if ( size(N) != s1 ) { return(0); }
  return(1);
}

ring r1 = (5,v),(x,y,z,w),dp;
minpoly = (v^2-2);
ideal I = (2*v)*x*y+(v)*x*w+z,(-v)*x^2*z+2*z;
list L1, L2;
int i ;

for (i=0; i<20; i++)
{
    system("random",1803001481);
    L1 = primdecGTZ (I);
    L2 = primdecSY (I);
    "equivalent?",primDecsAreEquivalent (L1,L2) ; 
}

ring r2 = (3,v),(x,y,z,w),dp;
minpoly = (v^2+1);
ideal I = (v)*y-1,-x*y*z+z^2+w;
for (i=1;i<10;i++)
{
list L1 = minAssGTZ(I);
list L2 = minAssGTZ(I,"GTZ");
list L3 = minAssGTZ(I,"facstd");
L1;
L2;
L3;

ideal I1 = std( L1[1] );
ideal I2 = std( L2[1] );
ideal I3 = std( L3[1] );

idealsEqual(I1, I2);
idealsEqual(I2, I3);

kill L1,L2,L3,I1,I2,I3;

poly f=xz+(-v)*z2+(-v)*w;
factorize(f);

kill f;
}

tst_status(1);$
