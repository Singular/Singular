LIB "tst.lib"; tst_init();

LIB "gfan.lib";

proc getfirst(fan f)
{
  int dimf = dimension(f);
  cone c = getCone(f, dimf, 1, 1);
  return(c);
}

intmat M[3][3]=
1,0,0,
0,1,0,
0,0,1;

cone c = coneViaPoints(M);
fan f = fanViaCones(c);
f;
getfirst(f);
kill f;

tst_status(1);$
