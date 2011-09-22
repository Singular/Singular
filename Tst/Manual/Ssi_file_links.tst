LIB "tst.lib"; tst_init();
  ring r;
  link l="ssi:w example.ssi"; // type=ssi, mode=overwrite
  l;
  ideal i=x2,y2,z2;
  write (l,1, i, "hello world");// write three expressions
  write(l,4);                   // append one more expression
  close(l);                     // link is closed
  // open the file for reading now
  read(l);                      // only first expression is read
  kill r;                       // no basering active now
  def i = read(l);              // second expression
  // notice that current ring was set, the name was assigned
  // automatically
  listvar(ring);
  def s = read(l);              // third expression
  listvar();
  close(l);                     // link is closed
tst_status(1);$
