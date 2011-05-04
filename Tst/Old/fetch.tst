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

list l = "dp";

proc extend_orderings(list olist, int j)
{
  int i;
  int c1;
  int c2;
  string o1;
  string o2;
  string o3;
  list nl;
  string weight_string = generate_weight_str(j);
  o1 = olist[1];
  if (size(olist) > 1)
  {
    o2 = olist[2];
    if (size(olist) > 2)
    {
      o3 = olist[3];
    }
    else
    {
      o3 = olist[2];
    }
  }
  else
  {
    o2 = o1;
    o3 = o1;
  }
      
  // add weight orderings 
  olist = olist + list("Wp" + weight_string, "wp" + weight_string);

    
  for (i=1; i<=size(olist); i++)
  {
    nl = nl + list(olist[i], "(C," + olist[i] + ")", "(c," + olist[i] + ")",
                   "(" + olist[i] + ",C)", "(" + olist[i] + ",c)");
  }

  // add product orderings
  if (j > 1)
  {
    c1 = j div 2;
    if (c1 + c1 == j)
    {
      c2 = c1;
    }
    else
    {
      c2 = c1 + 1;
    }
    nl = nl + list("(" + o1 + "(" + string(c1) + "),"+ o2 + "(" + string(c2) + "))", "(" + o1 + "(" + string(c1) + "),"+ o3 + "(" + string(c2) + "))");
    // and, extra weight vector
    nl = nl + list("(a" + weight_string + "," + o2 + "(" + string(c1) + "),"+ o3 + "(" + string(c2) + "))", "(a" + generate_weight_str(j-1)+ "," + o3 + "(" + string(c1) + "),"+ o1 + "(" + string(c2) + "))");
  }

  // and, last but not least, a Matrix ordering
  nl = nl + list("M(m)");
  
  return (nl);
}
    
proc check_fetch(int n, list olist)
{
  int i;
  int j;
  list e_olist;
  
  for (j=1; j<=n; j = j + (j div 10) + 1)
  {
    e_olist = extend_orderings(olist, j);
    execute(generate_ring_str(j, "r", e_olist[1]));
    execute(generate_polys_str(j));
    intmat m[j][j];
    m = m + 1;
    for (i=1; i<=size(e_olist); i++)
    {
      execute(generate_ring_str(j, "r1", e_olist[i]));
      fetch(r, p);
      fetch(r, p1);
      fetch(r, p2);
      fetch(r, p3);
      fetch(r, p4);
      r1;
      kill r1;
    }
    kill r;
    kill m;
  }
}

check_fetch(15, orderings);

LIB "tst.lib";tst_status(1);$
