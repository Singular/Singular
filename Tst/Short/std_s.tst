// std_s.tst -- long tests for std
// uses rcyclic examples from ISSAC'98 paper

LIB "tst.lib";
tst_init();

proc msetring(int charac, int nv, string ordering)
{
  execute("ring r = " + string(charac) + ",x(1.." + string(nv) + "), " +
  ordering + ";");
  keepring r;
}

proc ecyclic_i(int i, int comps)
{
  int j, k, l;
  poly p, q;

  for (j=1; j<=comps; j++)
  {
    q = 1;
    l = j;
    for (k=1; k<=i; k++)
    {
      q = q *var(l);
      l = l + comps;
    }
    p = p + q;
  }
  return (p);
}

proc rcyclic_i(int i, int vars, int comps)
{
  int j, k, l;
  poly p, q;

  for (j=1; j<=comps; j++)
  {
    q = 1;
    l = j;
    for (k=1; k<=i; k++)
    {
      q = q *var(l);
      if (l == vars)
      {
        l = 1;
      }
      else
      {
        l = l + 1;
      }
    }
    p = p + q;
  }
  return (p);
}

proc rcyclic_g(int vars, int comps)
{
  ideal id;
  int i;

  for (i=1; i<vars; i++)
  {
    id[i] = rcyclic_i(i, vars, comps);
  }

  return (id);
}

proc gcyclic(int vars, list #)
{
  if (vars < 1)
  {
    vars = 2;
  }
  int comps = vars;
  string kind = "n";

  if (size(#) > 0)
  {
    if (typeof(#[1]) == "string")
    {
      if (#[1] == "r" || #[1] == "h")
      {
        kind = #[1];
      }
      if (size(#) > 1)
      {
        if (typeof(#[2]) == "int" && #[2] > 1 && #[2] < vars)
        {
          comps = #[2];
        }
      }
    }
    else
    {
      if (typeof(#[1]) == "int")
      {
        if (#[1] > 1 && #[1] < vars)
        {
          comps = #[1];
        }
        if (size(#) > 1)
        {
          if (#[2] == "r" || #[2] == "h")
          {
            kind = #[2];
          }
        }
      }
    }
  }

  if (kind != "h")
  {
    msetring(global_char, vars, global_ordering);
  }
  else
  {
    msetring(global_char, vars+1, global_ordering);
  }

  keepring basering;

  ideal id = rcyclic_g(vars, comps);
  poly p = 1;
  int i;

  if (kind == "h" || kind == "n")
  {
    for (i=1; i<=vars; i++)
    {
      p = p * var(i);
    }
    id[vars] = p;
    if (kind == "h")
    {
      id[vars] = id[vars] - var(vars+1)^vars;
    }
    else
    {
      id[vars] = id[vars] -1;
    }
  }

  return (id);
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

  return (nl);

}

proc mystd
{
  "(" + charstr(basering) + "),(" + varstr(basering) + "),(" + ordstr(basering) + ");";
  print(#[1]);
  int t1 = timer;
  def id_res = std(#[1]);
   id_res;
//   t1 = timer - t1;
//   int i;
//   def id_poly = id_res[1];
//   for (i=1; i<=size(id_res); i++)
//   {
//     id_poly = id_poly + id_res[i];
//     if (size(id_res[i]) > 2)
//     {
//       lead(id_res[i]), lead(id_res[i] - lead(id_res[i])), size(id_res[i]);
//     }
//     else
//     {
//       id_res[i];
//     }
//   }
//   id_poly;
//  tst_ignore(t1, "time");
//  tst_ignore(memory(1), "memory");
}

proc gencopy
{
  def id = #[1];
  int n = #[2];
  int i, j;
  module m;


  for (i=1; i<=size(id); i++)
  {
    m[i] = id[i];
    for (j=1; j<=n; j++)
    {
      m[i] = m[i] + gen(j) *  id[i];
    }
  }
  return (m);
}


proc std_extended_range(int from, int to, int charac, list orderings)
{
  int k, j;
  list olist = orderings;
  global_char = charac;

  for (k=from; k<=to; k++)
  {
    olist = extend_orderings(orderings, k);
    for (j=1; j <= size(olist); j++)
    {
      global_ordering = olist[j];
      def id = gcyclic(k, 3, "r");
      mystd(id);
      mystd(gencopy(id, 2));
      kill basering;

      if (k>1)
      {
        def id = gcyclic(k-1, 3, "h");
        mystd(id);
        mystd(gencopy(id, 2));
        kill basering;
      }
    }
  }
}

proc std_range(int from, int to, int charac, list orderings)
{
  int k, j;
  list olist = orderings;
  global_char = charac;

  for (k=from; k<=to; k++)
  {
    for (j=1; j <= size(olist); j++)
    {
      global_ordering = olist[j];
      def id = gcyclic(k, 3, "r");
      mystd(id);
      kill basering;

      if (k>1)
      {
        def id = gcyclic(k-1, 3, "h");
        mystd(id);
        kill basering;
      }
    }
  }
}

int global_char = 32003;
string global_ordering = "dp";

option(prot);
option(noredefine);

list global_orderings = "dp", "lp";

std_extended_range(2, 5, 32003, global_orderings);

std_range(6,10, 32003, list("dp"));

std_range(2,5, 0, global_orderings);

std_range(6,10, 0,list("dp"));
tst_status(1);$
LIB "polylib.lib";
killall();
killall("proc");
exit;
