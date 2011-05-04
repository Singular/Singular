LIB "tst.lib";
tst_init();

proc generate_ring_str(int n, string r_name, string ostring)
{
  return ("ring "+r_name+" = 32003,x(1.." + string(n) + ")," + ostring + ";");
}

proc generate_polys_str(int n)
{
  string polystr = "poly p = ";
  int i;

  for (i=1; i<=n; i = i + (i div 10) + 1)
  {
    polystr = polystr + "x(" + string(i) + ") + ";
  }
  return (polystr + "1; poly p1 = p^2+p; vector p2 = (p^2+p)*(gen(2) + gen(3)); vector p3 = (p^2+p)*(gen(2) + gen(3))+ p^2 + p; vector p4 = p3 + p^3;p; p1; p2; p3; p4");
}

proc generate_weight_str(int j)
{
  int i;
  string res_str = "(";
  
  for (i=1; i<j; i++)
  {
    res_str = res_str + string(i) + ",";
  }
  return (res_str + string(j) + ")");
}


list orderings = "lp", "dp", "Dp", "ls", "ds", "Ds";

proc check_fetch(int n, list olist)
{
  int i;
  int j;
  
  for (j=1; j<=n; j = j + (j div 10) + 1)
  {
    execute(generate_ring_str(j, "r", olist[1]));
    execute(generate_polys_str(j));
    for (i=1; i<=size(olist); i++)
    {
      execute(generate_ring_str(j, "r1", olist[i]));
      fetch(r, p);
      fetch(r, p1);
      fetch(r, p2);
      fetch(r, p3);
      fetch(r, p4);
      r1;
      kill r1;
    }
    kill r;
  }
}

check_fetch(10, orderings);

tst_status(1);$
