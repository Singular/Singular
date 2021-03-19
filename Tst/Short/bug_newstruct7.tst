LIB "tst.lib";
tst_init();

proc testproc()
{
  ring S = 32003,x,dp;
  ideal I = 2x;
  teststruct output;
  output.I = I;
  return(output);
}
newstruct("teststruct", "ideal I");
ring R = 0,a,dp;
def erg = testproc();
erg.I;
erg.I;
def rr=erg.r_I;
setring rr;
erg.I;


tst_status(1);$

