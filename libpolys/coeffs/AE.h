#ifndef AE_H
#define AE_H


#include <gmp.h>
#include <omalloc/omalloc.h>


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

    int_poly poly_add(const int_poly , const int_poly );
    int_poly poly_add_to(const int_poly);
    int_poly poly_add_mon(const int_poly,mpz_t, int); //addiert Monome zu int_polynom
    int_poly poly_add_mon_to(mpz_t,int);
    int_poly poly_add_const( int_poly, const mpz_t);
    int_poly poly_add_const_to(const mpz_t);

    // Subtraktion

    int_poly poly_sub(const int_poly , const int_poly );
    int_poly poly_sub_to(const int_poly);
    int_poly poly_sub_mon(const int_poly,mpz_t,int);
    int_poly poly_sub_mon_to(mpz_t,int);
    int_poly poly_sub_const( int_poly, const mpz_t);
    int_poly poly_sub_const_to(const mpz_t);

    //Multiplikationen

    int_poly poly_mult_n(int_poly,int_poly);
    int_poly poly_mult_n_to(const int_poly);
    int_poly poly_mult_ka( int_poly,  int_poly);
    int_poly poly_scalar_mult(const mpz_t ,const int_poly);
    int_poly poly_scalar_mult(const int_poly, const mpz_t);
    int_poly poly_scalar_mult_to(const mpz_t);
    int_poly poly_neg();
    int_poly poly_mon_mult(const int_poly, const int);
    int_poly poly_mon_mult_to(const int);

    //Divisionen
    int_poly poly_div(int_poly &,int_poly &, int_poly,  int_poly); // exakte Division
    int_poly poly_div_to(int_poly &,int_poly &,const int_poly);       // To Variante exakte Division
    int_poly poly_pseudodiv(int_poly &, int_poly &, int_poly ,  int_poly );
    int_poly poly_pseudodiv_to(int_poly &, int_poly &, int_poly );
    int_poly poly_pseudodiv_rem( int_poly , int_poly);
    int_poly poly_pseudodiv_rem_to(const int_poly);
    int_poly poly_scalar_div(const int_poly, const mpz_t);
    int_poly poly_scalar_div_to(const mpz_t);
    int_poly poly_mon_div(const int_poly, const int);
    int_poly poly_mon_div_rem(const int_poly, const int);

    //Kombinationen

    int_poly poly_multadd_to(const int_poly, const int_poly); //a=a*b+c
    int_poly poly_multsub_to(const int_poly,const int_poly);  //a=a*b-c
    //int_poly poly_addmult_to(const int_poly, const int_poly);

    // Eigenschaften von int_polynomen

    // Content & Primitive Part
    void poly_cont(mpz_t&);
    int_poly poly_pp(int_poly);


    // Sonstige Operationen
    int_poly poly_set(const int_poly);
    int_poly poly_set(const mpz_t);                // Setzt int_polynom auf Konstante
    int_poly poly_set_zero();                        // Setzt int_polynom auf 0
    void poly_horner(mpz_t, const mpz_t);        // Wertet int_polynom mittels Horner-Schema an einer Stelle aus
    void poly_horner_int_poly(int_poly, const int_poly);        //Setzt int_polynom in int_polynom mittels Horner Schema ein
    int_poly poly_gcd(int_poly,int_poly);                //Standard GGT
    int_poly poly_extgcd(int_poly,int_poly,int_poly,int_poly);        //Erweiterter Standard GGT
    int_poly poly_ppgcd( int_poly, int_poly);                // Primitive int_polynomial GCD
    int_poly poly_ppgcd_to(int_poly);
    int_poly poly_subgcd( int_poly, int_poly);                // Subresulatant GCD
    int_poly poly_subgcd_to(int_poly);
    int_poly poly_extsubgcd(int_poly&, int_poly&,int_poly &,int_poly,int_poly);
    int is_equal(const int_poly);                // Test auf Gleichheit
    int is_zero();                                // Test auf Gleichheit mit 0
    int is_one();                                // Test auf Gleichheit mit 1
    int is_monic();                                // testet, ob das int_polynom normiert ist


    // Ein und Ausgabe
    void poly_insert();                        // Eingabe von int_polynom
    void poly_print();                        //Ausgabe von int_polynom
};

#endif


