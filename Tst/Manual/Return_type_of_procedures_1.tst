LIB "tst.lib"; tst_init();
proc def_return
{
  ring r=0,(x,y),dp;
  poly p = x;
  keepring r;
  return (x);
}
def p = def_return();
// poly p = def_return(); would be WRONG!!!
typeof(p);
tst_status(1);$
