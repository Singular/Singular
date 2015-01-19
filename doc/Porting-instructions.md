What has been changed in Singular 4  {#changes_in_sw_page}
===================================

1. lots of stuff moved in the headers, which messes up the includes. This should be fixed already.
   If there is still a problem look in `test.cc` for the correct includes.
2. coefficients were separated from polynomial rings.
   * There is a new type `coeffs`.
   * Operations on `coeffs` are done by `n_*` functions, where `coeffs` is the last argument (consider it as the context storing information on the coefficient domain).
   * Functions like `nAdd(a, b)` become `n_Add(a, b, r)`.
3. new arithmetic functions.
   * Same as the previous.
   * Arithmetic is done with `p_*` functions, which take a ring as the last argument.
   * The use of the global variable `currRing` is discouraged.
4. please use ring methods (e.g. rVar(r)) instead of (currRing specific) global variables (e.g. `pVariables`)
