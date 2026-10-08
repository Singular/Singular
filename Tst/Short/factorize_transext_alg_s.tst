LIB "tst.lib";
tst_init();

LIB "primdec.lib";

proc factorProduct(list L)
{
  poly g = 1;
  int i;
  for (i = 1; i <= size(L[1]); i++)
  {
    g = g * L[1][i]^L[2][i];
  }
  return(g);
}

ring K=(0,a),(t),dp;
minpoly=a2+a+1;
number aa=a;

ring R=K,(x,y),dp;
number aaR=imap(K,aa);
poly f=(x+par(1))^2*(y+aaR);

aaR^2+aaR+1;

list F=factorize(f,2);
F;
factorProduct(F)-f;

ideal I=f;
list P=primdecGTZ(I);
P;
testPrimary(P,I);

tst_status(1);$
