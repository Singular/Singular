/*  ===================================================================
    GLOBAL COMPILE OPTIONS FOR BIFAC
    =================================================================== */


#ifndef bifac_global__H
#define bifac_global__H

#ifdef HAVE_BIFAC

// Also compute the polynomial 'h' and check if the defining
// diffential equation is zero
const bool BIFAC_BASIS_OF_G_CHECK = true;

// Check if the result of the absolute and rational
// factorization of a univariate polynomial are identical.
const bool  BIFAC_CHECK_UNI_ABS = true;


// Probability (in percent) that Eg(x) is separable
const int EgSeparable  =  90;

// #define AUSGABE_LGS(text) ;
// #define AUSGABE_HQ(text) ;
// #define AUSGABE_ERR(text) ;
// #define AUSGABE_LOG(text) ;

// Should there be debugging output?
#ifdef NOSTREAMIO
#define AUSGABE_LGS(text)
#define AUSGABE_HQ(text)
#define AUSGABE_ERR(text)
#define AUSGABE_LOG(text)
#else
#define AUSGABE_LGS(text) (cout << text << endl);
#define AUSGABE_HQ(text) (cout << text << endl);
#define AUSGABE_ERR(text) (cerr << text << endl);
#define AUSGABE_LOG(text) (cout << text << endl);
#endif


/*  ===================================================================
    GLOBAL COMPILE OPTIONS FOR MULTIFAC
    =================================================================== */
const int PROB_p = 90;  // Probability in percent for acurate factorization
const bool HC_NOASSERT = false;

#endif
#endif
