LIB "tst.lib";
tst_init();

LIB "finvar.lib";

example invariant_ring; newline;
example invariant_ring_random; newline;
example primary_invariants; newline;
example primary_invariants_random; newline;
example invariant_algebra_reynolds; newline;
example invariant_algebra_perm; newline;
example cyclotomic; newline;
example group_reynolds; newline;
example molien; newline;
example reynolds_molien; newline;
example partial_molien; newline;
example evaluate_reynolds; newline;
example invariant_basis; newline;
example invariant_basis_reynolds; newline;
example primary_char0; newline;
example primary_charp; newline;
example primary_char0_no_molien; newline;
example primary_charp_no_molien; newline;
example primary_charp_without; newline;
example primary_char0_random; newline;
example primary_charp_random; newline;
example primary_char0_no_molien_random; newline;
example primary_charp_no_molien_random; newline;
// the following randomized example is interactive
// example primary_charp_without_random; newline;
example power_products; newline;
example secondary_char0; newline;
example irred_secondary_char0; newline;
example secondary_charp; newline;
example secondary_no_molien; newline;
example irred_secondary_no_molien; newline;
example secondary_and_irreducibles_no_molien; newline;
example secondary_not_cohen_macaulay; newline;
example orbit_variety; newline;
example rel_orbit_variety; newline;
example relative_orbit_variety; newline;
example image_of_variety; newline;
example orbit_sums; newline;

tst_status(1);
$

This is the file finvarlb.tst
To run the tests and create the .res file 
go to the directory containing this file and type
  
Singular -teqr12345678 finvarlb.tst > finvarlb.res
