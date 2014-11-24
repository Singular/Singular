#ifndef POLYS_NC_GB_HACK_H
#define POLYS_NC_GB_HACK_H

#ifdef HAVE_PLURAL

#ifdef PLURAL_INTERNAL_DECLARATIONS

struct  spolyrec; typedef struct spolyrec    polyrec; typedef polyrec *          poly;
struct ip_sring; typedef struct ip_sring *         ring;
struct sip_sideal; typedef struct sip_sideal *       ideal;

class intvec;

class skStrategy; typedef skStrategy * kStrategy;
typedef ideal (*GB_Proc_Ptr)(const ideal, const ideal, const intvec*, const intvec*, kStrategy, const ring);


ideal gnc_gr_bba(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing);
ideal gnc_gr_mora(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing);

/// Modified Plural's Buchberger's algorithmus.
ideal sca_gr_bba(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing);

/// Modified modern Sinuglar Buchberger's algorithm.
ideal sca_bba(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing);

/// Modified modern Sinuglar Mora's algorithm.
ideal sca_mora(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing);

poly kNF(ideal, ideal, poly, int, int, const ring _currRing);



# ifdef PLURAL_INTERNAL_DECLARATIONS_GB_HACK

extern void WerrorS(const char *);

# define STR_EXPAND(tok) #tok
# define D(A)    A{ WerrorS("This is a hack. Function is not defined: " STR_EXPAND(A) ); return NULL; (void)(_currRing); }

D(ideal gnc_gr_bba(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing))
D(ideal gnc_gr_mora(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing))

/// Modified Plural's Buchberger's algorithmus.
D(ideal sca_gr_bba(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing))

/// Modified modern Sinuglar Buchberger's algorithm.
D(ideal sca_bba(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing))

/// Modified modern Sinuglar Mora's algorithm.
D(ideal sca_mora(const ideal, const ideal, const intvec *, const intvec *, kStrategy, const ring _currRing))

D(poly kNF(ideal, ideal, poly, int, int, const ring _currRing))

#endif // # ifdef PLURAL_INTERNAL_DECLARATIONS_GB_HACK
#endif // PLURAL_INTERNAL_DECLARATIONS


#endif // HAVE_PLURAL
#endif // POLYS_NC_GB_HACK_H

