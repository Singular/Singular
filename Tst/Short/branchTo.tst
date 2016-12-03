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
// returning stuff:
proc square(int i){return(i^2);};
proc tst(){branchTo("int",square); ERROR("No method found");}
tst(2);

tst_status(1);$;

