LIB "tst.lib";
tst_init();

LIB "gaussman.lib";


proc tst_gaussm(poly f)
{
  basering;
  f;
  print(monodromy(f));
  list l=vwfilt(f);
  l;
  l=spnf(l[1],l[3],l[4])+list(l[5],l[6]);
  endvfilt(l);
  spgamma(l);
  l=tmatrix(f);
  print(l[1]);
  print(l[2]);
}


ring R=0,(x,y),ds;
poly f=x5+x2y2+y5;

tst_gaussm(f);

tst_status(1);
$
