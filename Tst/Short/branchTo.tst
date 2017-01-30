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
//---------------------------------------------------------------
newstruct("Net","list rows");

proc printNet(Net N)
{
  list L = N.rows;
  for (int j=1; j<=size(L); j++)
  {
    print(L[j]);
  }
}

system("install","Net","print",printNet,1);

proc netString(string M)
{
  Net N;
  list L;
  L[1]=M;
  N.rows=L;
  return(N);
}

proc net() {branchTo("string",netString);}

typeof(net("abc"));
Net N = net("abc");

tst_status(1);$;

