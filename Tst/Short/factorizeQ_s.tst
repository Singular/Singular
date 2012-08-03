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

//tr. 439
ring rQ=0,(u,v),dp;
poly f = u*(3v+1)^2;
list l= factorize(f);
testfactors (l,f);

tst_status(1);$
