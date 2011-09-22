LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring R=0,(x,y,z,t),dp;
ideal I=x^2,y^2,z^3;
list l=intclMonIdeal(I);
exportNuminvs();
// now the following variables are set:
nmz_hilbert_basis_elements;
nmz_number_extreme_rays;
nmz_rank;
nmz_index;
nmz_number_support_hyperplanes;
nmz_homogeneous;
nmz_primary;
nmz_ideal_multiplicity;
tst_status(1);$
