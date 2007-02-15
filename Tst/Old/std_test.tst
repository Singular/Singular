proc check_ideal_std(int n, string ostring)
{
  string ringstr = "ring r = 32003,x(0.." + string(n-1) + ")," + ostring + ";";
  string idealstr = "ideal j = ";
  int i;

  for (i=0; i < n-1; i++)
  {
    idealstr = idealstr + "x("+ string(i) + ") + ";
  }
  idealstr = idealstr + "x(" + string(n-1) + "),";

  
  for (i=0; i < n-1; i++)
  {
    idealstr = idealstr + "x("+ string(i % n) + ")*x(" + string((i+1) % n) + ") + ";
  }
  idealstr = idealstr + "x("+ string(i % n) + ")*x(" + string((i+1) % n) + "),";

  for (i=0; i < n-1; i++)
  {
    idealstr = idealstr + "x("+ string(i % n) + ")*x(" + string((i+1) % n)
      + ")*x(" + string((i+2) % n) + ")  + ";
  }
  idealstr = idealstr + "x("+ string(i % n) + ")*x(" + string((i+1) % n)
    + ")*x(" + string((i+2) % n) + ");";

  execute(ringstr);
  execute(idealstr);
  r;
  j;
  std(j);
}

  
proc check_module_std(int n, string ostring)
{
  "check_module_std n=",n," ostring=",ostring;
  string ringstr = "ring r = 32003,x(0.." + string(n-1) + ")," + ostring + ";";
  ringstr;
  string modulestr = "module j = ";
  int i;

  for (i=0; i < n-1; i++)
  {
    modulestr = modulestr + "x("+ string(i) + ")*gen(" +  string(i+1) + ") + ";
    modulestr = modulestr + "x("+ string(i) + ")*gen(" +  string(i+2) + ") + ";
  }
  modulestr = modulestr + "x(" + string(n-1) + ")*gen(" +  string(n) + "), ";

  
  for (i=0; i < n-1; i++)
  {
    modulestr = modulestr + "x("+ string(i % n) + ")*x(" + string((i+1) % n) +
      ")*gen(" +  string(1) + ") + ";
    modulestr = modulestr + "x("+ string(i % n) + ")*x(" + string((i+1) % n) +
      ")*gen(" +  string(2) + ") + ";
  }
  modulestr = modulestr + "x("+ string((n-1) % n) + ")*x(" + string(n % n) +
    ")*gen(" +  string((i%n)+1) + "),";

  for (i=0; i < n-1; i++)
  {
    modulestr = modulestr + "x("+ string(i % n) + ")*x(" + string((i+1) % n)
      + ")*x(" + string((i+2) % n) + ")*gen(" +  string(1) + ")  + ";
    modulestr = modulestr + "x("+ string(i % n) + ")*x(" + string((i+1) % n)
      + ")*x(" + string((i+2) % n) + ")*gen(" +  string(2) + ")  + ";
  }
  modulestr = modulestr + "x("+ string(i % n) + ")*x(" + string((i+1) % n)
    + ")*x(" + string((i+2) % n) + ")*gen(" +  string(n) + ");";

  execute(ringstr);
  execute(modulestr);
  r;
  j;
  std(j);
}

proc check_std_all_1(int n, string ostring)
{
  check_ideal_std(n, ostring);
  check_module_std(n, "(C," + ostring + ")");
  check_module_std(n, "(c," + ostring + ")");
  check_module_std(n, "(" + ostring + ",C)");
  check_module_std(n, "(" + ostring + ",c)");
}

list orderings = "lp", "dp", "Dp", "ls", "ds", "Ds";

list l = "dp";

  
proc check_std(int n, list olist)
{
  int i;
  int j;
  for (i=1; i<=size(olist); i++)
  {
    for (j=1; j<=n; j++)
    {
      check_ideal_std(j, olist[i]);
    }
  }
}

proc check_std_all(int n, list olist)
{
  int i;
  int j;
  for (i=1; i<=size(olist); i++)
  {
    for (j=1; j<=n; j++)
    {
      check_std_all_1(j, olist[i]);
    }
  }
}

option(prot);

check_std_all(9, orderings);

LIB "tst.lib";tst_status(1);$

