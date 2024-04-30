/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

// syntax table for Singular
//
// - search for an exact match of the argument types
// - otherwise search for the first possibility
//   with converted types of the arguments
// - otherwise report an error
//
// --------------------------------------------------
// depends on Singular/table.h and kernel/mod2.h

// operation: ++ (identifier)  ->  nothing
// operation: -- (identifier)  ->  nothing
// operation: - (int)  ->  int
// operation: - (bigint)  ->  bigint
// operation: - (number)  ->  number
// operation: - (poly)  ->  poly
// operation: - (vector)  ->  vector
// operation: - (matrix)  ->  matrix
// operation: - (intvec)  ->  intvec
// operation: - (intmat)  ->  intmat
// operation: - (bigintmat)  ->  bigintmat
// operation: ( (proc)  ->  any_type
// operation: attrib (def)  ->  nothing
// operation: bareiss (bigintmat)  ->  bigintmat
// operation: bareiss (module)  ->  list, only commutative rings, domain coeffs
// operation: betti (list)  ->  intmat, field coeffs
// operation: betti (resolution)  ->  intmat, field coeffs
// operation: betti (ideal)  ->  intmat, field coeffs
// operation: betti (module)  ->  intmat, field coeffs
// operation: bigint (bigint)  ->  bigint
// operation: bigint (number)  ->  bigint
// operation: bigint (poly)  ->  bigint
// operation: bigintmat (bigintmat)  ->  bigintmat
// operation: bigintvec (bigintvec)  ->  bigintvec
// operation: char (ring)  ->  int
// operation: char_series (ideal)  ->  matrix, only commutative rings, field coeffs
// operation: charstr (ring)  ->  string
// operation: close (link)  ->  nothing
// operation: coeffs (ring)  ->  cring
// operation: ncols (matrix)  ->  int
// operation: ncols (smatrix)  ->  int
// operation: ncols (ideal)  ->  int
// operation: ncols (module)  ->  int
// operation: ncols (intmat)  ->  int
// operation: ncols (bigintmat)  ->  int
// operation: cleardenom (poly)  ->  poly
// operation: cleardenom (vector)  ->  vector
// operation: size (bigint)  ->  int
// operation: size (number)  ->  int
// operation: size (resolution)  ->  int
// operation: size (string)  ->  int
// operation: size (poly)  ->  int
// operation: size (vector)  ->  int
// operation: size (ideal)  ->  int
// operation: size (module)  ->  int
// operation: size (matrix)  ->  int
// operation: size (intvec)  ->  int
// operation: size (intmat)  ->  int
// operation: size (bigintmat)  ->  int
// operation: size (bigintvec)  ->  int
// operation: size (list)  ->  int
// operation: size (ring)  ->  int
// operation: cring (ring)  ->  cring
// operation: def (int)  ->  def
// DUMMY // operation: def (any_type)  ->  $INVALID$
// operation: deg (poly)  ->  int
// operation: deg (vector)  ->  int
// operation: deg (matrix)  ->  int
// operation: degree (ideal)  ->  string, only commutative rings, domain coeffs
// operation: degree (module)  ->  string, only commutative rings, domain coeffs
// operation: defined (def)  ->  int
// operation: denominator (number)  ->  number
// operation: numerator (number)  ->  number
// operation: det (bigintmat)  ->  bigint
// operation: det (intmat)  ->  int
// operation: det (smatrix)  ->  poly, only commutative rings, field coeffs
// operation: det (matrix)  ->  poly, only commutative rings
// operation: dim (ideal)  ->  int
// operation: dim (module)  ->  int
// operation: dim (resolution)  ->  int
// operation: dump (link)  ->  nothing
// operation: gen (int)  ->  vector requires currRing
// operation: execute (string)  ->  nothing
// operation: ERROR (string)  ->  nothing
// operation: factorize (poly)  ->  list, only commutative rings, domain coeffs
// operation: finduni (ideal)  ->  ideal, only commutative rings, field coeffs
// operation: freemodule (int)  ->  module requires currRing
// operation: facstd (ideal)  ->  list, only commutative rings, field coeffs
// operation: getdump (link)  ->  nothing
// operation: highcorner (ideal)  ->  poly, only commutative rings
// operation: highcorner (module)  ->  vector, only commutative rings
// operation: hilb (ideal)  ->  nothing, domain coeffs
// operation: hilb (module)  ->  nothing, domain coeffs
// operation: hilb (intvec)  ->  intvec, domain coeffs
// operation: homog (ideal)  ->  int
// operation: homog (module)  ->  int
// operation: ideal (ideal)  ->  ideal
// operation: ideal (vector)  ->  ideal
// operation: ideal (matrix)  ->  ideal
// operation: ideal (ring)  ->  ideal requires currRing
// operation: ideal (map)  ->  ideal
// operation: impart (number)  ->  number
// operation: indepSet (ideal)  ->  intvec, only commutative rings, field coeffs
// operation: int (int)  ->  int
// operation: int (bigint)  ->  int
// operation: int (number)  ->  int
// operation: int (poly)  ->  int
// operation: int (string)  ->  int
// operation: interred (ideal)  ->  ideal
// operation: interred (module)  ->  module
// operation: intmat (bigintmat)  ->  intmat
// operation: intmat (intmat)  ->  intmat
// operation: intvec (intmat)  ->  intvec
// operation: intvec (intvec)  ->  intvec
// operation: intvec (bigintvec)  ->  intvec
// operation: rvar (poly)  ->  int
// operation: rvar (string)  ->  int
// operation: rvar (any_type)  ->  int
// operation: jacob (poly)  ->  ideal
// operation: jacob (ideal)  ->  matrix
// operation: jacob (module)  ->  module
// operation: janet (ideal)  ->  ideal, field coeffs
// operation: kbase (ideal)  ->  ideal, QQ coeffs
// operation: kbase (module)  ->  module, QQ coeffs
// operation: kernel (matrix)  ->  matrix
// operation: kernel (smatrix)  ->  smatrix
// operation: laguerre (poly)  ->  list, only commutative rings, field coeffs
// operation: ludecomp (matrix)  ->  list, only commutative rings, field coeffs
// operation: primefactors (bigint)  ->  list
// operation: primefactors (number)  ->  list
// operation: killattrib (identifier)  ->  nothing
// operation: lead (poly)  ->  poly
// operation: lead (ideal)  ->  ideal
// operation: lead (vector)  ->  vector
// operation: lead (module)  ->  module
// operation: leadcoef (poly)  ->  number
// operation: leadcoef (vector)  ->  number
// operation: leadexp (poly)  ->  intvec
// operation: leadexp (vector)  ->  intvec
// operation: leadmonom (poly)  ->  poly
// operation: leadmonom (vector)  ->  vector
// operation: link (link)  ->  link
// operation: list (def)  ->  list
// DUMMY // operation: map (any_type)  ->  $INVALID$
// operation: matrix (matrix)  ->  matrix
// operation: maxideal (int)  ->  ideal requires currRing
// operation: memory (int)  ->  bigint
// operation: minbase (ideal)  ->  ideal, only commutative rings, field coeffs
// operation: minbase (module)  ->  module, only commutative rings, field coeffs
// operation: minres (list)  ->  list, field coeffs
// operation: minres (resolution)  ->  resolution, field coeffs
// operation: module (module)  ->  module
// operation: monitor (link)  ->  nothing
// operation: monomial (intvec)  ->  poly requires currRing
// operation: mult (ideal)  ->  int, only commutative rings
// operation: mult (module)  ->  int, only commutative rings
// operation: mstd (ideal)  ->  list, only commutative rings
// operation: mstd (module)  ->  list, only commutative rings
// operation: nameof (any_type)  ->  string
// operation: names (int)  ->  list
// operation: names (package)  ->  list
// operation: names (ring)  ->  list
// operation: not (int)  ->  int
// operation: number (number)  ->  number
// operation: number (poly)  ->  number
// operation: number (bigint)  ->  number requires currRing
// operation: $INVALID$ ($INVALID$)  ->  $INVALID$
// operation: npars (ring)  ->  int
// operation: nvars (ring)  ->  int
// operation: open (link)  ->  nothing
// operation: option (def)  ->  nothing
// operation: ord (poly)  ->  int
// operation: ord (vector)  ->  int
// operation: ordstr (ring)  ->  string
// operation: par (int)  ->  number requires currRing
// operation: pardeg (number)  ->  int
// operation: parstr (int)  ->  string
// operation: parstr (ring)  ->  string
// operation: poly (poly)  ->  poly
// operation: poly (bigint)  ->  poly requires currRing
// operation: preimage (map)  ->  ring, letterplace rings
// operation: prime (int)  ->  int
// operation: print (list)  ->  string
// operation: print (def)  ->  string
// operation: proc (proc)  ->  proc
// operation: prune (module)  ->  module, field coeffs
// operation: qhweight (ideal)  ->  intvec
// operation: qhweight (module)  ->  intvec
// operation: rank (matrix)  ->  int, field coeffs
// operation: read (link)  ->  string
// operation: regularity (list)  ->  int, only commutative rings
// operation: repart (number)  ->  number
// operation: reservedName (string)  ->  int
// operation: resolution (list)  ->  resolution requires currRing
// operation: resolution (resolution)  ->  resolution
// operation: restart (int)  ->  nothing
// operation: rightstd (ideal)  ->  ideal, field coeffs
// operation: ringlist (ring)  ->  list
// operation: ring_list (cring)  ->  list
// operation: ring_list (ring)  ->  list
// operation: ring (ring)  ->  ring
// operation: ring (list)  ->  ring
// operation: nrows (vector)  ->  int
// operation: nrows (module)  ->  int
// operation: nrows (matrix)  ->  int
// operation: nrows (smatrix)  ->  int
// operation: nrows (intmat)  ->  int
// operation: nrows (bigintmat)  ->  int
// operation: nrows (intvec)  ->  int
// operation: sba (ideal)  ->  ideal
// operation: sba (module)  ->  module
// operation: setring (ring)  ->  nothing
// operation: slimgb (ideal)  ->  ideal, field coeffs
// operation: slimgb (module)  ->  module, field coeffs
// operation: smatrix (smatrix)  ->  smatrix
// operation: sortvec (ideal)  ->  intvec
// operation: sortvec (module)  ->  intvec
// operation: sqrfree (poly)  ->  list, only commutative rings, field coeffs
// operation: std (ideal)  ->  ideal
// operation: std (module)  ->  module
// operation: std (smatrix)  ->  smatrix
// operation: string (string)  ->  string
// operation: syz (ideal)  ->  module
// operation: syz (module)  ->  module
// operation: envelope (ring)  ->  ring, field coeffs
// operation: opposite (ring)  ->  ring, field coeffs
// operation: twostd (ideal)  ->  ideal
// operation: twostd (module)  ->  module, letterplace rings
// operation: trace (intmat)  ->  int
// operation: trace (matrix)  ->  poly
// operation: transpose (intvec)  ->  intmat
// operation: transpose (intmat)  ->  intmat
// operation: transpose (bigintmat)  ->  bigintmat
// operation: transpose (matrix)  ->  matrix
// operation: transpose (module)  ->  module
// operation: transpose (smatrix)  ->  smatrix
// operation: typeof (any_type)  ->  string
// operation: univariate (poly)  ->  int
// operation: variables (poly)  ->  ideal
// operation: variables (ideal)  ->  ideal
// operation: variables (matrix)  ->  ideal
// operation: vector (vector)  ->  vector
// operation: vdim (ideal)  ->  int, QQ coeffs
// operation: vdim (module)  ->  int, QQ coeffs
// operation: var (int)  ->  poly requires currRing
// operation: varstr (int)  ->  string
// operation: varstr (ring)  ->  string
// operation: weight (ideal)  ->  intvec
// operation: weight (module)  ->  intvec
// operation: load (string)  ->  nothing
// operation: convhull (ideal)  ->  ideal
// operation: waitfirst (list)  ->  int
// operation: waitall (list)  ->  int
/*---------------------------------------------*/
// operation: :: (def, def)  ->  any_type
// operation: + (int, int)  ->  int
// operation: + (bigint, bigint)  ->  bigint
// operation: + (number, number)  ->  number
// operation: + (poly, poly)  ->  polyBucket
// operation: + (vector, vector)  ->  vector
// operation: + (ideal, ideal)  ->  ideal
// operation: + (module, module)  ->  module
// operation: + (poly, matrix)  ->  matrix
// operation: + (matrix, poly)  ->  matrix
// operation: + (matrix, matrix)  ->  matrix
// operation: + (smatrix, smatrix)  ->  smatrix
// operation: + (string, string)  ->  string
// operation: + (bigintmat, bigintmat)  ->  bigintmat
// operation: + (bigintvec, bigintvec)  ->  bigintvec
// operation: + (bigintmat, int)  ->  bigintmat
// operation: + (int, bigintmat)  ->  bigintmat
// operation: + (bigintmat, bigint)  ->  bigintmat
// operation: + (bigint, bigintmat)  ->  bigintmat
// operation: + (bigintvec, int)  ->  bigintvec
// operation: + (int, bigintvec)  ->  bigintvec
// operation: + (bigintvec, bigint)  ->  bigintvec
// operation: + (bigint, bigintvec)  ->  bigintvec
// operation: + (intvec, int)  ->  intvec
// operation: + (int, intvec)  ->  intvec
// operation: + (intmat, int)  ->  intmat
// operation: + (int, intmat)  ->  intmat
// operation: + (intvec, intvec)  ->  intvec
// operation: + (intmat, intmat)  ->  intmat
// operation: + (list, list)  ->  list
// operation: + (ring, ring)  ->  ring
// operation: + (ring, string)  ->  ring
// operation: + (string, ring)  ->  ring
// operation: + (polyBucket, poly)  ->  polyBucket
// operation: - (int, int)  ->  int
// operation: - (bigint, bigint)  ->  bigint
// operation: - (number, number)  ->  number
// operation: - (poly, poly)  ->  polyBucket
// operation: - (vector, vector)  ->  vector
// operation: - (vector, vector)  ->  vector
// operation: - (matrix, poly)  ->  matrix
// operation: - (matrix, matrix)  ->  matrix
// operation: - (smatrix, smatrix)  ->  smatrix
// operation: - (bigintmat, bigintmat)  ->  bigintmat
// operation: - (bigintmat, int)  ->  bigintmat
// operation: - (int, bigintmat)  ->  bigintmat
// operation: - (bigintmat, bigint)  ->  bigintmat
// operation: - (bigint, bigintmat)  ->  bigintmat
// operation: - (bigintvec, bigintvec)  ->  bigintvec
// operation: - (bigintvec, int)  ->  bigintvec
// operation: - (int, bigintvec)  ->  bigintvec
// operation: - (bigintvec, bigint)  ->  bigintvec
// operation: - (bigint, bigintvec)  ->  bigintvec
// operation: - (intvec, int)  ->  intvec
// operation: - (intmat, int)  ->  intmat
// operation: - (intvec, intvec)  ->  intvec
// operation: - (intmat, intmat)  ->  intmat
// operation: - (ring, string)  ->  ring
// operation: - (polyBucket, poly)  ->  polyBucket
// DUMMY // operation: - (ideal, ideal)  ->  nothing
// DUMMY // operation: - (module, module)  ->  nothing
// operation: * (int, int)  ->  int
// operation: * (bigint, bigint)  ->  bigint
// operation: * (number, number)  ->  number
// operation: * (poly, poly)  ->  poly
// operation: * (poly, vector)  ->  vector
// operation: * (vector, poly)  ->  vector
// operation: * (ideal, poly)  ->  ideal
// operation: * (poly, ideal)  ->  ideal
// operation: * (module, poly)  ->  module
// operation: * (poly, module)  ->  module
// operation: * (ideal, ideal)  ->  ideal
// operation: * (ideal, vector)  ->  module
// operation: * (vector, ideal)  ->  module
// operation: * (ideal, module)  ->  module
// operation: * (module, ideal)  ->  module
// operation: * (matrix, poly)  ->  matrix
// operation: * (poly, matrix)  ->  matrix
// operation: * (matrix, number)  ->  matrix
// operation: * (number, matrix)  ->  matrix
// operation: * (matrix, int)  ->  matrix
// operation: * (int, matrix)  ->  matrix
// operation: * (matrix, matrix)  ->  matrix
// operation: * (matrix, bigint)  ->  matrix
// operation: * (bigint, matrix)  ->  matrix
// operation: * (smatrix, smatrix)  ->  smatrix
// operation: * (bigintmat, bigintmat)  ->  bigintmat
// operation: * (bigintmat, int)  ->  bigintmat
// operation: * (int, bigintmat)  ->  bigintmat
// operation: * (bigintmat, bigint)  ->  bigintmat
// operation: * (bigint, bigintmat)  ->  bigintmat
// operation: * (bigintvec, int)  ->  bigintvec
// operation: * (int, bigintvec)  ->  bigintvec
// operation: * (bigintvec, bigint)  ->  bigintvec
// operation: * (bigint, bigintvec)  ->  bigintvec
// operation: * (intvec, int)  ->  intvec
// operation: * (int, intvec)  ->  intvec
// operation: * (intmat, int)  ->  intmat
// operation: * (int, intmat)  ->  intmat
// operation: * (intmat, intvec)  ->  intvec
// operation: * (intmat, intmat)  ->  intmat
// operation: * (intvec, intmat)  ->  intmat
// DUMMY // operation: * (module, module)  ->  $INVALID$
// operation: / (number, number)  ->  number
// operation: / (poly, poly)  ->  poly
// operation: / (vector, poly)  ->  vector
// operation: / (matrix, poly)  ->  matrix, field coeffs
// operation: / (int, int)  ->  int
// operation: / (bigint, bigint)  ->  bigint
// operation: / (intvec, int)  ->  intvec
// operation: / (intmat, int)  ->  intmat
// operation: / (cring, int)  ->  cring
// operation: / (cring, bigint)  ->  cring
// operation: div (int, int)  ->  int
// operation: div (bigint, bigint)  ->  bigint
// operation: div (intvec, int)  ->  intvec
// operation: div (intmat, int)  ->  intmat
// operation: div (poly, poly)  ->  poly
// operation: div (vector, poly)  ->  vector
// operation: % (int, int)  ->  int
// operation: % (bigint, bigint)  ->  bigint
// operation: % (intvec, int)  ->  intvec
// operation: % (intmat, int)  ->  intmat
// operation: % (number, number)  ->  number
// operation: % (poly, poly)  ->  poly, only commutative rings, field coeffs
// operation: ^ (int, int)  ->  int
// operation: ^ (bigint, int)  ->  bigint
// operation: ^ (number, int)  ->  number
// operation: ^ (poly, int)  ->  poly
// operation: ^ (ideal, int)  ->  ideal
// operation: <= (int, int)  ->  int
// operation: <= (bigint, bigint)  ->  int
// operation: <= (number, number)  ->  int
// operation: <= (string, string)  ->  int
// operation: <= (intvec, int)  ->  int
// operation: <= (intvec, intvec)  ->  int
// operation: <= (poly, poly)  ->  int
// operation: <= (vector, vector)  ->  int
// operation: <= (matrix, matrix)  ->  int
// operation: < (int, int)  ->  int
// operation: < (bigint, bigint)  ->  int
// operation: < (number, number)  ->  int
// operation: < (intvec, int)  ->  int
// operation: < (intvec, intvec)  ->  int
// operation: < (string, string)  ->  int
// operation: < (poly, poly)  ->  int
// operation: < (vector, vector)  ->  int
// operation: < (matrix, matrix)  ->  int
// operation: >= (int, int)  ->  int
// operation: >= (bigint, bigint)  ->  int
// operation: >= (number, number)  ->  int
// operation: >= (string, string)  ->  int
// operation: >= (intvec, int)  ->  int
// operation: >= (intvec, intvec)  ->  int
// operation: >= (poly, poly)  ->  int
// operation: >= (vector, vector)  ->  int
// operation: >= (matrix, matrix)  ->  int
// operation: > (int, int)  ->  int
// operation: > (bigint, bigint)  ->  int
// operation: > (number, number)  ->  int
// operation: > (string, string)  ->  int
// operation: > (intvec, int)  ->  int
// operation: > (intvec, intvec)  ->  int
// operation: > (poly, poly)  ->  int
// operation: > (vector, vector)  ->  int
// operation: > (matrix, matrix)  ->  int
// operation: and (int, int)  ->  int
// operation: or (int, int)  ->  int
// operation: == (int, int)  ->  int
// operation: == (ring, int)  ->  int
// operation: == (ring, ring)  ->  int
// operation: == (bigint, bigint)  ->  int
// operation: == (number, number)  ->  int
// operation: == (string, string)  ->  int
// operation: == (poly, poly)  ->  int
// operation: == (vector, vector)  ->  int
// operation: == (intvec, int)  ->  int
// operation: == (intvec, intvec)  ->  int
// operation: == (intmat, intmat)  ->  int
// operation: == (bigintmat, bigintmat)  ->  int
// operation: == (bigintvec, bigintvec)  ->  int
// operation: == (cring, cring)  ->  int
// operation: == (matrix, matrix)  ->  int
// operation: == (smatrix, smatrix)  ->  int
// operation: == (ring, ring)  ->  int
// DUMMY // operation: == (ideal, ideal)  ->  $INVALID$
// DUMMY // operation: == (module, module)  ->  $INVALID$
// DUMMY // operation: == (ideal, module)  ->  $INVALID$
// DUMMY // operation: == (module, ideal)  ->  $INVALID$
// operation: <> (int, int)  ->  int
// operation: <> (bigint, bigint)  ->  int
// operation: <> (number, number)  ->  int
// operation: <> (string, string)  ->  int
// operation: <> (poly, poly)  ->  int
// operation: <> (vector, vector)  ->  int
// operation: <> (intvec, intvec)  ->  int
// operation: <> (intmat, intmat)  ->  int
// operation: <> (matrix, matrix)  ->  int
// operation: <> (smatrix, smatrix)  ->  int
// operation: <> (bigintmat, bigintmat)  ->  int
// operation: <> (bigintvec, bigintvec)  ->  int
// operation: <> (cring, cring)  ->  int
// DUMMY // operation: <> (ideal, ideal)  ->  $INVALID$
// DUMMY // operation: <> (module, module)  ->  $INVALID$
// DUMMY // operation: <> (ideal, module)  ->  $INVALID$
// DUMMY // operation: <> (module, ideal)  ->  $INVALID$
// operation: <> (ring, ring)  ->  int
// operation: .. (int, int)  ->  intvec
// operation: [ (intvec, int)  ->  int
// operation: [ (intvec, intvec)  ->  int
// operation: [ (bigintvec, int)  ->  bigint
// operation: [ (bigintvec, intvec)  ->  bigint
// operation: [ (ideal, int)  ->  poly
// operation: [ (map, int)  ->  poly
// operation: [ (ideal, intvec)  ->  poly
// operation: [ (module, int)  ->  vector
// operation: [ (module, intvec)  ->  vector
// operation: [ (string, int)  ->  string
// operation: [ (string, intvec)  ->  string
// operation: [ (list, int)  ->  any_type
// operation: [ (list, intvec)  ->  any_type
// operation: [ (poly, int)  ->  poly
// operation: [ (polyBucket, int)  ->  poly
// operation: [ (poly, intvec)  ->  poly
// operation: [ (vector, int)  ->  poly
// operation: [ (vector, intvec)  ->  vector
// operation: [ (cring, any_type)  ->  ring
// operation: ( (proc, def)  ->  any_type
// operation: ( (map, def)  ->  any_type
// operation: ( (LIB, string)  ->  nothing
// operation: ( (any_type, int)  ->  any_type
// operation: ( (any_type, intvec)  ->  any_type
// operation: : (int, int)  ->  intvec
// operation: align (vector, int)  ->  vector
// operation: align (module, int)  ->  module
// operation: attrib (def, string)  ->  nothing
// operation: betti (list, int)  ->  intmat
// operation: betti (resolution, int)  ->  intmat
// operation: betti (ideal, int)  ->  intmat
// operation: betti (module, int)  ->  intmat
// operation: bracket (poly, poly)  ->  poly
// operation: chinrem (intvec, intvec)  ->  bigint
// operation: chinrem (list, intvec)  ->  any_type
// operation: chinrem (list, list)  ->  any_type
// operation: coef (poly, poly)  ->  matrix
// operation: coef (ideal, poly)  ->  matrix
// operation: coeffs (ideal, poly)  ->  matrix
// operation: coeffs (module, poly)  ->  matrix
// operation: coeffs (ideal, ideal)  ->  matrix
// operation: coeffs (module, module)  ->  matrix
// operation: contract (ideal, ideal)  ->  matrix
// operation: deg (poly, intvec)  ->  int
// operation: deg (vector, intvec)  ->  int
// operation: deg (matrix, intvec)  ->  int
// operation: delete (intvec, int)  ->  intvec
// operation: delete (ideal, int)  ->  ideal
// operation: delete (ideal, intvec)  ->  ideal
// operation: delete (module, int)  ->  module
// operation: delete (module, intvec)  ->  module
// operation: delete (list, int)  ->  list
// operation: delete (list, intvec)  ->  list
// operation: det (smatrix, string)  ->  poly, only commutative rings, field coeffs
// operation: det (matrix, string)  ->  poly, only commutative rings
// operation: diff (poly, poly)  ->  poly
// operation: diff (vector, poly)  ->  vector
// operation: diff (ideal, poly)  ->  ideal
// operation: diff (ideal, ideal)  ->  matrix
// operation: diff (module, poly)  ->  module
// operation: diff (matrix, poly)  ->  matrix
// operation: diff (number, number)  ->  number
// operation: dim (ideal, ideal)  ->  int
// operation: dim (module, ideal)  ->  int
// operation: division (ideal, ideal)  ->  list
// operation: division (module, module)  ->  list
// operation: eliminate (ideal, poly)  ->  ideal
// operation: eliminate (module, poly)  ->  module
// operation: eliminate (ideal, intvec)  ->  ideal
// operation: eliminate (module, intvec)  ->  module
// operation: exportto (package, identifier)  ->  nothing
// operation: extgcd (int, int)  ->  list
// operation: extgcd (bigint, bigint)  ->  list
// operation: extgcd (poly, poly)  ->  list, only commutative rings, field coeffs
// operation: factorize (poly, int)  ->  ideal, only commutative rings, field coeffs
// operation: facstd (ideal, ideal)  ->  list, only commutative rings, field coeffs
// operation: farey (bigint, bigint)  ->  number requires currRing, field coeffs
// operation: farey (ideal, bigint)  ->  ideal, field coeffs
// operation: farey (module, bigint)  ->  module, field coeffs
// operation: farey (matrix, bigint)  ->  matrix, field coeffs
// operation: farey (smatrix, bigint)  ->  smatrix, field coeffs
// operation: farey (list, bigint)  ->  list, field coeffs
// operation: fetch (ring, any_type)  ->  any_type
// operation: fglm (ring, def)  ->  ideal requires currRing, only commutative rings, field coeffs
// operation: fglmquot (ideal, poly)  ->  ideal, only commutative rings, field coeffs
// operation: find (string, string)  ->  int
// operation: fres (ideal, int)  ->  resolution, only commutative rings, field coeffs
// operation: fres (module, int)  ->  resolution, only commutative rings, field coeffs
// operation: frwalk (ring, def)  ->  ideal requires currRing, only commutative rings, field coeffs
// operation: gcd (int, int)  ->  int
// operation: gcd (number, number)  ->  number
// operation: gcd (bigint, bigint)  ->  bigint
// operation: gcd (poly, poly)  ->  poly, only commutative rings
// operation: hilb (ideal, int)  ->  bigintvec, domain coeffs
// operation: hilb (module, int)  ->  bigintvec, domain coeffs
// operation: homog (ideal, intvec)  ->  int
// operation: homog (module, intvec)  ->  int
// operation: homog (poly, poly)  ->  poly
// operation: homog (vector, poly)  ->  vector
// operation: homog (ideal, poly)  ->  ideal
// operation: homog (module, poly)  ->  module
// operation: hres (ideal, int)  ->  resolution, only commutative rings, field coeffs
// operation: imap (ring, any_type)  ->  any_type
// operation: importfrom (package, any_type)  ->  nothing
// operation: indepSet (ideal, int)  ->  list, only commutative rings, field coeffs
// operation: insert (list, def)  ->  list
// operation: interpolation (list, intvec)  ->  ideal requires currRing, only commutative rings, field coeffs
// operation: intersect (ideal, ideal)  ->  ideal
// operation: intersect (module, module)  ->  module
// operation: janet (ideal, int)  ->  ideal, field coeffs
// operation: jet (poly, int)  ->  poly
// operation: jet (ideal, int)  ->  ideal
// operation: jet (vector, int)  ->  vector
// operation: jet (module, int)  ->  module
// operation: jet (matrix, int)  ->  matrix
// operation: kbase (ideal, int)  ->  ideal, QQ coeffs
// operation: kbase (module, int)  ->  module, QQ coeffs
// operation: kernel (ring, any_type)  ->  ideal requires currRing
// operation: killattrib (identifier, string)  ->  nothing
// operation: koszul (int, int)  ->  matrix requires currRing, only commutative rings
// operation: koszul (int, ideal)  ->  matrix, only commutative rings
// operation: kres (ideal, int)  ->  resolution, only commutative rings, field coeffs
// operation: lift (ideal, ideal)  ->  matrix
// operation: lift (module, module)  ->  matrix
// operation: liftstd (ideal, matrix)  ->  ideal
// operation: liftstd (module, matrix)  ->  module
// operation: load (string, string)  ->  nothing
// operation: lres (ideal, int)  ->  resolution, only commutative rings, field coeffs
// operation: modulo (ideal, ideal)  ->  module
// operation: modulo (module, module)  ->  module
// operation: monitor (link, string)  ->  nothing
// operation: mpresmat (ideal, int)  ->  module, only commutative rings, field coeffs
// operation: newstruct (string, string)  ->  nothing
// operation: mres (ideal, int)  ->  resolution, field coeffs
// operation: mres (module, int)  ->  resolution, field coeffs
// operation: primefactors (bigint, int)  ->  list
// operation: primefactors (number, int)  ->  list
// operation: ncalgebra (poly, poly)  ->  nothing, only commutative rings, field coeffs
// operation: ncalgebra (poly, matrix)  ->  nothing, only commutative rings, field coeffs
// operation: ncalgebra (matrix, poly)  ->  nothing, only commutative rings, field coeffs
// operation: ncalgebra (matrix, matrix)  ->  nothing, only commutative rings, field coeffs
// operation: nc_algebra (poly, poly)  ->  ring, only commutative rings, field coeffs
// operation: nc_algebra (poly, matrix)  ->  ring, only commutative rings, field coeffs
// operation: nc_algebra (matrix, poly)  ->  ring, only commutative rings, field coeffs
// operation: nc_algebra (matrix, matrix)  ->  ring, only commutative rings, field coeffs
// operation: oppose (ring, def)  ->  any_type, field coeffs
// operation: parstr (ring, int)  ->  string
// operation: print (def, string)  ->  string
// operation: prune_map (module, identifier)  ->  module
// operation: quotient (ideal, ideal)  ->  ideal
// operation: quotient (module, ideal)  ->  module
// operation: quotient (module, module)  ->  ideal
// operation: random (int, int)  ->  int
// operation: rank (matrix, int)  ->  int, field coeffs
// operation: read (link, string)  ->  string
// operation: reduce (poly, ideal)  ->  poly
// operation: reduce (vector, ideal)  ->  vector
// operation: reduce (vector, module)  ->  vector
// operation: reduce (ideal, ideal)  ->  ideal
// operation: reduce (module, ideal)  ->  module
// operation: reduce (module, module)  ->  module
// operation: nres (ideal, int)  ->  resolution, field coeffs
// operation: nres (module, int)  ->  resolution, field coeffs
// operation: sba (ideal, int)  ->  ideal
// operation: sba (module, int)  ->  module
// operation: simplify (poly, int)  ->  poly
// operation: simplify (vector, int)  ->  vector
// operation: simplify (ideal, int)  ->  ideal
// operation: simplify (module, int)  ->  module
// operation: sqrfree (poly, int)  ->  ideal, only commutative rings
// operation: status (link, string)  ->  string
// operation: status (list, int)  ->  int
// operation: sres (ideal, int)  ->  resolution, only commutative rings
// operation: sres (module, int)  ->  resolution, only commutative rings
// operation: std (ideal, poly)  ->  ideal
// operation: std (module, vector)  ->  module
// operation: std (ideal, ideal)  ->  ideal
// operation: std (module, module)  ->  module
// operation: std (ideal, bigintvec)  ->  ideal, field coeffs
// operation: std (module, bigintvec)  ->  module, field coeffs
// operation: syz (ideal, string)  ->  module
// operation: syz (module, string)  ->  module
// operation: tensor (matrix, matrix)  ->  matrix
// operation: tensor (module, module)  ->  module
// operation: tensor (smatrix, smatrix)  ->  smatrix
// operation: varstr (ring, int)  ->  string
// operation: waitfirst (list, int)  ->  int
// operation: waitall (list, int)  ->  int
// operation: wedge (matrix, int)  ->  matrix, only commutative rings
/*---------------------------------------------*/
// operation: [ (string, int, int)  ->  string
// operation: [ (intmat, int, int)  ->  int
// operation: [ (bigintmat, int, int)  ->  bigint
// operation: [ (intmat, int, intvec)  ->  int
// operation: [ (bigintmat, int, intvec)  ->  bigint
// operation: [ (intmat, intvec, int)  ->  int
// operation: [ (bigintmat, intvec, int)  ->  bigint
// operation: [ (intmat, intvec, intvec)  ->  int
// operation: [ (bigintmat, intvec, intvec)  ->  bigint
// operation: [ (matrix, int, int)  ->  poly
// operation: [ (smatrix, int, int)  ->  poly
// operation: [ (matrix, int, intvec)  ->  poly
// operation: [ (matrix, intvec, int)  ->  poly
// operation: [ (matrix, intvec, intvec)  ->  poly
// operation: [ (cring, any_type, any_type)  ->  ring
// operation: ( (proc, def, def)  ->  any_type
// operation: attrib (identifier, string, def)  ->  nothing
// operation: bareiss (module, int, int)  ->  list, only commutative rings, domain coeffs
// operation: bracket (poly, poly, int)  ->  poly
// operation: coeffs (poly, poly, matrix)  ->  matrix
// operation: coeffs (vector, poly, matrix)  ->  matrix
// operation: coeffs (ideal, poly, matrix)  ->  matrix
// operation: coeffs (module, poly, matrix)  ->  matrix
// operation: coeffs (ideal, ideal, poly)  ->  matrix
// operation: coeffs (module, module, poly)  ->  matrix
// operation: eliminate (ideal, poly, bigintvec)  ->  ideal, only commutative rings, field coeffs
// operation: eliminate (module, poly, bigintvec)  ->  module, only commutative rings, field coeffs
// operation: eliminate (ideal, poly, string)  ->  ideal
// operation: eliminate (module, poly, string)  ->  module
// operation: find (string, string, int)  ->  int
// operation: fres (ideal, int, string)  ->  resolution, only commutative rings, field coeffs
// operation: fres (module, int, string)  ->  resolution, only commutative rings, field coeffs
// operation: frwalk (ring, def, int)  ->  ideal requires currRing, only commutative rings
// operation: hilb (ideal, int, intvec)  ->  intvec, domain coeffs
// operation: hilb (module, int, intvec)  ->  intvec, domain coeffs
// operation: hilb (ideal, ring, string)  ->  nothing, domain coeffs
// operation: hilb (module, ring, string)  ->  nothing, domain coeffs
// operation: homog (poly, poly, intvec)  ->  poly
// operation: homog (vector, poly, intvec)  ->  vector
// operation: homog (ideal, poly, intvec)  ->  ideal
// operation: homog (module, poly, intvec)  ->  module
// operation: homog (module, intvec, intvec)  ->  int
// operation: insert (list, def, int)  ->  list
// operation: intersect (ideal, ideal, string)  ->  ideal
// operation: intersect (module, module, string)  ->  module
// operation: intersect (ideal, ideal, ideal)  ->  ideal
// operation: intersect (module, module, module)  ->  module
// operation: intmat (intmat, int, int)  ->  intmat
// operation: jet (poly, int, intvec)  ->  poly, field coeffs
// operation: jet (ideal, int, intvec)  ->  ideal, field coeffs
// operation: jet (vector, int, intvec)  ->  vector, field coeffs
// operation: jet (module, int, intvec)  ->  module, field coeffs
// operation: jet (poly, poly, int)  ->  poly, field coeffs
// operation: jet (vector, poly, int)  ->  vector, field coeffs
// operation: jet (ideal, matrix, int)  ->  ideal, field coeffs
// operation: jet (module, matrix, int)  ->  module, field coeffs
// DUMMY // operation: jet (poly, int, int)  ->  poly, field coeffs
// operation: koszul (int, int, ideal)  ->  matrix, only commutative rings, field coeffs
// operation: lift (ideal, ideal, matrix)  ->  matrix
// operation: lift (module, module, matrix)  ->  matrix
// operation: liftstd (ideal, matrix, module)  ->  ideal
// operation: liftstd (module, matrix, module)  ->  module
// operation: liftstd (ideal, matrix, string)  ->  ideal
// operation: liftstd (module, matrix, string)  ->  module
// operation: matrix (ideal, int, int)  ->  matrix
// operation: matrix (module, int, int)  ->  matrix
// operation: matrix (matrix, int, int)  ->  matrix
// operation: modulo (ideal, ideal, matrix)  ->  module
// operation: modulo (ideal, ideal, string)  ->  module
// operation: modulo (module, module, matrix)  ->  module
// operation: modulo (module, module, string)  ->  module
// operation: mres_map (ideal, int, smatrix)  ->  resolution, field coeffs
// operation: mres_map (module, int, smatrix)  ->  resolution, field coeffs
// operation: smatrix (module, int, int)  ->  smatrix
// operation: smatrix (smatrix, int, int)  ->  smatrix
// operation: newstruct (string, string, string)  ->  nothing
// operation: preimage (ring, any_type, any_type)  ->  ideal requires currRing
// operation: random (int, int, int)  ->  intmat
// operation: reduce (poly, ideal, int)  ->  poly
// operation: reduce (vector, ideal, int)  ->  vector
// operation: reduce (vector, module, int)  ->  vector
// operation: reduce (ideal, ideal, int)  ->  ideal
// operation: reduce (module, module, int)  ->  module
// operation: reduce (module, ideal, int)  ->  module
// operation: reduce (poly, poly, ideal)  ->  poly
// operation: reduce (vector, poly, module)  ->  vector
// operation: reduce (ideal, ideal, matrix)  ->  ideal
// operation: reduce (module, module, matrix)  ->  module
// operation: resultant (poly, poly, poly)  ->  poly, only commutative rings
// operation: ring (def, def, def)  ->  ring
// operation: sba (ideal, int, int)  ->  ideal
// operation: sba (module, int, int)  ->  module
// operation: status (link, string, string)  ->  int
// operation: std (ideal, bigintvec, intvec)  ->  ideal, field coeffs
// operation: std (module, bigintvec, intvec)  ->  module, field coeffs
// operation: subst (def, poly, polyBucket)  ->  any_type
// operation: subst (poly, poly, poly)  ->  poly
// operation: subst (vector, poly, poly)  ->  vector
// operation: subst (ideal, poly, poly)  ->  ideal
// operation: subst (module, poly, poly)  ->  module
// operation: subst (matrix, poly, poly)  ->  matrix
// operation: subst (matrix, poly, int)  ->  matrix
// operation: subst (matrix, poly, number)  ->  matrix
// operation: laguerre (poly, int, int)  ->  list, only commutative rings, field coeffs
// operation: vandermonde (ideal, ideal, int)  ->  poly, only commutative rings, field coeffs
/*---------------------------------------------*/
// operation: ( (...)  ->  any_type ( number of arguments >0 )
// operation: [ (...)  ->  ring ( number of arguments >0 )
// operation: bigintvec (...)  ->  bigintvec ( 1 arguments )
// operation: bigintvec (...)  ->  bigintvec ( number of arguments >0 )
// operation: breakpoint (...)  ->  nothing ( 0 arguments )
// operation: breakpoint (...)  ->  nothing ( number of arguments >0 )
// operation: branchTo (...)  ->  nothing ( number of arguments >0 )
// operation: coef (...)  ->  matrix ( 2 arguments )
// operation: coef (...)  ->  nothing ( 4 arguments )
// operation: division (...)  ->  list ( 2 arguments )
// operation: division (...)  ->  list ( 3 arguments )
// operation: division (...)  ->  list ( 4 arguments )
// operation: dbprint (...)  ->  nothing ( number of arguments >0 )
// operation: fetch (...)  ->  any_type ( 2 arguments )
// operation: fetch (...)  ->  any_type ( 3 arguments )
// operation: fetch (...)  ->  any_type ( 4 arguments )
// operation: hilb (...)  ->  any_type ( 1 arguments )
// operation: hilb (...)  ->  any_type ( 2 arguments )
// operation: hilb (...)  ->  any_type ( 3 arguments )
// operation: ideal (...)  ->  ideal ( 1 arguments )
// operation: ideal (...)  ->  ideal ( any number of arguments )
// operation: intersect (...)  ->  ideal ( 2 arguments )
// operation: intersect (...)  ->  ideal ( 3 arguments )
// operation: intersect (...)  ->  ideal ( number of arguments >0 )
// operation: intvec (...)  ->  intvec ( 1 arguments )
// operation: intvec (...)  ->  intvec ( number of arguments >0 )
// operation: jet (...)  ->  poly ( 2 arguments )
// operation: jet (...)  ->  poly ( 3 arguments )
// operation: jet (...)  ->  poly ( 4 arguments )
// operation: LIB (...)  ->  nothing ( 1 arguments )
// operation: lift (...)  ->  matrix ( 2 arguments )
// operation: lift (...)  ->  matrix ( 3 arguments )
// operation: lift (...)  ->  matrix ( 4 arguments )
// operation: liftstd (...)  ->  ideal ( 2 arguments )
// operation: liftstd (...)  ->  ideal ( 3 arguments )
// operation: liftstd (...)  ->  ideal ( 4 arguments )
// operation: liftstd (...)  ->  ideal ( 5 arguments )
// operation: list (...)  ->  list ( any number of arguments )
// operation: luinverse (...)  ->  list ( number of arguments >0 )
// operation: lusolve (...)  ->  list ( number of arguments >0 )
// operation: minor (...)  ->  nothing ( 1 arguments )
// operation: minor (...)  ->  ideal ( number of arguments >0 )
// operation: module (...)  ->  module ( 1 arguments )
// operation: module (...)  ->  module ( any number of arguments )
// operation: modulo (...)  ->  module ( 2 arguments )
// operation: modulo (...)  ->  module ( 3 arguments )
// operation: modulo (...)  ->  module ( 4 arguments )
// operation: names (...)  ->  list ( 1 arguments )
// operation: names (...)  ->  list ( 0 arguments )
// operation: $INVALID$ (...)  ->  $INVALID$ ( 2 arguments )
// operation: option (...)  ->  string ( any number of arguments )
// operation: reduce (...)  ->  ideal ( 2 arguments )
// operation: reduce (...)  ->  ideal ( 3 arguments )
// operation: reduce (...)  ->  ideal ( 4 arguments )
// operation: reduce (...)  ->  ideal ( 5 arguments )
// operation: reservedName (...)  ->  int ( 1 arguments )
// operation: reservedName (...)  ->  nothing ( 0 arguments )
// operation: reservedNameList (...)  ->  list ( 0 arguments )
// operation: string (...)  ->  string ( any number of arguments )
// operation: subst (...)  ->  nothing ( 3 arguments )
// operation: subst (...)  ->  nothing ( number of arguments >0 )
// operation: system (...)  ->  nothing ( number of arguments >0 )
// operation: test (...)  ->  nothing ( number of arguments >0 )
// operation: write (...)  ->  nothing ( number of arguments >0 )
// operation: status (...)  ->  string ( 2 arguments )
// operation: status (...)  ->  int ( 3 arguments )
// operation: status (...)  ->  int ( 4 arguments )
// operation: simplex (...)  ->  list ( 6 arguments )
// operation: uressolve (...)  ->  list ( 4 arguments )
// operation: std (...)  ->  ideal ( 1 arguments )
// operation: std (...)  ->  ideal ( 2 arguments )
// operation: std (...)  ->  ideal ( 3 arguments )
// operation: std (...)  ->  ideal ( 4 arguments )
// operation: qrds (...)  ->  list ( 4 arguments )
// operation: factmodd (...)  ->  list ( number of arguments >0 )
/*---------------------------------------------*/
// assign: bigint =  bigint
// assign: bigint =  number
// assign: bigintvec =  bigintvec
// assign: bigintvec =  intvec
// assign: bigintmat =  bigintmat
// assign: cring =  cring
// assign: ideal =  ideal
// assign: ideal =  module
// assign: ideal =  matrix
// assign: int =  int
// assign: int =  intmat
// assign: intvec =  intvec
// assign: intmat =  intmat
// assign: link =  string
// assign: link =  link
// assign: list =  resolution
// assign: list =  list
// assign: map =  ideal
// assign: map =  map
// assign: matrix =  matrix
// assign: module =  module
// assign: module =  poly
// assign: number =  number
// assign: package =  package
// assign: poly =  poly
// assign: poly =  matrix
// assign: poly =  polyBucket
// assign: proc =  string
// assign: proc =  proc
// assign: resolution =  resolution
// assign: ring =  ring
// assign: ring =  ideal
// assign: smatrix =  smatrix
// assign: smatrix =  module
// assign: string =  string
// assign: vector =  vector
// assign: def =  def
/*---------------------------------------------*/
// convert bigintvec -> bigintmat
// convert intmat -> bigintmat
// convert int -> bigintvec
// convert intvec -> bigintvec
// convert intvec -> intmat
// convert polyBucket -> ideal
// convert poly -> ideal
// convert bigint -> ideal
// convert int -> ideal
// convert intmat -> matrix
// convert polyBucket -> matrix
// convert ideal -> matrix
// convert module -> matrix
// convert number -> matrix
// convert poly -> matrix
// convert smatrix -> matrix
// convert vector -> matrix
// convert bigint -> matrix
// convert int -> matrix
// convert intvec -> matrix
// convert ideal -> module
// convert matrix -> module
// convert smatrix -> module
// convert vector -> module
// convert bigint -> number
// convert int -> number
// convert polyBucket -> poly
// convert number -> poly
// convert bigint -> poly
// convert int -> poly
// convert list -> resolution
// convert matrix -> smatrix
// convert module -> smatrix
// convert polyBucket -> vector
// convert poly -> vector
// convert bigint -> vector
// convert int -> vector
// convert int -> bigint
// convert int -> intvec
// convert string -> link
// convert resolution -> list
/*---------------------------------------------*/
// token 61 : =
// token 62 : >
// token 60 : <
// token 43 : +
// token 42 : *
// token 47 : /
// token 91 : [
// token 46 : .
// token 94 : ^
// token 44 : ,
// token 37 : %
// token 40 : (
// token 59 : ;
// token 258 : ..
// token 259 : ==
// token 260 : >=
// token 261 : <=
// token 262 : --
// token 263 : not
// token 264 : <>
// token 265 : ++
// token 266 : ::
// token 269 : bigintmat
// token 270 : bigintvec
// token 271 : intmat
// token 272 : proc
// token 273 : ring
// token 275 : polyBucket
// token 276 : ideal
// token 277 : map
// token 278 : matrix
// token 279 : module
// token 280 : number
// token 281 : poly
// token 282 : resolution
// token 283 : smatrix
// token 284 : vector
// token 285 : betti
// token 286 : gen
// token 287 : fetch
// token 288 : freemodule
// token 289 : keepring
// token 290 : imap
// token 291 : koszul
// token 292 : maxideal
// token 293 : monomial
// token 294 : par
// token 295 : preimage
// token 296 : var
// token 298 : degBound
// token 299 : multBound
// token 300 : noether
// token 301 : minpoly
// token 302 : nothing
// token 315 : example
// token 316 : export
// token 318 : kill
// token 319 : LIB
// token 320 : listvar
// token 321 : setring
// token 322 : type
// token 329 : apply
// token 330 : ASSUME
// token 331 : break
// token 332 : continue
// token 333 : else
// token 334 : eval
// token 335 : quote
// token 337 : forif
// token 340 : return
// token 341 : parameter
// token 345 : command
// token 346 : any_type
// token 347 : identifier
// token 358 : alias
// token 359 : align
// token 360 : attrib
// token 361 : bareiss
// token 362 : bigint
// token 363 : branchTo
// token 364 : bracket
// token 365 : breakpoint
// token 366 : char
// token 367 : charstr
// token 368 : char_series
// token 369 : chinrem
// token 373 : close
// token 374 : coeffs
// token 375 : coef
// token 376 : ncols
// token 377 : cleardenom
// token 378 : contract
// token 379 : size
// token 380 : cring
// token 381 : dbprint
// token 382 : def
// token 383 : defined
// token 384 : deg
// token 385 : degree
// token 386 : delete
// token 387 : denominator
// token 388 : det
// token 389 : diff
// token 390 : dim
// token 391 : division
// token 392 : dump
// token 393 : eliminate
// token 394 : RETURN
// token 395 : envelope
// token 396 : ERROR
// token 397 : execute
// token 398 : exportto
// token 399 : extgcd
// token 400 : factorize
// token 401 : farey
// token 402 : find
// token 403 : facstd
// token 404 : factmodd
// token 405 : fres
// token 406 : frwalk
// token 407 : fglm
// token 408 : fglmquot
// token 409 : finduni
// token 410 : gcd
// token 411 : getdump
// token 412 : highcorner
// token 413 : hilb
// token 414 : homog
// token 415 : hres
// token 416 : impart
// token 417 : importfrom
// token 418 : indepSet
// token 419 : insert
// token 420 : int
// token 421 : div
// token 422 : interpolation
// token 423 : interred
// token 424 : intersect
// token 425 : intvec
// token 426 : rvar
// token 427 : jacob
// token 428 : janet
// token 429 : jet
// token 430 : kbase
// token 431 : kernel
// token 432 : killattrib
// token 433 : kres
// token 434 : laguerre
// token 435 : lead
// token 436 : leadcoef
// token 437 : leadexp
// token 438 : leadmonom
// token 439 : liftstd
// token 440 : lift
// token 441 : link
// token 442 : list
// token 443 : load
// token 444 : lres
// token 445 : ludecomp
// token 446 : luinverse
// token 447 : lusolve
// token 448 : memory
// token 449 : minbase
// token 450 : minor
// token 451 : minres
// token 452 : modulo
// token 453 : monitor
// token 454 : mpresmat
// token 455 : mres
// token 456 : mres_map
// token 457 : mstd
// token 458 : mult
// token 459 : nameof
// token 460 : names
// token 461 : newstruct
// token 462 : ncalgebra
// token 463 : nc_algebra
// token 464 : convhull
// token 465 : npars
// token 466 : numerator
// token 467 : nvars
// token 468 : ord
// token 469 : open
// token 470 : oppose
// token 471 : opposite
// token 472 : option
// token 473 : ordstr
// token 474 : package
// token 475 : pardeg
// token 477 : parstr
// token 478 : primefactors
// token 479 : prime
// token 480 : print
// token 481 : prune
// token 482 : prune_map
// token 483 : qhweight
// token 484 : qring
// token 485 : qrds
// token 486 : quotient
// token 487 : random
// token 488 : rank
// token 489 : read
// token 490 : reduce
// token 491 : regularity
// token 492 : repart
// token 493 : nres
// token 494 : reservedName
// token 495 : reservedNameList
// token 496 : restart
// token 497 : resultant
// token 498 : rightstd
// token 499 : ringlist
// token 500 : ring_list
// token 501 : nrows
// token 502 : sba
// token 503 : simplex
// token 504 : simplify
// token 505 : slimgb
// token 506 : sortvec
// token 507 : sqrfree
// token 508 : sres
// token 509 : status
// token 510 : std
// token 511 : string
// token 512 : subst
// token 513 : system
// token 514 : syz
// token 515 : tensor
// token 516 : test
// token 517 : transpose
// token 518 : trace
// token 519 : twostd
// token 520 : typeof
// token 521 : univariate
// token 523 : uressolve
// token 524 : vandermonde
// token 525 : variables
// token 526 : varstr
// token 527 : vdim
// token 528 : waitfirst
// token 529 : waitall
// token 530 : wedge
// token 531 : weight
// token 532 : write
// token 533 : echo
// token 534 : pagewidth
// token 535 : timer
// token 536 : rtimer
// token 537 : TRACE
// token 538 : voice
// token 539 : short
// token 540 : printlevel
/*--max. token: 541, gr: 343 --*/
/*---------------------------------------------*/
const struct sValCmdTab dArithTab1[]=
{
 { 40,11 }, /* ( */
 { 45,2 }, /* - */
 { 262,1 }, /* -- */
 { 263,152 }, /* not */
 { 265,0 }, /* ++ */
 { 269,22 }, /* bigintmat */
 { 270,23 }, /* bigintvec */
 { 271,100 }, /* intmat */
 { 272,174 }, /* proc */
 { 273,190 }, /* ring */
 { 276,86 }, /* ideal */
 { 277,133 }, /* map */
 { 278,134 }, /* matrix */
 { 279,141 }, /* module */
 { 280,153 }, /* number */
 { 281,168 }, /* poly */
 { 282,183 }, /* resolution */
 { 283,204 }, /* smatrix */
 { 284,231 }, /* vector */
 { 285,15 }, /* betti */
 { 286,71 }, /* gen */
 { 288,76 }, /* freemodule */
 { 292,135 }, /* maxideal */
 { 293,143 }, /* monomial */
 { 294,164 }, /* par */
 { 295,170 }, /* preimage */
 { 296,234 }, /* var */
 { 321,201 }, /* setring */
 { 360,12 }, /* attrib */
 { 361,13 }, /* bareiss */
 { 362,19 }, /* bigint */
 { 366,24 }, /* char */
 { 367,26 }, /* charstr */
 { 368,25 }, /* char_series */
 { 371,156 }, /* $INVALID$ */
 { 373,27 }, /* close */
 { 374,28 }, /* coeffs */
 { 376,29 }, /* ncols */
 { 377,35 }, /* cleardenom */
 { 379,37 }, /* size */
 { 380,52 }, /* cring */
 { 382,53 }, /* def */
 { 383,60 }, /* defined */
 { 384,55 }, /* deg */
 { 385,58 }, /* degree */
 { 387,61 }, /* denominator */
 { 388,63 }, /* det */
 { 390,67 }, /* dim */
 { 392,70 }, /* dump */
 { 395,214 }, /* envelope */
 { 396,73 }, /* ERROR */
 { 397,72 }, /* execute */
 { 400,74 }, /* factorize */
 { 403,77 }, /* facstd */
 { 409,75 }, /* finduni */
 { 411,78 }, /* getdump */
 { 412,79 }, /* highcorner */
 { 413,81 }, /* hilb */
 { 414,84 }, /* homog */
 { 416,91 }, /* impart */
 { 418,92 }, /* indepSet */
 { 420,93 }, /* int */
 { 423,98 }, /* interred */
 { 425,102 }, /* intvec */
 { 426,105 }, /* rvar */
 { 427,108 }, /* jacob */
 { 428,111 }, /* janet */
 { 430,112 }, /* kbase */
 { 431,114 }, /* kernel */
 { 432,120 }, /* killattrib */
 { 434,116 }, /* laguerre */
 { 435,121 }, /* lead */
 { 436,125 }, /* leadcoef */
 { 437,127 }, /* leadexp */
 { 438,129 }, /* leadmonom */
 { 441,131 }, /* link */
 { 442,132 }, /* list */
 { 443,239 }, /* load */
 { 445,117 }, /* ludecomp */
 { 448,136 }, /* memory */
 { 449,137 }, /* minbase */
 { 451,139 }, /* minres */
 { 453,142 }, /* monitor */
 { 457,146 }, /* mstd */
 { 458,144 }, /* mult */
 { 459,148 }, /* nameof */
 { 460,149 }, /* names */
 { 464,240 }, /* convhull */
 { 465,157 }, /* npars */
 { 466,62 }, /* numerator */
 { 467,158 }, /* nvars */
 { 468,161 }, /* ord */
 { 469,159 }, /* open */
 { 471,215 }, /* opposite */
 { 472,160 }, /* option */
 { 473,163 }, /* ordstr */
 { 475,165 }, /* pardeg */
 { 477,166 }, /* parstr */
 { 478,118 }, /* primefactors */
 { 479,171 }, /* prime */
 { 480,172 }, /* print */
 { 481,175 }, /* prune */
 { 483,176 }, /* qhweight */
 { 488,178 }, /* rank */
 { 489,179 }, /* read */
 { 491,180 }, /* regularity */
 { 492,181 }, /* repart */
 { 494,182 }, /* reservedName */
 { 496,185 }, /* restart */
 { 498,186 }, /* rightstd */
 { 499,187 }, /* ringlist */
 { 500,188 }, /* ring_list */
 { 501,192 }, /* nrows */
 { 502,199 }, /* sba */
 { 505,202 }, /* slimgb */
 { 506,205 }, /* sortvec */
 { 507,207 }, /* sqrfree */
 { 510,208 }, /* std */
 { 511,211 }, /* string */
 { 514,212 }, /* syz */
 { 517,220 }, /* transpose */
 { 518,218 }, /* trace */
 { 519,216 }, /* twostd */
 { 520,226 }, /* typeof */
 { 521,227 }, /* univariate */
 { 525,228 }, /* variables */
 { 526,235 }, /* varstr */
 { 527,232 }, /* vdim */
 { 528,241 }, /* waitfirst */
 { 529,242 }, /* waitall */
 { 531,237 }, /* weight */
 { 10000,0 }
};
#define JJTAB1LEN 131
const struct sValCmdTab dArithTab2[]=
{
 { 37,119 }, /* % */
 { 38,166 }, /* and */
 { 40,227 }, /* ( */
 { 42,61 }, /* * */
 { 43,1 }, /* + */
 { 45,34 }, /* - */
 { 47,103 }, /* / */
 { 58,232 }, /* : */
 { 60,139 }, /* < */
 { 62,157 }, /* > */
 { 91,208 }, /* [ */
 { 94,125 }, /* ^ */
 { 124,167 }, /* or */
 { 258,207 }, /* .. */
 { 259,168 }, /* == */
 { 260,148 }, /* >= */
 { 261,130 }, /* <= */
 { 264,189 }, /* <> */
 { 266,0 }, /* :: */
 { 285,236 }, /* betti */
 { 287,290 }, /* fetch */
 { 290,310 }, /* imap */
 { 291,327 }, /* koszul */
 { 359,233 }, /* align */
 { 360,235 }, /* attrib */
 { 364,240 }, /* bracket */
 { 369,241 }, /* chinrem */
 { 374,246 }, /* coeffs */
 { 375,244 }, /* coef */
 { 378,250 }, /* contract */
 { 384,251 }, /* deg */
 { 386,254 }, /* delete */
 { 388,261 }, /* det */
 { 389,263 }, /* diff */
 { 390,270 }, /* dim */
 { 391,272 }, /* division */
 { 393,274 }, /* eliminate */
 { 398,278 }, /* exportto */
 { 399,279 }, /* extgcd */
 { 400,282 }, /* factorize */
 { 401,284 }, /* farey */
 { 402,293 }, /* find */
 { 403,283 }, /* facstd */
 { 405,294 }, /* fres */
 { 406,296 }, /* frwalk */
 { 407,291 }, /* fglm */
 { 408,292 }, /* fglmquot */
 { 410,297 }, /* gcd */
 { 413,301 }, /* hilb */
 { 414,303 }, /* homog */
 { 415,309 }, /* hres */
 { 417,311 }, /* importfrom */
 { 418,312 }, /* indepSet */
 { 419,313 }, /* insert */
 { 421,113 }, /* div */
 { 422,314 }, /* interpolation */
 { 424,315 }, /* intersect */
 { 428,317 }, /* janet */
 { 429,318 }, /* jet */
 { 430,323 }, /* kbase */
 { 431,325 }, /* kernel */
 { 432,326 }, /* killattrib */
 { 433,329 }, /* kres */
 { 439,332 }, /* liftstd */
 { 440,330 }, /* lift */
 { 443,334 }, /* load */
 { 444,335 }, /* lres */
 { 452,336 }, /* modulo */
 { 453,338 }, /* monitor */
 { 454,339 }, /* mpresmat */
 { 455,341 }, /* mres */
 { 461,340 }, /* newstruct */
 { 462,345 }, /* ncalgebra */
 { 463,349 }, /* nc_algebra */
 { 470,353 }, /* oppose */
 { 477,354 }, /* parstr */
 { 478,343 }, /* primefactors */
 { 480,355 }, /* print */
 { 482,356 }, /* prune_map */
 { 486,357 }, /* quotient */
 { 487,360 }, /* random */
 { 488,361 }, /* rank */
 { 489,362 }, /* read */
 { 490,363 }, /* reduce */
 { 493,369 }, /* nres */
 { 502,371 }, /* sba */
 { 504,373 }, /* simplify */
 { 507,377 }, /* sqrfree */
 { 508,380 }, /* sres */
 { 509,378 }, /* status */
 { 510,382 }, /* std */
 { 514,388 }, /* syz */
 { 515,390 }, /* tensor */
 { 526,393 }, /* varstr */
 { 528,394 }, /* waitfirst */
 { 529,395 }, /* waitall */
 { 530,396 }, /* wedge */
 { 10000,0 }
};
#define JJTAB2LEN 97
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

