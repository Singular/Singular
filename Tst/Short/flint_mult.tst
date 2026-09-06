LIB "tst.lib";
tst_init();

// Collision-heavy rational supports: the new Q selector uses FLINT below the
// former fixed length cutoff.  Construct the expected convolution without a
// polynomial-by-polynomial multiplication.
ring rq=0,(x1,x2,x3,x4),dp;
poly pq=0;
poly qq=0;
poly expectedq=0;
number cq,dq;
int i,j,e;
for (i=0; i<30; i++)
{
  cq=i+1;
  cq=cq/(i+2);
  pq=pq+cq*x1^i;
}
for (j=0; j<45; j++)
{
  dq=2*j+1;
  dq=dq/(2*j+3);
  qq=qq+dq*x1^j;
}
for (i=0; i<30; i++)
{
  cq=i+1;
  cq=cq/(i+2);
  for (j=0; j<45; j++)
  {
    dq=2*j+1;
    dq=dq/(2*j+3);
    expectedq=expectedq+cq*dq*x1^(i+j);
  }
}
pq*qq==expectedq;

// Five-variable dense homogeneous support exercises the conservative
// prediction of FLINT's DEG-array multiplication path.
ring rqa=0,(x1,x2,x3,x4,x5),dp;
poly linear=x1+x2+x3+x4+x5;
poly arrayq=linear^8;
poly expectedarrayq=1;
for (i=0; i<16; i++)
{
  expectedarrayq=expectedarrayq*linear;
}
arrayq*arrayq==expectedarrayq;

// In five and six variables, retain the former long-input cutoff: conversion
// is still cheap enough that collision-free products can favor FLINT.
ring rqm=0,(x1,x2,x3,x4,x5,x6),dp;
poly pm=0;
poly qm=0;
poly expectedm=0;
for (i=0; i<65; i++)
{
  pm=pm+x1^i;
  qm=qm+x1^(65*i);
}
for (i=0; i<65; i++)
{
  for (j=0; j<65; j++)
  {
    e=i+65*j;
    expectedm=expectedm+x1^e;
  }
}
pm*qm==expectedm;

// Collision-free support in a larger ambient ring: the old length cutoff used
// FLINT, while the new selector retains Singular's bucket multiplication.
ring rqh=0,(x1,x2,x3,x4,x5,x6,x7,x8),dp;
poly ph=0;
poly qh=0;
poly expectedh=0;
for (i=0; i<65; i++)
{
  ph=ph+x1^i;
  qh=qh+x1^(65*i);
}
for (i=0; i<65; i++)
{
  for (j=0; j<65; j++)
  {
    e=i+65*j;
    expectedh=expectedh+x1^e;
  }
}
ph*qh==expectedh;

// The corresponding Z decisions use a lower small-input crossover because
// there are no rational denominators to normalize.
ring rz=integer,(x1,x2,x3,x4),dp;
poly pz=0;
poly qz=0;
poly expectedz=0;
number cz,dz;
for (i=0; i<12; i++)
{
  cz=i+2;
  pz=pz+cz*x1^i;
}
for (j=0; j<40; j++)
{
  dz=2*j+3;
  qz=qz+dz*x1^j;
}
for (i=0; i<12; i++)
{
  cz=i+2;
  for (j=0; j<40; j++)
  {
    dz=2*j+3;
    expectedz=expectedz+cz*dz*x1^(i+j);
  }
}
pz*qz==expectedz;

ring rzh=integer,(x1,x2,x3,x4,x5,x6,x7,x8),dp;
poly pzh=0;
poly qzh=0;
poly expectedzh=0;
for (i=0; i<65; i++)
{
  pzh=pzh+x1^i;
  qzh=qzh+x1^(65*i);
}
for (i=0; i<65; i++)
{
  for (j=0; j<65; j++)
  {
    e=i+65*j;
    expectedzh=expectedzh+x1^e;
  }
}
pzh*qzh==expectedzh;

// Z/p keeps its existing selector.  In characteristic two this identity also
// verifies all coefficients of a FLINT-sized product with cancellation.
ring rp=2,(x1,x2,x3,x4),dp;
poly pp=0;
poly expectedp=0;
for (i=0; i<96; i++)
{
  pp=pp+x1^i;
  expectedp=expectedp+x1^(2*i);
}
pp*pp==expectedp;

tst_status(1);$
