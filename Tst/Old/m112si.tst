  ring r=32003,(x,y,z),dp;
  ideal i=x+y,z3+22y;
  // write the ideal i to the file save_i
  write(":w save_i",i);
  // @dots{}
  ring r0=0,(x,y,z),Dp;
  // create an ideal k equal to the content
  // of the file save_i
  string s="ideal k="+read("save_i")+";";
  execute(s);
  k;
LIB "tst.lib";tst_status(1);$
