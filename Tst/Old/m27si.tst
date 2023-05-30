  // create an empty date base:
  system("sh","rm -f example.dir example.pag");
  system("sh","touch example.dir example.pag");
  link dbmLink="DBM:rw example";
  write(dbmLink,"1","abc");
  write(dbmLink,"3","XYZ");
  write(dbmLink,"2","ABCc");
  dbmLink;
  close(dbmLink);
  // read all keys (till empty string):
  read(dbmLink);
  read(dbmLink);
  read(dbmLink);
  read(dbmLink);
  // read data corresponding to key "1"
  read(dbmLink,"1");
  // read all data:
  read(dbmLink,read(dbmLink));
  read(dbmLink,read(dbmLink));
  read(dbmLink,read(dbmLink));
  // close
  close(dbmLink);
  int dummy=system("sh","rm -f example.dir example.pag");
LIB "tst.lib";tst_status(1);$
