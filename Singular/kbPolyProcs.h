#ifndef KBPOLYPROCS_H
#define KBPOLYPROCS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kbPolyProcs.h,v 1.6 2000-08-14 12:56:29 obachman Exp $ */
#include "ring.h"
#include "kutil.h"


///////////////////////////////////////////////////////////////////////
///
/// Procedures for spolys and poly reductions
///
typedef void (*kb_n_Mult_p_Proc)(number n, poly p);
extern void kb_n_Mult_p_General(number n, poly p);

typedef void (*kb_p_Add_q_Proc)(poly *p, int *lp,
                                poly* q, int* lq,
                                omBin heap = NULL);
extern void kb_p_Add_q_General(poly *p, int *lp,
                               poly *q, int *lq,
                               omBin heap = NULL);

typedef poly (*kb_p_Mult_m_Proc)(poly p,
                                 poly m,
                                 poly spNoether,
                                 omBin heap = NULL);
extern poly  kb_p_Mult_m_General(poly p,
                                 poly m,
                                 poly spNoether,
                                 omBin heap = NULL);

typedef void (*kb_p_Minus_m_Mult_q_Proc)(poly *p, int *lp,
                                         poly m,
                                         poly q, int lq,
                                         poly spNoether,
                                         omBin heap = NULL);
extern void kb_p_Minus_m_Mult_q_General(poly *p, int *lp,
                                        poly m,
                                        poly q, int lq,
                                        poly spNoether,
                                        omBin heap = NULL);

typedef struct skbPolyProcs kbPolyProcs;
typedef kbPolyProcs * kbPolyProcs_pt;
struct skbPolyProcs
{
  kb_n_Mult_p_Proc          n_Mult_p;
  kb_p_Add_q_Proc           p_Add_q;
  kb_p_Mult_m_Proc          p_Mult_m;
  kb_p_Minus_m_Mult_q_Proc  p_Minus_m_Mult_q;
};

extern void kbSetPolyProcs(kbPolyProcs_pt procs,
                           // should be rGetOrderType(r), unless you
                           // know better (e.g., for syz or schreyer
                           // ordering, or if you know that components
                           // are not involved)
                           ring r,
                           rOrderType_t ot = rOrderType_General,
                           BOOLEAN homog = FALSE);


#define kb_n_Mult_p kb_n_Mult_p_General
#define kb_p_Add_q  kb_p_Add_q_General
#define kb_p_Minus_m_Mult_q  kb_p_Minus_m_Mult_q_General
#define kb_p_Mult_m kb_p_Mult_m_General

///////////////////////////////////////////////////////////////////////
///
/// General Procedures for poly arithemetic which always work
///
void kb_n_Mult_p_General(number n, poly p);
void kb_p_Add_q_General(poly *p, int *lp,
                        poly *q, int *lq,
                        omBin heap = NULL);
poly  kb_p_Mult_m_General(poly p,
                          poly m,
                          poly spNoether = NULL,
                          omBin heap = NULL);
void kb_p_Minus_m_Mult_q_General (poly *pp, int *lpp,
                                  poly m,
                                  poly q, int lq,
                                  poly spNoether = NULL,
                                  omBin heap = NULL);


void kbCreateSpoly(LObject* P,
                   poly spNoether = NULL);


void kbReducePoly(LObject* RP, // Poly to be reduced
                  TObject* WP, // Poly with which is reduced
                  poly spNoether = NULL);


#endif /* KBPOLYPROCS_H */
