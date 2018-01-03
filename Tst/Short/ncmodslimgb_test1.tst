LIB "tst.lib";
tst_init();
LIB"ncModslimgb.lib";
  def r = reiffen(4,5);
  setring r;
  def A = Sannfs(RC);
  setring A;
  ideal bs = LD, imap(r,RC);
  ideal I=ncmodslimgb(bs);
tst_status(1);$
