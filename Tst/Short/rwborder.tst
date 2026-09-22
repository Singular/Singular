//
// kernel border bases in commutative and rational Weyl algebras
//
LIB "tst.lib";
LIB "rwborder.lib";
LIB "rstandard.lib";
tst_init();

ring r=0,(x,y),dp;
ideal I=x2-y,y2-x;
list C=kernelBorderBasis(I);
if (size(C[1])!=4 || size(C[2])!=4)
{
  ERROR("unexpected commutative border or order-ideal size");
}
ideal Cold=borderBasis(I);
if (size(Cold)!=size(C[1]))
{
  ERROR("kernel and library border sizes disagree");
}
ideal R=reduce(C[1],std(I));
int i;
for (i=1;i<=size(R);i++)
{
  if (R[i]!=0) { ERROR("commutative border relation is not in the ideal"); }
}

ring rl=0,(u,v),ls;
ideal IL=u2,v2;
ideal BL=borderBasis(IL);
if (size(BL)!=4)
{
  ERROR("rstandard local-ordering border loop failed");
}

proc testRationalWeylBorderBasis()
{
  if (!system("with","ratGB")) { return(); }
  ring r1=0,(x,D),(a(0,1),dp);
  def W1=Weyl(); setring W1;
  ideal I=x*D-1;
  ideal BI=borderBasis(I,1,1);
  list B=rationalWeylBorderBasisFromGB(pGBid,1);
  if (size(B[1])!=1 || size(B[2])!=1 || B[1][1]!=(x*D-1) || B[2][1]!=1)
  {
    ERROR("unexpected rank-one rational Weyl border basis");
  }
  if (size(BI)!=1 || BI[1]!=(x*D-1))
  {
    ERROR("rstandard rational Weyl dispatch failed");
  }
  kill B,BI,I,pGBid;
  kill Ratgb::Ddim,Ratgb::KXdim;

  // Rodriguez--Sattelberger, Border Bases in the Rational Weyl Algebra,
  // arXiv:2510.23411v2, Example 2.13.  The paper's displayed J_O1 is used
  // as input.  For J_O2, consistency requires a plus sign in front of
  // (3*y-x)/(y*(y-x))*Dy; the preprint currently prints a minus sign.
  ring rp1=0,(x,y,Dx,Dy),(a(0,0,1,1),a(0,0,0,1),dp);
  def WP1=Weyl(); setring WP1;
  ideal JP=
    x*(x-y)*Dx^2+(3*x-y)*Dx+1,
    y*(y-x)*Dx*Dy+(x+y)*Dx+1,
    y*Dy+x*Dx+1;
  list BP1=rationalWeylBorderBasis(JP,2,1);
  if (size(BP1[1])!=3 || size(BP1[2])!=2 ||
      BP1[2][1]!=1 || BP1[2][2]!=Dx ||
      BP1[1][1]!=(y*Dy+x*Dx+1) ||
      BP1[1][2]!=(-y*(y-x)*Dx*Dy-(x+y)*Dx-1) ||
      BP1[1][3]!=(x*(x-y)*Dx^2+(3*x-y)*Dx+1))
  {
    ERROR("paper Example 2.13: O1 border basis mismatch");
  }
  kill pGBid;
  kill Ratgb::Ddim,Ratgb::KXdim;

  ring rp2=0,(x,y,Dx,Dy),(a(0,0,1,1),a(0,0,1,0),dp);
  def WP2=Weyl(); setring WP2;
  ideal JP=imap(WP1,JP);
  list BP2=rationalWeylBorderBasis(JP,2,1);
  if (size(BP2[1])!=3 || size(BP2[2])!=2 ||
      BP2[2][1]!=1 || BP2[2][2]!=Dy ||
      BP2[1][1]!=(-y*(y-x)*Dy^2-(3*y-x)*Dy-1) ||
      BP2[1][2]!=(x*Dx+y*Dy+1) ||
      BP2[1][3]!=(x*(x-y)*Dx*Dy+(x+y)*Dy+1))
  {
    ERROR("paper Example 2.13: O2 border basis mismatch");
  }
  kill pGBid;
  kill Ratgb::Ddim,Ratgb::KXdim;

  // Rodriguez--Sattelberger, Example 2.5.  The ideal is symmetric, but
  // {1,Dx,Dy,Dx*Dy} cannot be a quotient basis since Dx*Dy=1 modulo I.
  // The term order instead yields the valid nonsymmetric order ideal below.
  ring rp3=0,(x,y,Dx,Dy),(a(0,0,1,1),a(0,0,1,0),dp);
  def WP3=Weyl(); setring WP3;
  ideal IP=Dx^2+Dy^2-2,Dx*Dy-1;
  list BP3=rationalWeylBorderBasis(IP,2,1);
  if (size(BP3[1])!=4 || size(BP3[2])!=4 ||
      BP3[2][1]!=1 || BP3[2][2]!=Dy ||
      BP3[2][3]!=Dy^2 || BP3[2][4]!=Dx ||
      BP3[1][1]!=(Dy^3+Dx-2*Dy) ||
      BP3[1][2]!=(Dx*Dy-1) ||
      BP3[1][3]!=(Dx*Dy^2-Dy) ||
      BP3[1][4]!=(Dx^2+Dy^2-2))
  {
    ERROR("paper Example 2.5 border basis mismatch");
  }
  kill pGBid;
  kill Ratgb::Ddim,Ratgb::KXdim;

  ring r2=(0,c),(x,y,Dx,Dy),(a(0,0,1,1),a(0,0,1,0),dp);
  def W2=Weyl(); setring W2;
  ideal I=
    x*Dx*(x*Dx+c-1)-x*(x*Dx+y*Dy-2)*(x*Dx+y*Dy-1),
    y*Dy*(y*Dy-1)-y*(x*Dx+y*Dy-2)*(x*Dx+y*Dy-1);
  def A=ratstd(I,2,1);
  list B=rationalWeylBorderBasisFromGB(pGBid,2);
  if (size(B[1])!=4 || size(B[2])!=4)
  {
    ERROR("unexpected Appell F4 border or order-ideal size");
  }
  poly h;
  for (i=1;i<=size(B[1]);i++)
  {
    h=system("intratNF",B[1][i],pGBid,2);
    if (h!=0) { ERROR("rational Weyl border relation is not in the ideal"); }
  }
}
testRationalWeylBorderBasis();

tst_status(1);$
