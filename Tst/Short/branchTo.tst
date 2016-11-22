LIB "tst.lib";
tst_init();

  proc p1(int i) { "int:",i; }
  proc p21(string s) { "string:",s; }
  proc p22(string s1, string s2) { "two strings:",s1,s2; }
  proc p()
  { branchTo("int",p1);
    branchTo("string","string",p22);
    branchTo("string",p21);
    ERROR("not defined for these argument types");
  }
  p(1);
  p("hu");
  p("ha","ha");

tst_status(1);$;

