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
// operation: char (ring)  ->  int
// operation: char_series (ideal)  ->  matrix, only commutative rings, field coeffs
// operation: charstr (ring)  ->  string
// operation: close (link)  ->  nothing
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
// operation: interred (ideal)  ->  ideal, field coeffs
// operation: interred (module)  ->  module, field coeffs
// operation: intmat (bigintmat)  ->  intmat
// operation: intmat (intmat)  ->  intmat
// operation: intvec (intmat)  ->  intvec
// operation: intvec (intvec)  ->  intvec
// operation: rvar (poly)  ->  int
// operation: rvar (string)  ->  int
// operation: rvar (any_type)  ->  int
// operation: jacob (poly)  ->  ideal
// operation: jacob (ideal)  ->  matrix
// operation: jacob (module)  ->  module
// operation: janet (ideal)  ->  ideal, field coeffs
// operation: kbase (ideal)  ->  ideal, QQ coeffs
// operation: kbase (module)  ->  module, QQ coeffs
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
// operation: minres (list)  ->  list
// operation: minres (resolution)  ->  resolution
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
// operation: preimage (map)  ->  ring, only commutative rings
// operation: prime (int)  ->  int
// operation: print (list)  ->  string
// operation: print (def)  ->  string
// operation: proc (proc)  ->  proc
// operation: prune (module)  ->  module
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
// operation: + (bigintmat, int)  ->  bigintmat
// operation: + (int, bigintmat)  ->  bigintmat
// operation: + (bigintmat, bigint)  ->  bigintmat
// operation: + (bigint, bigintmat)  ->  bigintmat
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
// operation: <> (cring, cring)  ->  int
// DUMMY // operation: <> (ideal, ideal)  ->  $INVALID$
// DUMMY // operation: <> (module, module)  ->  $INVALID$
// DUMMY // operation: <> (ideal, module)  ->  $INVALID$
// DUMMY // operation: <> (module, ideal)  ->  $INVALID$
// operation: <> (ring, ring)  ->  int
// operation: .. (int, int)  ->  intvec
// operation: [ (intvec, int)  ->  int
// operation: [ (intvec, intvec)  ->  int
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
// operation: delete (module, int)  ->  module
// operation: delete (list, int)  ->  list
// operation: det (smatrix, string)  ->  poly, only commutative rings, field coeffs
// operation: det (matrix, string)  ->  poly, only commutative rings
// operation: diff (poly, poly)  ->  poly
// operation: diff (vector, poly)  ->  vector
// operation: diff (ideal, poly)  ->  ideal
// operation: diff (ideal, ideal)  ->  matrix
// operation: diff (module, poly)  ->  module
// operation: diff (matrix, poly)  ->  matrix
// operation: diff (number, number)  ->  number
// operation: dim (ideal, ideal)  ->  int, field coeffs
// operation: dim (module, ideal)  ->  int, field coeffs
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
// operation: hilb (ideal, int)  ->  intvec, domain coeffs
// operation: hilb (module, int)  ->  intvec, domain coeffs
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
// operation: mres (ideal, int)  ->  resolution
// operation: mres (module, int)  ->  resolution
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
// operation: nres (ideal, int)  ->  resolution
// operation: nres (module, int)  ->  resolution
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
// operation: std (ideal, intvec)  ->  ideal, field coeffs
// operation: std (module, intvec)  ->  module, field coeffs
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
// operation: eliminate (ideal, poly, intvec)  ->  ideal, only commutative rings
// operation: eliminate (module, poly, intvec)  ->  module, only commutative rings
// operation: eliminate (ideal, poly, string)  ->  ideal
// operation: eliminate (module, poly, string)  ->  module
// operation: find (string, string, int)  ->  int
// operation: fres (ideal, int, string)  ->  resolution, only commutative rings, field coeffs
// operation: fres (module, int, string)  ->  resolution, only commutative rings, field coeffs
// operation: frwalk (ring, def, int)  ->  ideal requires currRing, only commutative rings
// operation: hilb (ideal, int, intvec)  ->  intvec, domain coeffs
// operation: hilb (module, int, intvec)  ->  intvec, domain coeffs
// operation: homog (poly, poly, intvec)  ->  poly
// operation: homog (vector, poly, intvec)  ->  vector
// operation: homog (ideal, poly, intvec)  ->  ideal
// operation: homog (module, poly, intvec)  ->  module
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
// operation: matrix (ideal, int, int)  ->  matrix
// operation: matrix (module, int, int)  ->  matrix
// operation: matrix (matrix, int, int)  ->  matrix
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
// operation: std (ideal, intvec, intvec)  ->  ideal, field coeffs
// operation: std (module, intvec, intvec)  ->  module, field coeffs
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
// operation: list (...)  ->  list ( any number of arguments )
// operation: luinverse (...)  ->  list ( number of arguments >0 )
// operation: lusolve (...)  ->  list ( number of arguments >0 )
// operation: minor (...)  ->  nothing ( 1 arguments )
// operation: minor (...)  ->  ideal ( number of arguments >0 )
// operation: module (...)  ->  module ( 1 arguments )
// operation: module (...)  ->  module ( any number of arguments )
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
// convert intmat -> bigintmat
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
// token 270 : intmat
// token 271 : proc
// token 272 : ring
// token 274 : polyBucket
// token 275 : ideal
// token 276 : map
// token 277 : matrix
// token 278 : module
// token 279 : number
// token 280 : poly
// token 281 : resolution
// token 282 : smatrix
// token 283 : vector
// token 284 : betti
// token 285 : gen
// token 286 : fetch
// token 287 : freemodule
// token 288 : keepring
// token 289 : imap
// token 290 : koszul
// token 291 : maxideal
// token 292 : monomial
// token 293 : par
// token 294 : preimage
// token 295 : var
// token 297 : degBound
// token 298 : multBound
// token 299 : noether
// token 300 : minpoly
// token 301 : nothing
// token 314 : example
// token 315 : export
// token 317 : kill
// token 318 : LIB
// token 319 : listvar
// token 320 : setring
// token 321 : type
// token 328 : apply
// token 329 : ASSUME
// token 330 : break
// token 331 : continue
// token 332 : else
// token 333 : eval
// token 334 : quote
// token 336 : forif
// token 339 : return
// token 340 : parameter
// token 344 : command
// token 345 : any_type
// token 346 : identifier
// token 357 : alias
// token 358 : align
// token 359 : attrib
// token 360 : bareiss
// token 361 : bigint
// token 362 : branchTo
// token 363 : bracket
// token 364 : breakpoint
// token 365 : char
// token 366 : charstr
// token 367 : char_series
// token 368 : chinrem
// token 372 : close
// token 373 : coeffs
// token 374 : coef
// token 375 : ncols
// token 376 : cleardenom
// token 377 : contract
// token 378 : size
// token 379 : cring
// token 380 : dbprint
// token 381 : def
// token 382 : defined
// token 383 : deg
// token 384 : degree
// token 385 : delete
// token 386 : denominator
// token 387 : det
// token 388 : diff
// token 389 : dim
// token 390 : division
// token 391 : dump
// token 392 : eliminate
// token 393 : RETURN
// token 394 : envelope
// token 395 : ERROR
// token 396 : execute
// token 397 : exportto
// token 398 : extgcd
// token 399 : factorize
// token 400 : farey
// token 401 : find
// token 402 : facstd
// token 403 : factmodd
// token 404 : fres
// token 405 : frwalk
// token 406 : fglm
// token 407 : fglmquot
// token 408 : finduni
// token 409 : gcd
// token 410 : getdump
// token 411 : highcorner
// token 412 : hilb
// token 413 : homog
// token 414 : hres
// token 415 : impart
// token 416 : importfrom
// token 417 : indepSet
// token 418 : insert
// token 419 : int
// token 420 : div
// token 421 : interpolation
// token 422 : interred
// token 423 : intersect
// token 424 : intvec
// token 425 : rvar
// token 426 : jacob
// token 427 : janet
// token 428 : jet
// token 429 : kbase
// token 430 : kernel
// token 431 : killattrib
// token 432 : kres
// token 433 : laguerre
// token 434 : lead
// token 435 : leadcoef
// token 436 : leadexp
// token 437 : leadmonom
// token 438 : liftstd
// token 439 : lift
// token 440 : link
// token 441 : list
// token 442 : load
// token 443 : lres
// token 444 : ludecomp
// token 445 : luinverse
// token 446 : lusolve
// token 447 : memory
// token 448 : minbase
// token 449 : minor
// token 450 : minres
// token 451 : modulo
// token 452 : monitor
// token 453 : mpresmat
// token 454 : mres
// token 455 : mstd
// token 456 : mult
// token 457 : nameof
// token 458 : names
// token 459 : newstruct
// token 460 : ncalgebra
// token 461 : nc_algebra
// token 462 : convhull
// token 463 : npars
// token 464 : numerator
// token 465 : nvars
// token 466 : ord
// token 467 : open
// token 468 : oppose
// token 469 : opposite
// token 470 : option
// token 471 : ordstr
// token 472 : package
// token 473 : pardeg
// token 475 : parstr
// token 476 : primefactors
// token 477 : prime
// token 478 : print
// token 479 : prune
// token 480 : qhweight
// token 481 : qring
// token 482 : qrds
// token 483 : quotient
// token 484 : random
// token 485 : rank
// token 486 : read
// token 487 : reduce
// token 488 : regularity
// token 489 : repart
// token 490 : nres
// token 491 : reservedName
// token 492 : reservedNameList
// token 493 : restart
// token 494 : resultant
// token 495 : rightstd
// token 496 : ringlist
// token 497 : ring_list
// token 498 : nrows
// token 499 : sba
// token 500 : simplex
// token 501 : simplify
// token 502 : slimgb
// token 503 : sortvec
// token 504 : sqrfree
// token 505 : sres
// token 506 : status
// token 507 : std
// token 508 : string
// token 509 : subst
// token 510 : system
// token 511 : syz
// token 512 : tensor
// token 513 : test
// token 514 : transpose
// token 515 : trace
// token 516 : twostd
// token 517 : typeof
// token 518 : univariate
// token 520 : uressolve
// token 521 : vandermonde
// token 522 : variables
// token 523 : varstr
// token 524 : vdim
// token 525 : waitfirst
// token 526 : waitall
// token 527 : wedge
// token 528 : weight
// token 529 : write
// token 530 : echo
// token 531 : pagewidth
// token 532 : timer
// token 533 : rtimer
// token 534 : TRACE
// token 535 : voice
// token 536 : short
// token 537 : printlevel
/*--max. token: 538, gr: 342 --*/
/*---------------------------------------------*/
const struct sValCmdTab dArithTab1[]=
{
 { 40,11 },
 { 45,2 },
 { 262,1 },
 { 263,146 },
 { 265,0 },
 { 269,22 },
 { 270,97 },
 { 271,168 },
 { 272,184 },
 { 275,83 },
 { 276,127 },
 { 277,128 },
 { 278,135 },
 { 279,147 },
 { 280,162 },
 { 281,177 },
 { 282,198 },
 { 283,225 },
 { 284,15 },
 { 285,68 },
 { 287,73 },
 { 291,129 },
 { 292,137 },
 { 293,158 },
 { 294,164 },
 { 295,228 },
 { 320,195 },
 { 359,12 },
 { 360,13 },
 { 361,19 },
 { 365,23 },
 { 366,25 },
 { 367,24 },
 { 370,150 },
 { 372,26 },
 { 375,27 },
 { 376,33 },
 { 378,35 },
 { 379,49 },
 { 381,50 },
 { 382,57 },
 { 383,52 },
 { 384,55 },
 { 386,58 },
 { 387,60 },
 { 389,64 },
 { 391,67 },
 { 394,208 },
 { 395,70 },
 { 396,69 },
 { 399,71 },
 { 402,74 },
 { 408,72 },
 { 410,75 },
 { 411,76 },
 { 412,78 },
 { 413,81 },
 { 415,88 },
 { 417,89 },
 { 419,90 },
 { 422,95 },
 { 424,99 },
 { 425,101 },
 { 426,104 },
 { 427,107 },
 { 429,108 },
 { 431,114 },
 { 433,110 },
 { 434,115 },
 { 435,119 },
 { 436,121 },
 { 437,123 },
 { 440,125 },
 { 441,126 },
 { 442,233 },
 { 444,111 },
 { 447,130 },
 { 448,131 },
 { 450,133 },
 { 452,136 },
 { 455,140 },
 { 456,138 },
 { 457,142 },
 { 458,143 },
 { 462,234 },
 { 463,151 },
 { 464,59 },
 { 465,152 },
 { 466,155 },
 { 467,153 },
 { 469,209 },
 { 470,154 },
 { 471,157 },
 { 473,159 },
 { 475,160 },
 { 476,112 },
 { 477,165 },
 { 478,166 },
 { 479,169 },
 { 480,170 },
 { 485,172 },
 { 486,173 },
 { 488,174 },
 { 489,175 },
 { 491,176 },
 { 493,179 },
 { 495,180 },
 { 496,181 },
 { 497,182 },
 { 498,186 },
 { 499,193 },
 { 502,196 },
 { 503,199 },
 { 504,201 },
 { 507,202 },
 { 508,205 },
 { 511,206 },
 { 514,214 },
 { 515,212 },
 { 516,210 },
 { 517,220 },
 { 518,221 },
 { 522,222 },
 { 523,229 },
 { 524,226 },
 { 525,235 },
 { 526,236 },
 { 528,231 },
 { 10000,0 }
};
#define JJTAB1LEN 128
const struct sValCmdTab dArithTab2[]=
{
 { 37,105 },
 { 38,152 },
 { 40,208 },
 { 42,51 },
 { 43,1 },
 { 45,29 },
 { 47,89 },
 { 58,213 },
 { 60,125 },
 { 62,143 },
 { 91,191 },
 { 94,111 },
 { 124,153 },
 { 258,190 },
 { 259,154 },
 { 260,134 },
 { 261,116 },
 { 264,174 },
 { 266,0 },
 { 284,217 },
 { 286,267 },
 { 289,287 },
 { 290,304 },
 { 358,214 },
 { 359,216 },
 { 363,221 },
 { 368,222 },
 { 373,227 },
 { 374,225 },
 { 377,231 },
 { 383,232 },
 { 385,235 },
 { 387,239 },
 { 388,241 },
 { 389,248 },
 { 390,250 },
 { 392,252 },
 { 397,256 },
 { 398,257 },
 { 399,260 },
 { 400,262 },
 { 401,270 },
 { 402,261 },
 { 404,271 },
 { 405,273 },
 { 406,268 },
 { 407,269 },
 { 409,274 },
 { 412,278 },
 { 413,280 },
 { 414,286 },
 { 416,288 },
 { 417,289 },
 { 418,290 },
 { 420,99 },
 { 421,291 },
 { 423,292 },
 { 427,294 },
 { 428,295 },
 { 429,300 },
 { 430,302 },
 { 431,303 },
 { 432,306 },
 { 438,309 },
 { 439,307 },
 { 442,311 },
 { 443,312 },
 { 451,313 },
 { 452,315 },
 { 453,316 },
 { 454,318 },
 { 459,317 },
 { 460,322 },
 { 461,326 },
 { 468,330 },
 { 475,331 },
 { 476,320 },
 { 478,332 },
 { 483,333 },
 { 484,336 },
 { 485,337 },
 { 486,338 },
 { 487,339 },
 { 490,345 },
 { 499,347 },
 { 501,349 },
 { 504,353 },
 { 505,356 },
 { 506,354 },
 { 507,358 },
 { 511,364 },
 { 512,366 },
 { 523,369 },
 { 525,370 },
 { 526,371 },
 { 527,372 },
 { 10000,0 }
};
#define JJTAB2LEN 96
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

