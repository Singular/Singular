LIB "tst.lib";
tst_init();

// used to crash
proc p()
{
  ring r=0,x,dp;
  return(x);
}
ring RR;
def pp=p();
def pp=p();
tst_status(1);$
