LIB "tst.lib";
tst_init();

newstruct("ideal_in_another_ring", "ideal I");

proc ttt()
{
  ring r;
  ideal I = 3*x;
  ideal_in_another_ring s;
  s.I = I;
  return(s);
}

LIB "parallel.lib";
list out = parallelWaitAll("ttt", list(list()));
def s = out[1];
def r = s.r_I;
setring r;
ideal I = s.I;
setring r;
ideal I = s.I;
I;
listvar();
tst_status(1);$
