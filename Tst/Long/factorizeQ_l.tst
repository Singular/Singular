LIB "tst.lib";
tst_init();

proc testfactors (list l, poly f)
{
  poly g= 1;
  for (int i= 1; i <= size (l[1]); i++)
  {
    g= g*(l[1][i]^l[2][i]);
  }
  g == f;
  l;
}

ring r=0,(x,y,z,s),dp;
list l;
poly f= (1+x+y+z)^20+1;
poly g= f*(f+1);

l= factorize (g); testfactors (l, g);
tst_status();

f= (1+x2+y2+z2)^20+1;
g= f*(f+1);

l= factorize (g); testfactors (l, g);
tst_status();

f= (1+x+y+z)^30+1;
g= f*(f+1);

l= factorize (g); testfactors (l, g);
tst_status();

f= (1+x+y+z+s)^20+1;
g= f*(f+1);

l= factorize (g); testfactors (l, g);

tst_status(1);$
