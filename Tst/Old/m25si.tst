  ring r;
  link l="MPfile:w example.mp"; // type=MPfile, mode=overwrite
  l;
  status(l, "name"); // name is example.mp; link is not yet opened
  ideal i=maxideal(2);
  write (l,1, i, "hello world");// write three expressions
  status(l, "open", "yes");     // now link is open
  status(l, "mode");            // in write mode
  write(l,4);                   // append one more expression
  close(l);                     // link is closed
  read(l);                      // only first expression is read
  status(l, "open", "yes");     // now link is open
  status(l, "mode");            // for reading
  kill r;
  def i = read(l);              // second expression
  listvar();                    // notice that current ring was set
  def s = read(l);              // third expression
  close(l);                     // link is closed
  dump("MPfile:w example.mp");  // dump everything to example.mp
  kill basering, s; listvar();  // kill all vars
  getdump("MPfile: example.mp");// get dump from file
  LIB "general.lib";
  killall("proc");
  kill killall;
  dump("");                     // dump to stdout
  system("sh","rm example.mp");
LIB "tst.lib";tst_status(1);$
