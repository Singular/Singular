LIB "tst.lib";
tst_init();
LIB"ncModslimgb.lib";
  ring r = 0,(x,y,z),Dp;
  poly F = x^3+y^3+z^3;
  def A  = Sannfs(F);
  setring A;
  ideal I=LD,imap(r,F) ;
  ideal J=ncmodslimgb(I,2);

tst_status(1);$

