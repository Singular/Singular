LIB "tst.lib";
tst_init();

LIB "polylib.lib";

list tst_rgen_simple_orderings = list("lp", "dp", "ls", "ds");
intvec tst_rgen_products = 1;
intvec tst_rgen_extra_weights = intvec(0);
intvec tst_rgen_nvars = 4..10;
list tst_rgen_comp_orderings = list("", "cc");
list tst_rgen_charstrs = list("0");
list tst_rgen_exp_bounds = list(63);

tst_rgen_init();

intvec how_often = 0,0,0,25,15,10,7,5,4,3;
proc pAdd_Test(string rs)
{
  Tst::tst_rgen_Lring;
  rs = "ring r = " + rs + ";";
  execute(rs);
  int n_vars = tst_rgen_nvars[Tst::tst_rgen_var_index];
//  ideal id = 1, tst_FullIdeal();
  ideal id = cyclic(n_vars);
  tst_TestMult(id, how_often[n_vars],
               size(tst_rgen_comp_orderings[Tst::tst_rgen_comp_index]));

  kill r;
}

string rs = tst_next_ring();

while (size(rs) > 1)
{
  pAdd_Test(rs);
  rs = tst_next_ring();
}

tst_status(1); $
