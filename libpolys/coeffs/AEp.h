#ifndef AEP_H
#define AEP_H

#include <misc/auxiliary.h>
#include "si_gmp.h"

#ifdef SINGULAR_4_1

class p_poly // Klasse von p_polynomen mit Typ (Grad, Koeffizienten ganzzahlig)
{

public:
    // Charakteristika der p_polynome (TO DO??: Dynamisches Array)

    int deg;                // Grad des p_polynoms
    int mod;                // Primzahl (Modul??)
    mpz_t coef[100];        // Feld der Koeffizienten
    //mpz_t coef = reinterpret_cast<mpz_t*> (omAlloc(100*sizeof(mpz_t)));


    // Konstruktoren und Destruktoren
    p_poly();
    p_poly( int ,int, mpz_t*);
    //p_poly(int_poly,int);
    //~p_poly();

    //Reduktion modulo p
    void p_poly_reduce(p_poly, int);

    // Arithmetische Operationen


    // Additionen

    void p_poly_add(const p_poly , const p_poly );
    void p_poly_add_to(const p_poly);
    void p_poly_add_mon(const p_poly,mpz_t, int); //addiert Monome zu p_polynom
    void p_poly_add_mon_to(mpz_t,int);
    void p_poly_add_const( p_poly, const mpz_t);
    void p_poly_add_const_to(const mpz_t);

    // Subtraktion

    void p_poly_sub(const p_poly , const p_poly );
    void p_poly_sub_to(const p_poly);
    void p_poly_sub_mon(const p_poly,mpz_t,int);
    void p_poly_sub_mon_to(mpz_t,int);
    void p_poly_sub_const( p_poly, const mpz_t);
    void p_poly_sub_const_to(const mpz_t);

    //Multiplikationen

    void p_poly_mult_n(p_poly,p_poly);
    void p_poly_mult_n_to(const p_poly);
    void p_poly_mult_ka( p_poly, p_poly);
    void p_poly_scalar_mult(const mpz_t ,const p_poly);
    void p_poly_scalar_mult(const p_poly, const mpz_t);
    void p_poly_scalar_mult_to(const mpz_t);
    void p_poly_neg();
    void p_poly_mon_mult( p_poly, const int);
    void p_poly_mon_mult_to(const int);

    //Divisionen
    void p_poly_div(p_poly&, p_poly&, p_poly,  p_poly); // exakte Division
    void p_poly_div_to(p_poly&, p_poly&, p_poly);       // To Variante exakte Division
    void p_poly_scalar_div(const p_poly, const mpz_t n); // Multipliziert konstante an Polynom
    void p_poly_scalar_div_to(const mpz_t n);
    void p_poly_div_rem( p_poly, p_poly);//Division mit Rest
    void p_poly_div_rem_to( p_poly);
    void p_poly_mon_div(const p_poly, const int); //Division durch MOnom
    void p_poly_mon_div_rem(const p_poly, const int);

    //Kombinationen

    void p_poly_multadd_to(const p_poly, const p_poly); //a=a*b+c
    void p_poly_multsub_to(const p_poly,const p_poly);  //a=a*b-c
    //p_poly p_poly_addmult_to(const p_poly, const p_poly);




    // Sonstige Operationen
    void p_poly_set(const p_poly);
    void p_poly_set(const mpz_t,int);                // Setzt p_polynom auf Konstante
    void p_poly_set_zero();                        // Setzt p_polynom auf 0
    void p_poly_horner(mpz_t, const mpz_t);        // Wertet p_polynom mittels Horner-Schema an einer Stelle aus
    void p_poly_horner_p_poly(p_poly, p_poly);        //Setzt p_polynom in p_polynom mittels Horner Schema ein
    void p_poly_gcd(p_poly,p_poly);                //Standard GGT
    void p_poly_extgcd(p_poly &,p_poly &,p_poly &, p_poly, p_poly);        //Erweiterter Standard GGT
    int is_equal(const p_poly) const;                // Test auf Gleichheit
    int is_zero() const;                                // Test auf Gleichheit mit 0
    int is_one() const;                                // Test auf Gleichheit mit 1
    int is_monic() const;                                // testet, ob das p_polynom normiert ist

    // Ein und Ausgabe
    void p_poly_insert();                        // Eingabe von p_polynom
    void p_poly_print();                        //Ausgabe von p_polynom
};

#endif
#endif


