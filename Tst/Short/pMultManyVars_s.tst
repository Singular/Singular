LIB "tst.lib";
tst_init();

LIB "general.lib";

list tst_rgen_charstrs = list("3");
list tst_rgen_simple_orderings = list("dp");
intvec tst_rgen_products = 1;
intvec tst_rgen_extra_weights = intvec(0);
intvec tst_rgen_nvars = 20, 50, 100, 500, 1000, 2000, 3000, 5000, 10000;
list tst_rgen_comp_orderings = list("", "C");
tst_rgen_init();
proc pMultManyVars_Test(string rs)
{
  rs;
  rs = "ring r = " + rs + ";";
  execute(rs);
  int n_vars = tst_rgen_nvars[tst_rgen_var_index];
  ideal id = tst_cyclic(5);
  tst_TestMult(id, 2,size(tst_rgen_comp_orderings[tst_rgen_comp_index]));
  kill r;
}

string rs = tst_next_ring();
while (size(rs) > 1)
{
  pMultManyVars_Test(rs);
  rs = tst_next_ring();
}
 
tst_status(1);$
