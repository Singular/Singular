LIB "tst.lib";
tst_init();

proc sig11()
{
  ring r;
  ideal i=x;
  return(i);
}
sig11();

tst_status(1);$
