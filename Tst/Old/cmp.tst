proc generate_polys (int n, string ostring)
{
  string ringstr = "ring r = 32003,x(1.." + string(n) + ")," + ostring + ";";
  string polystr = "poly p = ";
  int i;

  for (i=1; i<=n; i++)
  {
    polystr = polystr + "x(" + string(i) + ") + ";
  }
  polystr = polystr + "1;";
  ringstr;
  polystr;
  execute ringstr;
  execute polystr;
  p;
  p^2 + p;
  (p^2+p)*(gen(2) + gen(3));
  (p^2+p)*(gen(2) + gen(3))+ p^2 + p;
  kill r;
}

proc check_ordering(int n, string ostring)
{
  generate_polys(n, ostring);
  generate_polys(n, "(C," + ostring + ")");
  generate_polys(n, "(c," + ostring + ")");
  generate_polys(n, "(" + ostring + ",C)");
  generate_polys(n, "(" + ostring + ",c)");
}

list orderings = "lp", "dp", "Dp", "ls", "ds", "Ds";

list l = "dp";

proc check_comp(int n, list olist)
{
  int i;
  int j;
  for (i=1; i<=size(olist); i++)
  {
    for (j=1; j<=n; j++)
    {
      check_ordering(j, olist[i]);
    }
  }
}

check_comp(9, orderings);

LIB "tst.lib";tst_status(1);$
  
  

