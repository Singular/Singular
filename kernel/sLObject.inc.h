// This file is intended to be used in kutil.h and SDkutil.h
// to ensure, that sLObject and ShiftDVec::sLObject define
// the same members (except maybe for their inherited part).
// See SDkutil.h for explanations, why this is needed.
{
public:
  unsigned long sev;
  unsigned long from; // from which polynomial it comes from
            // this is important for signature-based
            // algorithms
  unsigned long checked; // this is the index of S up to which
                      // the corresponding LObject was already checked in
                      // critical pair creation => when entering the
                      // reduction process it is enough to start a second
                      // rewritten criterion check from checked+1 onwards
  poly  p1,p2; /*- the pair p comes from,
                 lm(pi) in currRing, tail(pi) in tailring -*/

  poly  lcm;   /*- the lcm of p1,p2 -*/

  poly last;   // pLast(p) during reductions
  kBucket_pt bucket;
  int   i_r1, i_r2;

  // initialization
  KINLINE void Init(ring tailRing = currRing);
  KINLINE sLObject(ring tailRing = currRing);
  KINLINE sLObject(poly p, ring tailRing = currRing);
  KINLINE sLObject(poly p, ring c_r, ring tailRing);

  // Frees the polys of L
  KINLINE void Delete();
  KINLINE void Clear();

  // Iterations
  KINLINE void LmDeleteAndIter();
  KINLINE poly LmExtractAndIter();

  // spoly related things
  // preparation for reduction if not spoly
  KINLINE void PrepareRed(BOOLEAN use_bucket);
  KINLINE void SetLmTail(poly lm, poly new_p, int length,
                         int use_bucket, ring r);
  KINLINE void Tail_Minus_mm_Mult_qq
    (poly m, poly qq, int lq, poly spNoether);
#ifdef HAVE_SHIFTBBADVEC
  KINLINE void Tail_Minus_mml_Mult_qq_Mult_mmr
    (poly mml, poly q, poly mmr, int lq, poly spNoether);
#endif
  KINLINE void Tail_Mult_nn(number n);
  // deletes bucket, makes sure that p and t_p exists
  KINLINE poly GetP(omBin lmBin = NULL);
  // similar, except that only t_p exists
  KINLINE poly GetTP();

  // does not delete bucket, just canonicalizes it
  // returned poly is such that Lm(p) \in currRing, Tail(p) \in tailRing
  KINLINE poly CanonicalizeP();

  // makes a copy of the poly of L
  KINLINE void Copy();
  // gets the poly and makes a copy of it
  KINLINE poly CopyGetP();

  KINLINE int GetpLength();
  KINLINE long pLDeg(BOOLEAN use_last);
  KINLINE long pLDeg();
  KINLINE int SetLength(BOOLEAN lengt_pLength = FALSE);
  KINLINE long SetDegStuffReturnLDeg();
  KINLINE long SetDegStuffReturnLDeg(BOOLEAN use_last);

  // returns minimal component of p
  KINLINE long MinComp();
  // returns component of p
  KINLINE long Comp();

  KINLINE void ShallowCopyDelete(ring new_tailRing,
                                 pShallowCopyDeleteProc p_shallow_copy_delete);

  // sets sev
  KINLINE void SetShortExpVector();

  // enable assignment from TObject
  KINLINE sLObject& operator=(const sTObject&);

  // get T's corresponding to p1, p2: they might return NULL
  KINLINE TObject* T_1(const skStrategy* strat);
  KINLINE TObject* T_2(const skStrategy* strat);
  KINLINE void     T_1_2(const skStrategy* strat,
                         TObject* &T_1, TObject* &T_2);

  // simplify coefficients
  KINLINE void Normalize();
  KINLINE void HeadNormalize();
};
