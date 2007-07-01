// File: finvar_s.tst
// Short tests for finvar.lib

LIB "tst.lib";
tst_init();
LIB "finvar.lib";

example cyclotomic;
example group_reynolds;
example molien;
example reynolds_molien;
example partial_molien;
example evaluate_reynolds;
example invariant_basis;
example invariant_basis_reynolds;
example primary_char0;
example primary_charp;
example primary_char0_no_molien;
example primary_charp_no_molien;
example primary_charp_without;
example primary_invariants;
example primary_char0_random;
example primary_charp_random;
example primary_char0_no_molien_random;
example primary_charp_no_molien_random;
// the following randomized example often fails:
example primary_charp_without_random;
example primary_invariants_random;
example power_products;
example secondary_char0;
example irred_secondary_char0;
example secondary_charp;
example secondary_no_molien;
example irred_secondary_no_molien;
example secondary_and_irreducibles_no_molien;
example secondary_not_cohen_macaulay;
example invariant_ring;
example invariant_ring_random;
example orbit_variety;
example relative_orbit_variety;
example rel_orbit_variety;
example image_of_variety;

tst_status(1);$
