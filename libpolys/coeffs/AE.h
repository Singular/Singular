#ifndef AE_H
#define AE_H

#include <misc/auxiliary.h>
#include "si_gmp.h"

#ifdef SINGULAR_4_1

class int_poly // Klasse von int_polynomen mit Typ (Grad, Koeffizienten ganzzahlig)
{

public:
    // Charakteristika der int_polynome (TO DO??: Dynamisches Array)

    int deg;                // Grad des int_polynoms
    mpz_t coef[100];        // Feld der Koeffizienten
    //mpz_t coef = reinterpret_cast<mpz_t*> (omAlloc(100*sizeof(mpz_t)));


    // Konstruktoren und Destruktoren
    int_poly();
    int_poly( int , mpz_t*);
    //~int_poly();



    // Arithmetische Operationen


    // Additionen

    void poly_add(const int_poly , const int_poly );
    void poly_add_to(const int_poly);
    void poly_add_mon(const int_poly,mpz_t, int); //addiert Monome zu int_polynom
    void poly_add_mon_to(mpz_t,int);
    void poly_add_const( int_poly, const mpz_t);
    void poly_add_const_to(const mpz_t);

    // Subtraktion

    void poly_sub(const int_poly , const int_poly );
    void poly_sub_to(const int_poly);
    void poly_sub_mon(const int_poly,mpz_t,int);
    void poly_sub_mon_to(mpz_t,int);
    void poly_sub_const( int_poly, const mpz_t);
    void poly_sub_const_to(const mpz_t);

    //Multiplikationen

    void poly_mult_n(int_poly,int_poly);
    void poly_mult_n_to(const int_poly);
    void poly_mult_ka( int_poly,  int_poly);
    void poly_scalar_mult(const mpz_t ,const int_poly);
    void poly_scalar_mult(const int_poly, const mpz_t);
    void poly_scalar_mult_to(const mpz_t);
    void poly_neg();
    void poly_mon_mult(const int_poly, const int);
    void poly_mon_mult_to(const int);

    //Divisionen
    void poly_div(int_poly &,int_poly &, int_poly,  int_poly); // exakte Division
    void poly_div_to(int_poly &,int_poly &,const int_poly);       // To Variante exakte Division
    void poly_pseudodiv(int_poly &, int_poly &, int_poly ,  int_poly );
    void poly_pseudodiv_to(int_poly &, int_poly &, int_poly );
    void poly_pseudodiv_rem( int_poly , int_poly);
    void poly_pseudodiv_rem_to(const int_poly);
    void poly_scalar_div(const int_poly, const mpz_t);
    void poly_scalar_div_to(const mpz_t);
    void poly_mon_div(const int_poly, const int);
    void poly_mon_div_rem(const int_poly, const int);

    //Kombinationen

    void poly_multadd_to(const int_poly, const int_poly); //a=a*b+c
    void poly_multsub_to(const int_poly,const int_poly);  //a=a*b-c
    //int_poly poly_addmult_to(const int_poly, const int_poly);

    // Eigenschaften von int_polynomen

    // Content & Primitive Part
    void poly_cont(mpz_t&);
    void poly_pp(int_poly);


    // Sonstige Operationen
    void poly_set(const int_poly);
    void poly_set(const mpz_t);                // Setzt int_polynom auf Konstante
    void poly_set_zero();                        // Setzt int_polynom auf 0
    void poly_horner(mpz_t, const mpz_t);        // Wertet int_polynom mittels Horner-Schema an einer Stelle aus
    void poly_horner_int_poly(int_poly, const int_poly);        //Setzt int_polynom in int_polynom mittels Horner Schema ein
    void poly_gcd(int_poly,int_poly);                //Standard GGT
    void poly_extgcd(int_poly,int_poly,int_poly,int_poly);        //Erweiterter Standard GGT
    void poly_ppgcd( int_poly, int_poly);                // Primitive int_polynomial GCD
    void poly_ppgcd_to(int_poly);
    void poly_subgcd( int_poly, int_poly);                // Subresulatant GCD
    void poly_subgcd_to(int_poly);
    void poly_extsubgcd(int_poly&, int_poly&,int_poly &,int_poly,int_poly);
    int is_equal(const int_poly) const;                // Test auf Gleichheit
    int is_zero() const;                                // Test auf Gleichheit mit 0
    int is_one() const;                                // Test auf Gleichheit mit 1
    int is_monic() const;                                // testet, ob das int_polynom normiert ist


    // Ein und Ausgabe
    void poly_insert();                        // Eingabe von int_polynom
    void poly_print();                        //Ausgabe von int_polynom
};

#endif
#endif


