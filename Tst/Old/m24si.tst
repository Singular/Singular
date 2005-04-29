  ring r=32003,(x,y,z),dp;
  link l=":w example.txt";     // type is Ascii, mode is overwrite
  l;
  status(l, "name");           // name is example.txt
  status(l, "open", "yes");    // link is not yet opened
  ideal i=maxideal(2);
  write (l,1,";",2,";","ideal i=",i,";");
  status(l, "open", "yes");    // now link is open
  status(l, "mode");           // for writing 
  write("example.txt","int j=5;");// data is appended to file
  read("example.txt");         // data is returned as string
  close(l);                    // link is closed
  execute(read(l));             // read string is executed
  status(l, "open", "yes");    // now link is open
  status(l, "mode");           // in read mode
  close(l);                    // link is closed
  // dump vars overwriting previous file content:
  dump(":w example.txt");
  kill r;
  getdump("example.txt");      // get dump from file
  LIB "general.lib";
  killall("proc");
  kill killall;
  dump("");                    // dump to stdout
  system("sh","rm example.txt");
LIB "tst.lib";tst_status(1);$
