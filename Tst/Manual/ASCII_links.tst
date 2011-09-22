LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),dp;
  link l=":w example.txt";     // type is ASCII, mode is overwrite
  l;
  status(l, "open", "yes");    // link is not yet opened
  ideal i=x2,y2,z2;
  write (l,1,";",2,";","ideal i=",i,";");
  status(l, "open", "yes");    // now link is open
  status(l, "mode");           // for writing
  close(l);                    // link is closed
  write("example.txt","int j=5;");// data is appended to file
  read("example.txt");         // data is returned as string
  execute(read(l));            // read string is executed
  close(l);                    // link is closed
tst_status(1);$
