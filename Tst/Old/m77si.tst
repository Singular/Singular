  ring R;
  poly f=x^2+y^3+z^5;
  jacob(f);
  ideal i=jacob(f);
  print(jacob(i));
LIB "tst.lib";tst_status(1);$
