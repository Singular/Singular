typedef enum Characteristics {chGEN = 0, chMODP} Characteristics;
typedef enum OrderingTypes {otGEN = 0, otEXP, otCOMPEXP, otEXPCOMP} OrderingTypes;
typedef enum Homogs {homGEN = 0, homYES} Homogs;
typedef enum NumWords {nwGEN = 0, nwONE, nwTWO, nwEVEN, nwODD} NumWords;
static void spSpolyLoop_chMODP_otCOMPEXP_homGEN_nwEVEN
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  ;              // inits order for homog case
  

  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetComp(a2) - pGetComp(b);
    NonZeroTestA(d, pComponentOrder, goto NotEqual);
    d = pGetOrder(b) - pGetOrder(a2);
    NonZeroTestA(d, pOrdSgn, goto NotEqual);
    _pMonComp_otEXP_nwEVEN(b, a2, pVariables1W, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otCOMPEXP_homGEN_nwODD
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  ;              // inits order for homog case
  

  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetComp(a2) - pGetComp(b);
    NonZeroTestA(d, pComponentOrder, goto NotEqual);
    d = pGetOrder(b) - pGetOrder(a2);
    NonZeroTestA(d, pOrdSgn, goto NotEqual);
    _pMonComp_otEXP_nwODD(b, a2, pVariables1W, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otCOMPEXP_homGEN_nwONE
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  ;              // inits order for homog case
  

  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetComp(a2) - pGetComp(b);
    NonZeroTestA(d, pComponentOrder, goto NotEqual);
    d = pGetOrder(b) - pGetOrder(a2);
    NonZeroTestA(d, pOrdSgn, goto NotEqual);
    _pMonComp_otEXP_nwONE(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otCOMPEXP_homGEN_nwTWO
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  ;              // inits order for homog case
  

  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetComp(a2) - pGetComp(b);
    NonZeroTestA(d, pComponentOrder, goto NotEqual);
    d = pGetOrder(b) - pGetOrder(a2);
    NonZeroTestA(d, pOrdSgn, goto NotEqual);
    _pMonComp_otEXP_nwTWO(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otCOMPEXP_homYES_nwEVEN
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  order = pGetOrder(a2);              // inits order for homog case
  

  pCopyAddFastHomog(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetComp(a2) - pGetComp(b);
    NonZeroTestA(d, pComponentOrder, goto NotEqual);
    _pMonComp_otEXP_nwEVEN(b, a2, pVariables1W, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFastHomog(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFastHomog(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otCOMPEXP_homYES_nwODD
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  order = pGetOrder(a2);              // inits order for homog case
  

  pCopyAddFastHomog(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetComp(a2) - pGetComp(b);
    NonZeroTestA(d, pComponentOrder, goto NotEqual);
    _pMonComp_otEXP_nwODD(b, a2, pVariables1W, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFastHomog(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFastHomog(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otCOMPEXP_homYES_nwONE
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  order = pGetOrder(a2);              // inits order for homog case
  

  pCopyAddFastHomog(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetComp(a2) - pGetComp(b);
    NonZeroTestA(d, pComponentOrder, goto NotEqual);
    _pMonComp_otEXP_nwONE(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFastHomog(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFastHomog(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otCOMPEXP_homYES_nwTWO
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  order = pGetOrder(a2);              // inits order for homog case
  

  pCopyAddFastHomog(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetComp(a2) - pGetComp(b);
    NonZeroTestA(d, pComponentOrder, goto NotEqual);
    _pMonComp_otEXP_nwTWO(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFastHomog(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFastHomog(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXPCOMP_homGEN_nwEVEN
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  ;              // inits order for homog case
  

  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetOrder(b) - pGetOrder(a2);
    NonZeroTestA(d, pOrdSgn, goto NotEqual);
    _pMonComp_otEXPCOMP_nwEVEN(b, a2, pVariables1W, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXPCOMP_homGEN_nwODD
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  ;              // inits order for homog case
  

  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetOrder(b) - pGetOrder(a2);
    NonZeroTestA(d, pOrdSgn, goto NotEqual);
    _pMonComp_otEXPCOMP_nwODD(b, a2, pVariables1W, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXPCOMP_homGEN_nwONE
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  ;              // inits order for homog case
  

  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetOrder(b) - pGetOrder(a2);
    NonZeroTestA(d, pOrdSgn, goto NotEqual);
    _pMonComp_otEXPCOMP_nwONE(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXPCOMP_homGEN_nwTWO
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  ;              // inits order for homog case
  

  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetOrder(b) - pGetOrder(a2);
    NonZeroTestA(d, pOrdSgn, goto NotEqual);
    _pMonComp_otEXPCOMP_nwTWO(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXPCOMP_homYES_nwEVEN
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  order = pGetOrder(a2);              // inits order for homog case
  

  pCopyAddFastHomog(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    _pMonComp_otEXPCOMP_nwEVEN(b, a2, pVariables1W, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFastHomog(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFastHomog(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXPCOMP_homYES_nwODD
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  order = pGetOrder(a2);              // inits order for homog case
  

  pCopyAddFastHomog(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    _pMonComp_otEXPCOMP_nwODD(b, a2, pVariables1W, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFastHomog(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFastHomog(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXPCOMP_homYES_nwONE
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  order = pGetOrder(a2);              // inits order for homog case
  

  pCopyAddFastHomog(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    _pMonComp_otEXPCOMP_nwONE(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFastHomog(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFastHomog(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXPCOMP_homYES_nwTWO
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  order = pGetOrder(a2);              // inits order for homog case
  

  pCopyAddFastHomog(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    _pMonComp_otEXPCOMP_nwTWO(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFastHomog(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFastHomog(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXP_homGEN_nwEVEN
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  ;              // inits order for homog case
  

  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetOrder(b) - pGetOrder(a2);
    NonZeroTestA(d, pOrdSgn, goto NotEqual);
    _pMonComp_otEXP_nwEVEN(b, a2, pVariables1W, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXP_homGEN_nwODD
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  ;              // inits order for homog case
  

  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetOrder(b) - pGetOrder(a2);
    NonZeroTestA(d, pOrdSgn, goto NotEqual);
    _pMonComp_otEXP_nwODD(b, a2, pVariables1W, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXP_homGEN_nwONE
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  ;              // inits order for homog case
  

  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetOrder(b) - pGetOrder(a2);
    NonZeroTestA(d, pOrdSgn, goto NotEqual);
    _pMonComp_otEXP_nwONE(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXP_homGEN_nwTWO
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  ;              // inits order for homog case
  

  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    d = pGetOrder(b) - pGetOrder(a2);
    NonZeroTestA(d, pOrdSgn, goto NotEqual);
    _pMonComp_otEXP_nwTWO(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXP_homYES_nwEVEN
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  order = pGetOrder(a2);              // inits order for homog case
  

  pCopyAddFastHomog(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    _pMonComp_otEXP_nwEVEN(b, a2, pVariables1W, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFastHomog(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFastHomog(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXP_homYES_nwODD
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  order = pGetOrder(a2);              // inits order for homog case
  

  pCopyAddFastHomog(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    _pMonComp_otEXP_nwODD(b, a2, pVariables1W, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFastHomog(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFastHomog(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXP_homYES_nwONE
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  order = pGetOrder(a2);              // inits order for homog case
  

  pCopyAddFastHomog(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    _pMonComp_otEXP_nwONE(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFastHomog(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFastHomog(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static void spSpolyLoop_chMODP_otEXP_homYES_nwTWO
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = npNegM(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  order = pGetOrder(a2);              // inits order for homog case
  

  pCopyAddFastHomog(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    _pMonComp_otEXP_nwTWO(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);;

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = npMultM(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!npEqualM(tc, tb))
    {
      tc=npSubM(tc, tb);
      ;
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      ;
      pFree1(c);
    }
    ;
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    pCopyAddFastHomog(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,npMultM(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      pCopyAddFastHomog(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spMultCopyX(a1, monom, a, tneg, spNoether);
   ;
   if (b != NULL) pFree1(b);
} 
static spSpolyLoopProc spGetSpolyLoop(Characteristics ch,OrderingTypes ot,Homogs hom,NumWords nw)
{
if (ch == chMODP)
{
if (ot == otEXP)
{
if (hom == homYES)
{
if (nw == nwTWO)
{
return spSpolyLoop_chMODP_otEXP_homYES_nwTWO;
}
if (nw == nwONE)
{
return spSpolyLoop_chMODP_otEXP_homYES_nwONE;
}
if (nw == nwODD)
{
return spSpolyLoop_chMODP_otEXP_homYES_nwODD;
}
if (nw == nwEVEN)
{
return spSpolyLoop_chMODP_otEXP_homYES_nwEVEN;
}
}
if (nw == nwTWO)
{
return spSpolyLoop_chMODP_otEXP_homGEN_nwTWO;
}
if (nw == nwONE)
{
return spSpolyLoop_chMODP_otEXP_homGEN_nwONE;
}
if (nw == nwODD)
{
return spSpolyLoop_chMODP_otEXP_homGEN_nwODD;
}
if (nw == nwEVEN)
{
return spSpolyLoop_chMODP_otEXP_homGEN_nwEVEN;
}
}
if (ot == otEXPCOMP)
{
if (hom == homYES)
{
if (nw == nwTWO)
{
return spSpolyLoop_chMODP_otEXPCOMP_homYES_nwTWO;
}
if (nw == nwONE)
{
return spSpolyLoop_chMODP_otEXPCOMP_homYES_nwONE;
}
if (nw == nwODD)
{
return spSpolyLoop_chMODP_otEXPCOMP_homYES_nwODD;
}
if (nw == nwEVEN)
{
return spSpolyLoop_chMODP_otEXPCOMP_homYES_nwEVEN;
}
}
if (nw == nwTWO)
{
return spSpolyLoop_chMODP_otEXPCOMP_homGEN_nwTWO;
}
if (nw == nwONE)
{
return spSpolyLoop_chMODP_otEXPCOMP_homGEN_nwONE;
}
if (nw == nwODD)
{
return spSpolyLoop_chMODP_otEXPCOMP_homGEN_nwODD;
}
if (nw == nwEVEN)
{
return spSpolyLoop_chMODP_otEXPCOMP_homGEN_nwEVEN;
}
}
if (ot == otCOMPEXP)
{
if (hom == homYES)
{
if (nw == nwTWO)
{
return spSpolyLoop_chMODP_otCOMPEXP_homYES_nwTWO;
}
if (nw == nwONE)
{
return spSpolyLoop_chMODP_otCOMPEXP_homYES_nwONE;
}
if (nw == nwODD)
{
return spSpolyLoop_chMODP_otCOMPEXP_homYES_nwODD;
}
if (nw == nwEVEN)
{
return spSpolyLoop_chMODP_otCOMPEXP_homYES_nwEVEN;
}
}
if (nw == nwTWO)
{
return spSpolyLoop_chMODP_otCOMPEXP_homGEN_nwTWO;
}
if (nw == nwONE)
{
return spSpolyLoop_chMODP_otCOMPEXP_homGEN_nwONE;
}
if (nw == nwODD)
{
return spSpolyLoop_chMODP_otCOMPEXP_homGEN_nwODD;
}
if (nw == nwEVEN)
{
return spSpolyLoop_chMODP_otCOMPEXP_homGEN_nwEVEN;
}
}
}
return NULL;
}
