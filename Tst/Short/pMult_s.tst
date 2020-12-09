LIB "tst.lib";
tst_init();

LIB "polylib.lib";

list tst_rgen_charstrs = list("3", "0");
list tst_rgen_simple_orderings = list("lp", "dp", "Dp", "ls", "ds", "Ds", "Wp","Ws","M");
intvec tst_rgen_products = 1..2;
intvec tst_rgen_extra_weights = intvec(0,1);
intvec tst_rgen_nvars = 1..4;

tst_rgen_init();
proc pAdd_Test(string rs)
{
  rs;
  rs = "ring r = " + rs + ";";
  execute(rs);
  int n_vars = tst_rgen_nvars[Tst::tst_rgen_var_index];
  ideal id = tst_cyclic(n_vars);
  if (n_vars == 1)
  {
    id = var(1), id;
  }
  tst_TestMult(id, n_vars, size(Tst::tst_rgen_comp_orderings[Tst::tst_rgen_comp_index]));
  kill r;
}


string rs = tst_next_ring();

while (size(rs) > 1)
{
  pAdd_Test(rs);
  rs = tst_next_ring();
}


tst_status(1);$

