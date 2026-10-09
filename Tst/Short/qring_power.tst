LIB "tst.lib";
tst_init();

// Ordinary polynomial powering remains unchanged.
ring r=0,(x),dp;
size((1+x)^20);

// Quotient powering normalizes even without qringNF; multiplication is unchanged.
qring q=std(ideal(x2));
option(noqringNF);
poly p=1+x+x2;
size(p);
size(p^1);
size(p^2);
size(p*p);
size(p*p*p);
reduce(p*p-(1+2*x),std(0))==0;

// The dedicated switch restores the old raw representatives.
option(noqringPower);
size(p^1);
size(p^2);
size(p*p);
option(qringNF);
poly legacy=(p^2);
size(legacy);
option(noqringNF,qringPower);
size(p^2);
option(noqringPower);
intvec saved_options=option(get);
option(qringPower);
size(p^2);
option(set,saved_options);
size(p^2);
option(none);
size(p^2);

// The default option display stays unchanged; only the opt-out is displayed.
option();
option(noqringPower);
option();
option(qringPower);
option();

// qringNF remains compatible with the normalized arithmetic results.
option(qringNF);
attrib(p^2,"qringNF");
option(noqringPower);
attrib(p^2,"qringNF");
option(qringPower);
(p^0)==1;
(p^1)==1+x;
poly z=0;
(z^0)==1;
(z^17)==0;
poly h=(1+x)^100000;
size(h);
h==(1+100000*x);
reduce(h-(1+100000*x),std(0))==0;

// Compare binary powering with repeated normal-form multiplication.
poly expected=1;
int failures=0;
for (int e=0; e<=16; e++)
{
  if (reduce(p^e-expected,std(0))!=0)
  {
    failures=failures+1;
  }
  expected=reduce(expected*p,std(0));
}
failures;

// An inhomogeneous quotient relation.
ring ri=0,(u),dp;
qring qi=std(u2-u);
option(qringNF);
poly f=1+u;
f^8==1+255*u;

// Finite characteristic and a local ordering.
ring rl=32003,(x,y),ds;
qring ql=std(ideal(x2,y2));
poly l=(1+x+y)^5;
size(l);
l==(1+5*x+5*y+20*x*y);
reduce(l-(1+5*x+5*y+20*x*y),std(0))==0;

// The unit ideal also normalizes the exponent-zero identity.
ring ru=0,(v),dp;
qring qu=std(ideal(1));
option(noqringNF);
poly unitbase=1+v;
size(unitbase^0);
size(unitbase^1);

// Detect overflow in a high-degree tail with a local ordering.  The leading
// term is constant, so checking only the leading monomial would miss it.
ring ro0=0,(x(1..11)),ds;
list RL=ringlist(ro0);
RL[4]=std(x(2));
attrib(RL,"maxExp",31);
def ro=ring(RL);
setring ro;
poly o=1+x(1)^8;
o^2;
"overflow was caught";

// A qring with the zero ideal behaves like its origin ring.
ring rz=0,(x),dp;
qring qz=std(ideal(0));
size((1+x)^20);

tst_status(1);$
