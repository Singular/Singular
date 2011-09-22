LIB "tst.lib"; tst_init();
proc type_return (int i)
{
  if (i > 0) {return (i);}
  else {return (list(i));}
}
def t1 = type_return(1);
def t2 = type_return(-1);
typeof(t1); typeof(t2);
tst_status(1);$