// identifier table for Singular
//
void iiInitCmdName()
{
  sArithBase.nCmdUsed      = 0;
  sArithBase.nCmdAllocated = 245;
  sArithBase.sCmds = (cmdnames*)omAlloc0(245/*sArithBase.nCmdAllocated*/ *sizeof(cmdnames));

  // name-string                   alias  tokval toktype index
  iiArithAddCmd("$INVALID$",           0,  -1, 0, 0);
  iiArithAddCmd("ASSUME",              0, 330, 330, 1);
  iiArithAddCmd("ERROR",               0, 396, CMD_1, 2);
  iiArithAddCmd("GCD",                 2, 410, CMD_2, 3);
  iiArithAddCmd("IN",                  1, 435, CMD_1, 4);
  iiArithAddCmd("LIB",                 0, 319, SYSVAR, 5);
  iiArithAddCmd("NF",                  1, 490, CMD_M, 6);
  iiArithAddCmd("RETURN",              0, 394, 340, 7);
  iiArithAddCmd("TRACE",               0, 537, SYSVAR, 8);
  iiArithAddCmd("alias",               0, 358, 341, 9);
  iiArithAddCmd("align",               0, 359, CMD_2, 10);
  iiArithAddCmd("and",                 0,  38, '&', 11);
  iiArithAddCmd("apply",               0, 329, 329, 12);
  iiArithAddCmd("attrib",              0, 360, CMD_123, 13);
  iiArithAddCmd("bareiss",             0, 361, CMD_13, 14);
  iiArithAddCmd("betti",               0, 285, CMD_12, 15);
  iiArithAddCmd("bigint",              0, 362, ROOT_DECL, 16);
  iiArithAddCmd("bigintmat",           0, 269, 269, 17);
  iiArithAddCmd("bigintvec",           0, 270, ROOT_DECL_LIST, 18);
  iiArithAddCmd("bracket",             0, 364, CMD_23, 19);
  iiArithAddCmd("branchTo",            0, 363, CMD_M, 20);
  iiArithAddCmd("break",               0, 331, 331, 21);
  iiArithAddCmd("breakpoint",          0, 365, CMD_M, 22);
  iiArithAddCmd("char",                0, 366, CMD_1, 23);
  iiArithAddCmd("char_series",         0, 368, CMD_1, 24);
  iiArithAddCmd("charstr",             0, 367, CMD_1, 25);
  iiArithAddCmd("chinrem",             0, 369, CMD_2, 26);
  iiArithAddCmd("cleardenom",          0, 377, CMD_1, 27);
  iiArithAddCmd("close",               0, 373, CMD_1, 28);
  iiArithAddCmd("coef",                0, 375, CMD_M, 29);
  iiArithAddCmd("coeffs",              0, 374, CMD_123, 30);
  iiArithAddCmd("continue",            0, 332, 332, 31);
  iiArithAddCmd("contract",            0, 378, CMD_2, 32);
  iiArithAddCmd("convhull",            0, 464, CMD_1, 33);
  iiArithAddCmd("cring",               0, 380, ROOT_DECL, 34);
  iiArithAddCmd("dbprint",             0, 381, CMD_M, 35);
  iiArithAddCmd("def",                 0, 382, ROOT_DECL, 36);
  iiArithAddCmd("defined",             0, 383, CMD_1, 37);
  iiArithAddCmd("deg",                 0, 384, CMD_12, 38);
  iiArithAddCmd("degBound",            0, 298, SYSVAR, 39);
  iiArithAddCmd("degree",              0, 385, CMD_1, 40);
  iiArithAddCmd("delete",              0, 386, CMD_2, 41);
  iiArithAddCmd("denominator",         0, 387, CMD_1, 42);
  iiArithAddCmd("det",                 0, 388, CMD_12, 43);
  iiArithAddCmd("diff",                0, 389, CMD_2, 44);
  iiArithAddCmd("dim",                 0, 390, CMD_12, 45);
  iiArithAddCmd("div",                 0, 421, '/', 46);
  iiArithAddCmd("division",            0, 391, CMD_M, 47);
  iiArithAddCmd("dump",                0, 392, CMD_1, 48);
  iiArithAddCmd("echo",                0, 533, SYSVAR, 49);
  iiArithAddCmd("eliminate",           0, 393, CMD_23, 50);
  iiArithAddCmd("else",                0, 333, 333, 51);
  iiArithAddCmd("envelope",            0, 395, CMD_1, 52);
  iiArithAddCmd("eval",                0, 334, 334, 53);
  iiArithAddCmd("example",             0, 315, 315, 54);
  iiArithAddCmd("execute",             0, 397, CMD_1, 55);
  iiArithAddCmd("export",              0, 316, 316, 56);
  iiArithAddCmd("exportto",            0, 398, CMD_2, 57);
  iiArithAddCmd("extgcd",              0, 399, CMD_2, 58);
  iiArithAddCmd("facstd",              0, 403, CMD_12, 59);
  iiArithAddCmd("factmodd",            0, 404, CMD_M, 60);
  iiArithAddCmd("factorize",           0, 400, CMD_12, 61);
  iiArithAddCmd("farey",               0, 401, CMD_2, 62);
  iiArithAddCmd("fetch",               0, 287, CMD_M, 63);
  iiArithAddCmd("fglm",                0, 407, CMD_2, 64);
  iiArithAddCmd("fglmquot",            0, 408, CMD_2, 65);
  iiArithAddCmd("find",                0, 402, CMD_23, 66);
  iiArithAddCmd("finduni",             0, 409, CMD_1, 67);
  iiArithAddCmd("forif",               0, 337, 337, 68);
  iiArithAddCmd("freemodule",          0, 288, CMD_1, 69);
  iiArithAddCmd("fres",                0, 405, CMD_23, 70);
  iiArithAddCmd("frwalk",              0, 406, CMD_23, 71);
  iiArithAddCmd("gcd",                 0, 410, CMD_2, 72);
  iiArithAddCmd("gen",                 0, 286, CMD_1, 73);
  iiArithAddCmd("getdump",             0, 411, CMD_1, 74);
  iiArithAddCmd("highcorner",          0, 412, CMD_1, 75);
  iiArithAddCmd("hilb",                0, 413, CMD_M, 76);
  iiArithAddCmd("homog",               0, 414, CMD_123, 77);
  iiArithAddCmd("hres",                0, 415, CMD_2, 78);
  iiArithAddCmd("ideal",               0, 276, 314, 79);
  iiArithAddCmd("if",                  0, 337, 337, 80);
  iiArithAddCmd("imap",                0, 290, CMD_2, 81);
  iiArithAddCmd("impart",              0, 416, CMD_1, 82);
  iiArithAddCmd("importfrom",          0, 417, CMD_2, 83);
  iiArithAddCmd("indepSet",            0, 418, CMD_12, 84);
  iiArithAddCmd("insert",              0, 419, CMD_23, 85);
  iiArithAddCmd("int",                 0, 420, ROOT_DECL, 86);
  iiArithAddCmd("interpolation",       0, 422, CMD_2, 87);
  iiArithAddCmd("interred",            0, 423, CMD_1, 88);
  iiArithAddCmd("intersect",           0, 424, CMD_M, 89);
  iiArithAddCmd("intmat",              0, 271, 271, 90);
  iiArithAddCmd("intvec",              0, 425, ROOT_DECL_LIST, 91);
  iiArithAddCmd("jacob",               0, 427, CMD_1, 92);
  iiArithAddCmd("janet",               0, 428, CMD_12, 93);
  iiArithAddCmd("jet",                 0, 429, CMD_M, 94);
  iiArithAddCmd("kbase",               0, 430, CMD_12, 95);
  iiArithAddCmd("keepring",            0, 289, 289, 96);
  iiArithAddCmd("kernel",              0, 431, CMD_12, 97);
  iiArithAddCmd("kill",                0, 318, 318, 98);
  iiArithAddCmd("killattrib",          0, 432, CMD_12, 99);
  iiArithAddCmd("koszul",              0, 291, CMD_23, 100);
  iiArithAddCmd("kres",                0, 433, CMD_2, 101);
  iiArithAddCmd("laguerre",            0, 434, CMD_13, 102);
  iiArithAddCmd("lead",                0, 435, CMD_1, 103);
  iiArithAddCmd("leadcoef",            0, 436, CMD_1, 104);
  iiArithAddCmd("leadexp",             0, 437, CMD_1, 105);
  iiArithAddCmd("leadmonom",           0, 438, CMD_1, 106);
  iiArithAddCmd("lift",                0, 440, CMD_M, 107);
  iiArithAddCmd("liftstd",             0, 439, CMD_M, 108);
  iiArithAddCmd("link",                0, 441, ROOT_DECL, 109);
  iiArithAddCmd("list",                0, 442, ROOT_DECL_LIST, 110);
  iiArithAddCmd("listvar",             0, 320, 320, 111);
  iiArithAddCmd("load",                0, 443, CMD_12, 112);
  iiArithAddCmd("lres",                0, 444, CMD_2, 113);
  iiArithAddCmd("ludecomp",            0, 445, CMD_1, 114);
  iiArithAddCmd("luinverse",           0, 446, CMD_M, 115);
  iiArithAddCmd("lusolve",             0, 447, CMD_M, 116);
  iiArithAddCmd("map",                 0, 277, RING_DECL, 117);
  iiArithAddCmd("matrix",              0, 278, 278, 118);
  iiArithAddCmd("maxideal",            0, 292, CMD_1, 119);
  iiArithAddCmd("memory",              0, 448, CMD_1, 120);
  iiArithAddCmd("minbase",             0, 449, CMD_1, 121);
  iiArithAddCmd("minor",               0, 450, CMD_M, 122);
  iiArithAddCmd("minpoly",             0, 301, SYSVAR, 123);
  iiArithAddCmd("minres",              0, 451, CMD_1, 124);
  iiArithAddCmd("mod",                 0,  37, '/', 125);
  iiArithAddCmd("module",              0, 279, 314, 126);
  iiArithAddCmd("modulo",              0, 452, CMD_M, 127);
  iiArithAddCmd("monitor",             0, 453, CMD_12, 128);
  iiArithAddCmd("monomial",            0, 293, CMD_1, 129);
  iiArithAddCmd("mpresmat",            0, 454, CMD_2, 130);
  iiArithAddCmd("mres",                0, 455, CMD_2, 131);
  iiArithAddCmd("mres_map",            0, 456, CMD_3, 132);
  iiArithAddCmd("mstd",                0, 457, CMD_1, 133);
  iiArithAddCmd("mult",                0, 458, CMD_1, 134);
  iiArithAddCmd("multBound",           0, 299, SYSVAR, 135);
  iiArithAddCmd("multiplicity",        1, 458, CMD_1, 136);
  iiArithAddCmd("nameof",              0, 459, CMD_1, 137);
  iiArithAddCmd("names",               0, 460, CMD_M, 138);
  iiArithAddCmd("nc_algebra",          0, 463, CMD_2, 139);
  iiArithAddCmd("ncalgebra",           2, 462, CMD_2, 140);
  iiArithAddCmd("ncols",               0, 376, CMD_1, 141);
  iiArithAddCmd("newstruct",           0, 461, CMD_23, 142);
  iiArithAddCmd("noether",             0, 300, SYSVAR, 143);
  iiArithAddCmd("not",                 0, 263, 263, 144);
  iiArithAddCmd("npars",               0, 465, CMD_1, 145);
  iiArithAddCmd("nres",                0, 493, CMD_2, 146);
  iiArithAddCmd("nrows",               0, 501, CMD_1, 147);
  iiArithAddCmd("number",              0, 280, RING_DECL, 148);
  iiArithAddCmd("numerator",           0, 466, CMD_1, 149);
  iiArithAddCmd("nvars",               0, 467, CMD_1, 150);
  iiArithAddCmd("open",                0, 469, CMD_1, 151);
  iiArithAddCmd("oppose",              0, 470, CMD_2, 152);
  iiArithAddCmd("opposite",            0, 471, CMD_1, 153);
  iiArithAddCmd("option",              0, 472, CMD_M, 154);
  iiArithAddCmd("or",                  0, 124, '&', 155);
  iiArithAddCmd("ord",                 0, 468, CMD_1, 156);
  iiArithAddCmd("ordstr",              0, 473, CMD_1, 157);
  iiArithAddCmd("package",             0, 474, ROOT_DECL, 158);
  iiArithAddCmd("pagewidth",           0, 534, SYSVAR, 159);
  iiArithAddCmd("par",                 0, 294, CMD_1, 160);
  iiArithAddCmd("parameter",           0, 341, 341, 161);
  iiArithAddCmd("pardeg",              0, 475, CMD_1, 162);
  iiArithAddCmd("parstr",              0, 477, CMD_12, 163);
  iiArithAddCmd("poly",                0, 281, RING_DECL, 164);
  iiArithAddCmd("polyBucket",          0, 275, RING_DECL, 165);
  iiArithAddCmd("preimage",            0, 295, CMD_13, 166);
  iiArithAddCmd("prime",               0, 479, CMD_1, 167);
  iiArithAddCmd("primefactors",        0, 478, CMD_12, 168);
  iiArithAddCmd("print",               0, 480, CMD_12, 169);
  iiArithAddCmd("printlevel",          0, 540, SYSVAR, 170);
  iiArithAddCmd("proc",                0, 272, 272, 171);
  iiArithAddCmd("prune",               0, 481, CMD_1, 172);
  iiArithAddCmd("prune_map",           0, 482, CMD_2, 173);
  iiArithAddCmd("qhweight",            0, 483, CMD_1, 174);
  iiArithAddCmd("qrds",                0, 485, CMD_M, 175);
  iiArithAddCmd("qring",               0, 484, ROOT_DECL, 176);
  iiArithAddCmd("quote",               0, 335, 335, 177);
  iiArithAddCmd("quotient",            0, 486, CMD_2, 178);
  iiArithAddCmd("random",              0, 487, CMD_23, 179);
  iiArithAddCmd("rank",                0, 488, CMD_12, 180);
  iiArithAddCmd("read",                0, 489, CMD_12, 181);
  iiArithAddCmd("reduce",              0, 490, CMD_M, 182);
  iiArithAddCmd("regularity",          0, 491, CMD_1, 183);
  iiArithAddCmd("repart",              0, 492, CMD_1, 184);
  iiArithAddCmd("reservedName",        0, 494, CMD_M, 185);
  iiArithAddCmd("reservedNameList",    0, 495, CMD_M, 186);
  iiArithAddCmd("resolution",          0, 282, RING_DECL, 187);
  iiArithAddCmd("restart",             0, 496, CMD_1, 188);
  iiArithAddCmd("resultant",           0, 497, CMD_3, 189);
  iiArithAddCmd("return",              0, 340, 340, 190);
  iiArithAddCmd("rightstd",            0, 498, CMD_1, 191);
  iiArithAddCmd("ring",                0, 273, 273, 192);
  iiArithAddCmd("ring_list",           0, 500, CMD_1, 193);
  iiArithAddCmd("ringlist",            0, 499, CMD_1, 194);
  iiArithAddCmd("rtimer",              0, 536, SYSVAR, 195);
  iiArithAddCmd("rvar",                0, 426, CMD_1, 196);
  iiArithAddCmd("sba",                 0, 502, CMD_123, 197);
  iiArithAddCmd("setring",             0, 321, 321, 198);
  iiArithAddCmd("short",               0, 539, SYSVAR, 199);
  iiArithAddCmd("simplex",             0, 503, CMD_M, 200);
  iiArithAddCmd("simplify",            0, 504, CMD_2, 201);
  iiArithAddCmd("size",                0, 379, CMD_1, 202);
  iiArithAddCmd("slimgb",              0, 505, CMD_1, 203);
  iiArithAddCmd("smatrix",             0, 283, 283, 204);
  iiArithAddCmd("sortvec",             0, 506, CMD_1, 205);
  iiArithAddCmd("sqrfree",             0, 507, CMD_12, 206);
  iiArithAddCmd("sres",                0, 508, CMD_2, 207);
  iiArithAddCmd("status",              0, 509, CMD_M, 208);
  iiArithAddCmd("std",                 0, 510, CMD_M, 209);
  iiArithAddCmd("string",              0, 511, ROOT_DECL_LIST, 210);
  iiArithAddCmd("subst",               0, 512, CMD_M, 211);
  iiArithAddCmd("system",              0, 513, CMD_M, 212);
  iiArithAddCmd("syz",                 0, 514, CMD_12, 213);
  iiArithAddCmd("tensor",              0, 515, CMD_2, 214);
  iiArithAddCmd("test",                0, 516, CMD_M, 215);
  iiArithAddCmd("timer",               0, 535, SYSVAR, 216);
  iiArithAddCmd("trace",               0, 518, CMD_1, 217);
  iiArithAddCmd("transpose",           0, 517, CMD_1, 218);
  iiArithAddCmd("twostd",              0, 519, CMD_1, 219);
  iiArithAddCmd("type",                0, 322, 322, 220);
  iiArithAddCmd("typeof",              0, 520, CMD_1, 221);
  iiArithAddCmd("univariate",          0, 521, CMD_1, 222);
  iiArithAddCmd("uressolve",           0, 523, CMD_M, 223);
  iiArithAddCmd("vandermonde",         0, 524, CMD_3, 224);
  iiArithAddCmd("var",                 0, 296, CMD_1, 225);
  iiArithAddCmd("variables",           0, 525, CMD_1, 226);
  iiArithAddCmd("varstr",              0, 526, CMD_12, 227);
  iiArithAddCmd("vdim",                0, 527, CMD_1, 228);
  iiArithAddCmd("vector",              0, 284, RING_DECL, 229);
  iiArithAddCmd("verbose",             2, 472, CMD_M, 230);
  iiArithAddCmd("voice",               0, 538, SYSVAR, 231);
  iiArithAddCmd("waitall",             0, 529, CMD_12, 232);
  iiArithAddCmd("waitfirst",           0, 528, CMD_12, 233);
  iiArithAddCmd("wedge",               0, 530, CMD_2, 234);
  iiArithAddCmd("weight",              0, 531, CMD_1, 235);
  iiArithAddCmd("whileif",             0, 337, 337, 236);
  iiArithAddCmd("write",               0, 532, CMD_M, 237);
  iiArithAddCmd("exit",                0,  -1, 0, 238);
  iiArithAddCmd("for",                 0,  -1, 0, 239);
  iiArithAddCmd("help",                0,  -1, 0, 240);
  iiArithAddCmd("newline",             0,  -1, 0, 241);
  iiArithAddCmd("pause",               2,  -1, 0, 242);
  iiArithAddCmd("quit",                0,  -1, 0, 243);
  iiArithAddCmd("while",               0,  -1, 0, 244);
/* end of list marker */
  sArithBase.nLastIdentifier = 237;
}
#define LAST_IDENTIFIER 237
