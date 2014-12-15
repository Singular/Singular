LIB "tst.lib";

ring r=7,(x,y),dp;
poly f=(x^9-2*x^6)*y^9+(-3*x^7+2*x^4)*y^8+(-2*x^11-2*x^8+x^5-2*x^2)*y^7+(-x^9+x^6+x^3+1)*y^6+(-2*x^10+2*x^7+x^4-2*x)*y^5+(-3*x^8+3*x^5-3*x^2)*y^4+(-x^9+3*x^6-2*x^3)*y^3+(-3*x^7+x^4)*y^2+(-x^8-2*x^5)*y+2*x^6;
def l=factorize (f);
l;
poly t= 1;
for (int i= 1; i <= size(l[1]); i++)
{
  t= t*(l[1][i]^l[2][i]);
}
t == f;

ring rng = (7),(xc,xt,xp,xq),dp;
poly f= xt^3*xp*xq^3+xt^2*xp^2*xq^3+xt^4*xq^2+3*xt^3*xp*xq^2-2*xt^2*xp^2*xq^2+3*xt*xp^3*xq^2-3*xt^4*xq-3*xt^3*xp*xq+2*xt^2*xp^2*xq-2*xp^4*xq-2*xt^4+3*xt^3*xp+xt^2*xp^2-3*xp^4;
system ("--random", 22);
factorize (f);

ring rng = (7),(xc,xt,xp,xq),dp;
poly f= xt^3*xp*xq^3+xt^2*xp^2*xq^3+xt^4*xq^2+3*xt^3*xp*xq^2-2*xt^2*xp^2*xq^2+3*xt*xp^3*xq^2-3*xt^4*xq-3*xt^3*xp*xq+2*xt^2*xp^2*xq-2*xp^4*xq-2*xt^4+3*xt^3*xp+xt^2*xp^2-3*xp^4;
system ("--random", 20);
factorize (f);

tst_status(1);$
