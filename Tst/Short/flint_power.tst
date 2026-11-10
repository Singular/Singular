LIB "tst.lib";
tst_init();

// Exercise the FLINT-backed p_Power paths over Q and Zp, comparing each result
// with ordinary repeated multiplication.
ring rq=0,(x,y,z),dp;
// This low-dimensional support exercises the new Q heuristic below the old
// fixed exponent threshold.
poly hq=1/2*x2+2/3*xy+5/7*y2;
poly expectedh=1;
int i;
for (i=1; i<=12; i++)
{
  expectedh=expectedh*hq;
}
poly actualh=hq^12;
actualh==expectedh;

poly fq=1/2*x2-2/3*yz+5*z2+7;
poly expectedq=1;
for (i=1; i<=17; i++)
{
  expectedq=expectedq*fq;
}
poly actualq=fq^17;
actualq==expectedq;

ring rp=32003,(x,y,z),dp;
poly fp=2*x2+3*xy-5*yz+7;
poly expectedp=1;
for (i=1; i<=18; i++)
{
  expectedp=expectedp*fp;
}
poly actualp=fp^18;
actualp==expectedp;

tst_status(1);$