// identifier table for Singular
//
void iiInitCmdName()
{
  sArithBase.nCmdUsed      = 0;
  sArithBase.nCmdAllocated = 242;
  sArithBase.sCmds = (cmdnames*)omAlloc0(242/*sArithBase.nCmdAllocated*/ *sizeof(cmdnames));

  // name-string                   alias  tokval toktype index
  iiArithAddCmd("$INVALID$",           0,  -1, 0, 0);
  iiArithAddCmd("ASSUME",              0, 329, 329, 1);
  iiArithAddCmd("ERROR",               0, 395, CMD_1, 2);
  iiArithAddCmd("GCD",                 2, 409, CMD_2, 3);
  iiArithAddCmd("IN",                  1, 434, CMD_1, 4);
  iiArithAddCmd("LIB",                 0, 318, SYSVAR, 5);
  iiArithAddCmd("NF",                  1, 487, CMD_M, 6);
  iiArithAddCmd("RETURN",              0, 393, 339, 7);
  iiArithAddCmd("TRACE",               0, 534, SYSVAR, 8);
  iiArithAddCmd("alias",               0, 357, 340, 9);
  iiArithAddCmd("align",               0, 358, CMD_2, 10);
  iiArithAddCmd("and",                 0,  38, '&', 11);
  iiArithAddCmd("apply",               0, 328, 328, 12);
  iiArithAddCmd("attrib",              0, 359, CMD_123, 13);
  iiArithAddCmd("bareiss",             0, 360, CMD_13, 14);
  iiArithAddCmd("betti",               0, 284, CMD_12, 15);
  iiArithAddCmd("bigint",              0, 361, ROOT_DECL, 16);
  iiArithAddCmd("bigintmat",           0, 269, 269, 17);
  iiArithAddCmd("bracket",             0, 363, CMD_23, 18);
  iiArithAddCmd("branchTo",            0, 362, CMD_M, 19);
  iiArithAddCmd("break",               0, 330, 330, 20);
  iiArithAddCmd("breakpoint",          0, 364, CMD_M, 21);
  iiArithAddCmd("char",                0, 365, CMD_1, 22);
  iiArithAddCmd("char_series",         0, 367, CMD_1, 23);
  iiArithAddCmd("charstr",             0, 366, CMD_1, 24);
  iiArithAddCmd("chinrem",             0, 368, CMD_2, 25);
  iiArithAddCmd("cleardenom",          0, 376, CMD_1, 26);
  iiArithAddCmd("close",               0, 372, CMD_1, 27);
  iiArithAddCmd("coef",                0, 374, CMD_M, 28);
  iiArithAddCmd("coeffs",              0, 373, CMD_23, 29);
  iiArithAddCmd("continue",            0, 331, 331, 30);
  iiArithAddCmd("contract",            0, 377, CMD_2, 31);
  iiArithAddCmd("convhull",            0, 462, CMD_1, 32);
  iiArithAddCmd("cring",               0, 379, ROOT_DECL, 33);
  iiArithAddCmd("dbprint",             0, 380, CMD_M, 34);
  iiArithAddCmd("def",                 0, 381, ROOT_DECL, 35);
  iiArithAddCmd("defined",             0, 382, CMD_1, 36);
  iiArithAddCmd("deg",                 0, 383, CMD_12, 37);
  iiArithAddCmd("degBound",            0, 297, SYSVAR, 38);
  iiArithAddCmd("degree",              0, 384, CMD_1, 39);
  iiArithAddCmd("delete",              0, 385, CMD_2, 40);
  iiArithAddCmd("denominator",         0, 386, CMD_1, 41);
  iiArithAddCmd("det",                 0, 387, CMD_12, 42);
  iiArithAddCmd("diff",                0, 388, CMD_2, 43);
  iiArithAddCmd("dim",                 0, 389, CMD_12, 44);
  iiArithAddCmd("div",                 0, 420, '/', 45);
  iiArithAddCmd("division",            0, 390, CMD_M, 46);
  iiArithAddCmd("dump",                0, 391, CMD_1, 47);
  iiArithAddCmd("echo",                0, 530, SYSVAR, 48);
  iiArithAddCmd("eliminate",           0, 392, CMD_23, 49);
  iiArithAddCmd("else",                0, 332, 332, 50);
  iiArithAddCmd("envelope",            0, 394, CMD_1, 51);
  iiArithAddCmd("eval",                0, 333, 333, 52);
  iiArithAddCmd("example",             0, 314, 314, 53);
  iiArithAddCmd("execute",             0, 396, CMD_1, 54);
  iiArithAddCmd("export",              0, 315, 315, 55);
  iiArithAddCmd("exportto",            0, 397, CMD_2, 56);
  iiArithAddCmd("extgcd",              0, 398, CMD_2, 57);
  iiArithAddCmd("facstd",              0, 402, CMD_12, 58);
  iiArithAddCmd("factmodd",            0, 403, CMD_M, 59);
  iiArithAddCmd("factorize",           0, 399, CMD_12, 60);
  iiArithAddCmd("farey",               0, 400, CMD_2, 61);
  iiArithAddCmd("fetch",               0, 286, CMD_M, 62);
  iiArithAddCmd("fglm",                0, 406, CMD_2, 63);
  iiArithAddCmd("fglmquot",            0, 407, CMD_2, 64);
  iiArithAddCmd("find",                0, 401, CMD_23, 65);
  iiArithAddCmd("finduni",             0, 408, CMD_1, 66);
  iiArithAddCmd("forif",               0, 336, 336, 67);
  iiArithAddCmd("freemodule",          0, 287, CMD_1, 68);
  iiArithAddCmd("fres",                0, 404, CMD_23, 69);
  iiArithAddCmd("frwalk",              0, 405, CMD_23, 70);
  iiArithAddCmd("gcd",                 0, 409, CMD_2, 71);
  iiArithAddCmd("gen",                 0, 285, CMD_1, 72);
  iiArithAddCmd("getdump",             0, 410, CMD_1, 73);
  iiArithAddCmd("highcorner",          0, 411, CMD_1, 74);
  iiArithAddCmd("hilb",                0, 412, CMD_123, 75);
  iiArithAddCmd("homog",               0, 413, CMD_123, 76);
  iiArithAddCmd("hres",                0, 414, CMD_2, 77);
  iiArithAddCmd("ideal",               0, 275, 313, 78);
  iiArithAddCmd("if",                  0, 336, 336, 79);
  iiArithAddCmd("imap",                0, 289, CMD_2, 80);
  iiArithAddCmd("impart",              0, 415, CMD_1, 81);
  iiArithAddCmd("importfrom",          0, 416, CMD_2, 82);
  iiArithAddCmd("indepSet",            0, 417, CMD_12, 83);
  iiArithAddCmd("insert",              0, 418, CMD_23, 84);
  iiArithAddCmd("int",                 0, 419, ROOT_DECL, 85);
  iiArithAddCmd("interpolation",       0, 421, CMD_2, 86);
  iiArithAddCmd("interred",            0, 422, CMD_1, 87);
  iiArithAddCmd("intersect",           0, 423, CMD_M, 88);
  iiArithAddCmd("intmat",              0, 270, 270, 89);
  iiArithAddCmd("intvec",              0, 424, ROOT_DECL_LIST, 90);
  iiArithAddCmd("jacob",               0, 426, CMD_1, 91);
  iiArithAddCmd("janet",               0, 427, CMD_12, 92);
  iiArithAddCmd("jet",                 0, 428, CMD_M, 93);
  iiArithAddCmd("kbase",               0, 429, CMD_12, 94);
  iiArithAddCmd("keepring",            0, 288, 288, 95);
  iiArithAddCmd("kernel",              0, 430, CMD_2, 96);
  iiArithAddCmd("kill",                0, 317, 317, 97);
  iiArithAddCmd("killattrib",          0, 431, CMD_12, 98);
  iiArithAddCmd("koszul",              0, 290, CMD_23, 99);
  iiArithAddCmd("kres",                0, 432, CMD_2, 100);
  iiArithAddCmd("laguerre",            0, 433, CMD_13, 101);
  iiArithAddCmd("lead",                0, 434, CMD_1, 102);
  iiArithAddCmd("leadcoef",            0, 435, CMD_1, 103);
  iiArithAddCmd("leadexp",             0, 436, CMD_1, 104);
  iiArithAddCmd("leadmonom",           0, 437, CMD_1, 105);
  iiArithAddCmd("lift",                0, 439, CMD_M, 106);
  iiArithAddCmd("liftstd",             0, 438, CMD_M, 107);
  iiArithAddCmd("link",                0, 440, ROOT_DECL, 108);
  iiArithAddCmd("list",                0, 441, ROOT_DECL_LIST, 109);
  iiArithAddCmd("listvar",             0, 319, 319, 110);
  iiArithAddCmd("load",                0, 442, CMD_12, 111);
  iiArithAddCmd("lres",                0, 443, CMD_2, 112);
  iiArithAddCmd("ludecomp",            0, 444, CMD_1, 113);
  iiArithAddCmd("luinverse",           0, 445, CMD_M, 114);
  iiArithAddCmd("lusolve",             0, 446, CMD_M, 115);
  iiArithAddCmd("map",                 0, 276, RING_DECL, 116);
  iiArithAddCmd("matrix",              0, 277, 277, 117);
  iiArithAddCmd("maxideal",            0, 291, CMD_1, 118);
  iiArithAddCmd("memory",              0, 447, CMD_1, 119);
  iiArithAddCmd("minbase",             0, 448, CMD_1, 120);
  iiArithAddCmd("minor",               0, 449, CMD_M, 121);
  iiArithAddCmd("minpoly",             0, 300, SYSVAR, 122);
  iiArithAddCmd("minres",              0, 450, CMD_1, 123);
  iiArithAddCmd("mod",                 0,  37, '/', 124);
  iiArithAddCmd("module",              0, 278, 313, 125);
  iiArithAddCmd("modulo",              0, 451, CMD_2, 126);
  iiArithAddCmd("monitor",             0, 452, CMD_12, 127);
  iiArithAddCmd("monomial",            0, 292, CMD_1, 128);
  iiArithAddCmd("mpresmat",            0, 453, CMD_2, 129);
  iiArithAddCmd("mres",                0, 454, CMD_2, 130);
  iiArithAddCmd("mstd",                0, 455, CMD_1, 131);
  iiArithAddCmd("mult",                0, 456, CMD_1, 132);
  iiArithAddCmd("multBound",           0, 298, SYSVAR, 133);
  iiArithAddCmd("multiplicity",        1, 456, CMD_1, 134);
  iiArithAddCmd("nameof",              0, 457, CMD_1, 135);
  iiArithAddCmd("names",               0, 458, CMD_M, 136);
  iiArithAddCmd("nc_algebra",          0, 461, CMD_2, 137);
  iiArithAddCmd("ncalgebra",           2, 460, CMD_2, 138);
  iiArithAddCmd("ncols",               0, 375, CMD_1, 139);
  iiArithAddCmd("newstruct",           0, 459, CMD_23, 140);
  iiArithAddCmd("noether",             0, 299, SYSVAR, 141);
  iiArithAddCmd("not",                 0, 263, 263, 142);
  iiArithAddCmd("npars",               0, 463, CMD_1, 143);
  iiArithAddCmd("nres",                0, 490, CMD_2, 144);
  iiArithAddCmd("nrows",               0, 498, CMD_1, 145);
  iiArithAddCmd("number",              0, 279, RING_DECL, 146);
  iiArithAddCmd("numerator",           0, 464, CMD_1, 147);
  iiArithAddCmd("nvars",               0, 465, CMD_1, 148);
  iiArithAddCmd("open",                0, 467, CMD_1, 149);
  iiArithAddCmd("oppose",              0, 468, CMD_2, 150);
  iiArithAddCmd("opposite",            0, 469, CMD_1, 151);
  iiArithAddCmd("option",              0, 470, CMD_M, 152);
  iiArithAddCmd("or",                  0, 124, '&', 153);
  iiArithAddCmd("ord",                 0, 466, CMD_1, 154);
  iiArithAddCmd("ordstr",              0, 471, CMD_1, 155);
  iiArithAddCmd("package",             0, 472, ROOT_DECL, 156);
  iiArithAddCmd("pagewidth",           0, 531, SYSVAR, 157);
  iiArithAddCmd("par",                 0, 293, CMD_1, 158);
  iiArithAddCmd("parameter",           0, 340, 340, 159);
  iiArithAddCmd("pardeg",              0, 473, CMD_1, 160);
  iiArithAddCmd("parstr",              0, 475, CMD_12, 161);
  iiArithAddCmd("poly",                0, 280, RING_DECL, 162);
  iiArithAddCmd("polyBucket",          0, 274, RING_DECL, 163);
  iiArithAddCmd("preimage",            0, 294, CMD_13, 164);
  iiArithAddCmd("prime",               0, 477, CMD_1, 165);
  iiArithAddCmd("primefactors",        0, 476, CMD_12, 166);
  iiArithAddCmd("print",               0, 478, CMD_12, 167);
  iiArithAddCmd("printlevel",          0, 537, SYSVAR, 168);
  iiArithAddCmd("proc",                0, 271, 271, 169);
  iiArithAddCmd("prune",               0, 479, CMD_1, 170);
  iiArithAddCmd("qhweight",            0, 480, CMD_1, 171);
  iiArithAddCmd("qrds",                0, 482, CMD_M, 172);
  iiArithAddCmd("qring",               0, 481, ROOT_DECL, 173);
  iiArithAddCmd("quote",               0, 334, 334, 174);
  iiArithAddCmd("quotient",            0, 483, CMD_2, 175);
  iiArithAddCmd("random",              0, 484, CMD_23, 176);
  iiArithAddCmd("rank",                0, 485, CMD_12, 177);
  iiArithAddCmd("read",                0, 486, CMD_12, 178);
  iiArithAddCmd("reduce",              0, 487, CMD_M, 179);
  iiArithAddCmd("regularity",          0, 488, CMD_1, 180);
  iiArithAddCmd("repart",              0, 489, CMD_1, 181);
  iiArithAddCmd("reservedName",        0, 491, CMD_M, 182);
  iiArithAddCmd("reservedNameList",    0, 492, CMD_M, 183);
  iiArithAddCmd("resolution",          0, 281, RING_DECL, 184);
  iiArithAddCmd("restart",             0, 493, CMD_1, 185);
  iiArithAddCmd("resultant",           0, 494, CMD_3, 186);
  iiArithAddCmd("return",              0, 339, 339, 187);
  iiArithAddCmd("rightstd",            0, 495, CMD_1, 188);
  iiArithAddCmd("ring",                0, 272, 272, 189);
  iiArithAddCmd("ring_list",           0, 497, CMD_1, 190);
  iiArithAddCmd("ringlist",            0, 496, CMD_1, 191);
  iiArithAddCmd("rtimer",              0, 533, SYSVAR, 192);
  iiArithAddCmd("rvar",                0, 425, CMD_1, 193);
  iiArithAddCmd("sba",                 0, 499, CMD_123, 194);
  iiArithAddCmd("setring",             0, 320, 320, 195);
  iiArithAddCmd("short",               0, 536, SYSVAR, 196);
  iiArithAddCmd("simplex",             0, 500, CMD_M, 197);
  iiArithAddCmd("simplify",            0, 501, CMD_2, 198);
  iiArithAddCmd("size",                0, 378, CMD_1, 199);
  iiArithAddCmd("slimgb",              0, 502, CMD_1, 200);
  iiArithAddCmd("smatrix",             0, 282, 282, 201);
  iiArithAddCmd("sortvec",             0, 503, CMD_1, 202);
  iiArithAddCmd("sqrfree",             0, 504, CMD_12, 203);
  iiArithAddCmd("sres",                0, 505, CMD_2, 204);
  iiArithAddCmd("status",              0, 506, CMD_M, 205);
  iiArithAddCmd("std",                 0, 507, CMD_M, 206);
  iiArithAddCmd("string",              0, 508, ROOT_DECL_LIST, 207);
  iiArithAddCmd("subst",               0, 509, CMD_M, 208);
  iiArithAddCmd("system",              0, 510, CMD_M, 209);
  iiArithAddCmd("syz",                 0, 511, CMD_12, 210);
  iiArithAddCmd("tensor",              0, 512, CMD_2, 211);
  iiArithAddCmd("test",                0, 513, CMD_M, 212);
  iiArithAddCmd("timer",               0, 532, SYSVAR, 213);
  iiArithAddCmd("trace",               0, 515, CMD_1, 214);
  iiArithAddCmd("transpose",           0, 514, CMD_1, 215);
  iiArithAddCmd("twostd",              0, 516, CMD_1, 216);
  iiArithAddCmd("type",                0, 321, 321, 217);
  iiArithAddCmd("typeof",              0, 517, CMD_1, 218);
  iiArithAddCmd("univariate",          0, 518, CMD_1, 219);
  iiArithAddCmd("uressolve",           0, 520, CMD_M, 220);
  iiArithAddCmd("vandermonde",         0, 521, CMD_3, 221);
  iiArithAddCmd("var",                 0, 295, CMD_1, 222);
  iiArithAddCmd("variables",           0, 522, CMD_1, 223);
  iiArithAddCmd("varstr",              0, 523, CMD_12, 224);
  iiArithAddCmd("vdim",                0, 524, CMD_1, 225);
  iiArithAddCmd("vector",              0, 283, RING_DECL, 226);
  iiArithAddCmd("verbose",             2, 470, CMD_M, 227);
  iiArithAddCmd("voice",               0, 535, SYSVAR, 228);
  iiArithAddCmd("waitall",             0, 526, CMD_12, 229);
  iiArithAddCmd("waitfirst",           0, 525, CMD_12, 230);
  iiArithAddCmd("wedge",               0, 527, CMD_2, 231);
  iiArithAddCmd("weight",              0, 528, CMD_1, 232);
  iiArithAddCmd("whileif",             0, 336, 336, 233);
  iiArithAddCmd("write",               0, 529, CMD_M, 234);
  iiArithAddCmd("exit",                0,  -1, 0, 235);
  iiArithAddCmd("for",                 0,  -1, 0, 236);
  iiArithAddCmd("help",                0,  -1, 0, 237);
  iiArithAddCmd("newline",             0,  -1, 0, 238);
  iiArithAddCmd("pause",               2,  -1, 0, 239);
  iiArithAddCmd("quit",                0,  -1, 0, 240);
  iiArithAddCmd("while",               0,  -1, 0, 241);
/* end of list marker */
  sArithBase.nLastIdentifier = 234;
}
#define LAST_IDENTIFIER 234
