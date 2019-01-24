/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include "cf_assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_binom.h"

#define MAXPT 40

#define INITPT 10

VAR CFArray * ptZ = 0;
VAR CFArray * ptF = 0;

VAR int charac = 0;
VAR int gfdeg = 0;
VAR int ptZmax = INITPT;
VAR int ptFmax = 0;

void
resetFPT()
{
    ptFmax = 0;
}

void
initPT ( )
{
    STATIC_VAR bool initialized = false;

    if ( ! initialized ) {
        initialized = true;
        ptZ = new CFArray[MAXPT+1];
        ptF = new CFArray[MAXPT+1];
        int i, j;
        ptZ[0] = CFArray(1); ptZ[0][0] = 1;
        ptF[0] = CFArray(1);
        for ( i = 1; i <= INITPT; i++ ) {
            ptF[i] = CFArray(i+1);
            ptZ[i] = CFArray(i+1);
            (ptZ[i])[0] = 1;
            for ( j = 1; j < i; j++ )
                (ptZ[i])[j] = (ptZ[i-1])[j-1] + (ptZ[i-1])[j];
            (ptZ[i])[i] = 1;
        }
        for ( i = INITPT+1; i <= MAXPT; i++ ) {
            ptF[i] = CFArray(i+1);
            ptZ[i] = CFArray(i+1);
        }
        ptZmax = INITPT;
        ptFmax = 0;
    }
}

CanonicalForm
binomialpower ( const Variable & x, const CanonicalForm & a, int n )
{
    if ( n == 0 )
        return 1;
    else if ( n == 1 )
        return x + a;
    else if ( getCharacteristic() == 0 ) {
        if ( n <= MAXPT ) {
            if ( n > ptZmax ) {
                int i, j;
                for ( i = ptZmax+1; i <= n; i++ ) {
                    (ptZ[i])[0] = 1;
                    for ( j = 1; j < i; j++ )
                        (ptZ[i])[j] = (ptZ[i-1])[j-1] + (ptZ[i-1])[j];
                    (ptZ[i])[i] = 1;
                }
                ptZmax = n;
            }
            CanonicalForm result = 0, apower = 1;
            int k;
            for ( k = n; k >= 0; k-- ) {
                result += power( x, k ) * apower * (ptZ[n])[k];
                if ( k != 0 )
                    apower *= a;
            }
            return result;
        }
        else {
            CanonicalForm result = binomialpower( x, a, MAXPT );
            CanonicalForm xa = x + a;
            int i;
            for ( i = MAXPT; i < n; i++ )
                result *= xa;
            return result;
        }
    }
    else {
        if ( getCharacteristic() != charac || gfdeg != getGFDegree() ) {
            ptFmax = 0;
            charac = getCharacteristic();
            gfdeg = getGFDegree();
            (ptF[0])[0] = 1;
        }
        if ( n <= MAXPT ) {
            if ( n > ptFmax ) {
                int i, j;
                for ( i = ptFmax+1; i <= n; i++ ) {
                    (ptF[i])[0] = 1;
                    for ( j = 1; j < i; j++ )
                        (ptF[i])[j] = (ptF[i-1])[j-1] + (ptF[i-1])[j];
                    (ptF[i])[i] = 1;
                }
                ptFmax = n;
            }
            CanonicalForm result = 0, apower = 1;
            int k;
            for ( k = n; k >= 0; k-- ) {
                result += power( x, k ) * apower * (ptF[n])[k];
                if ( k != 0 )
                    apower *= a;
            }
            return result;
        }
        else {
            CanonicalForm result = binomialpower( x, a, MAXPT );
            CanonicalForm xa = x + a;
            int i;
            for ( i = MAXPT; i < n; i++ )
                result *= xa;
            return result;
        }
    }
}
