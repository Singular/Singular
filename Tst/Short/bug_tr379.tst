LIB "tst.lib";
tst_init();

// used more arry entries(length(w))  than allocated(var(R))
LIB "decodegb.lib";

int q=16; // square root of the finite field size
int k,j,l;
//importfrom(Top, q);
int nr_transmission=100;
int max_weight; // maximum weight of functions used for code construction

ring r0=(2^8,a),(e2,e1,x),(wp(1+0,1,1));

ideal monomials = kbase(std(ideal(x^(q^2)-x, e1, e2)));
intvec sort_result = sortvec(monomials);
intvec monomial_weights = 0:size(monomials);
ideal sorted_monomials;
for (k=1; k <= size(monomials); k++) {
  sorted_monomials[k] = monomials[sort_result[k]];
  monomial_weights[k] = deg(sorted_monomials[k]);
 }

list all_field_elements = 0;
for (j=0; j<=q^2-2; j++) {
  all_field_elements = all_field_elements + list(a^j);
 };


matrix generator_matrix[size(sorted_monomials)][size(sorted_monomials)];
for (j=1; j<=size(sorted_monomials); j++) {
  for (k=1; k<=size(sorted_monomials); k++) {
    generator_matrix[j,k] = subst(sorted_monomials[j], x, all_field_elements[k]);
  }
 }
matrix inv_generator_matrix = inverse(generator_matrix);

max_weight=254;
ring r=(2^8,a),(e2,e1,x),(wp(1+max_weight,1,1));

ideal sorted_monomials = imap(r0, sorted_monomials);
int found_monomial=0;
for (j=1; j<=size(sorted_monomials); j++) {
  if (max_weight == deg(sorted_monomials[j])) {
    found_monomial=1;
  }
 }
if (!found_monomial) { max_weight--; continue; }

int minimum_distance = size(sorted_monomials)-max_weight+1;
int correctable_errors = (minimum_distance-1) div 2;
list all_curve_points = imap(r0, all_field_elements);
matrix generator_matrix = imap(r0, generator_matrix);
matrix inv_generator_matrix = imap(r0, inv_generator_matrix);
int error_modifier, counter;
error_modifier=0;
int nr_errors = correctable_errors + error_modifier;
counter=1;
// Randomly generate information.
//printf("counter=%s", counter);
matrix info[1][size(sorted_monomials)];
for (k=1; k<=size(sorted_monomials); k++) {
  if (deg(sorted_monomials[k]) <= max_weight /* *(counter mod 3) div 2 */ ) {
    info[1,k] = a^(random(0,q^2-2));
  } else {
    info[1,k] = 0;
  }
 }
matrix codeword = encode(info, generator_matrix);
matrix recvword = errorRand(codeword, nr_errors, 8);
matrix recv_info = recvword * inv_generator_matrix;
matrix zero_mask[1][size(sorted_monomials)];
for (j=1; j<= size(sorted_monomials); j++) {
  if ((recvword-codeword)[j]==0) {
    zero_mask[1,j] = 1;
  } else {
    zero_mask[1,j] = 0;
  }
 }
matrix zero_mask_info = zero_mask * inv_generator_matrix;
poly info_poly = (info * matrix(sorted_monomials, size(sorted_monomials),1))[1,1];
poly interpolation_polynomial = (recv_info * matrix(sorted_monomials, size(sorted_monomials),1))[1,1];
poly zero_mask_polynomial = (zero_mask_info * matrix(sorted_monomials, size(sorted_monomials),1))[1,1];
poly min_zero_mask_poly = groebner(ideal(zero_mask_polynomial,x^(q^2)-x))[1];

ideal gb = groebner(ideal(e1*(x^(q^2)-x), -e2+e1*interpolation_polynomial));
poly min_degree_poly_with_lpos_two;
for (j=1; j<=size(gb); j++) {
  if (deg(lead(gb[j]), intvec(1,0,0,0)) == 1) {
    min_degree_poly_with_lpos_two = gb[j];
    break;
  }
 }
poly target_poly = e1*info_poly*min_zero_mask_poly-e2*min_zero_mask_poly;

//ideal(min_degree_poly_with_lpos_two);
division(target_poly, ideal(min_degree_poly_with_lpos_two));

tst_status(1);$
