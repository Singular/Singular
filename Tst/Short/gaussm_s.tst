LIB "tst.lib";
tst_init();

LIB "gaussman.lib";


proc tst_gaussm(poly f)
{
  basering;
  f;
  print(monodromy(f));
  list l=vfiltration(f);
  l;
  vfiltjacalg(l);
  gamma(l);
}


ring R=0,(x,y),ds;
poly f=x5+x2y2+y5;

tst_gaussm(f);

tst_status(1);
$
