LIB "tst.lib";
tst_init();

proc t
{
  ring r;
  poly p=x;
  poly q=y;
  export p;
  return(r);
}
ring s;
def rr=t();
listvar(all);
// p should appear, but q not
tst_status(1);$
