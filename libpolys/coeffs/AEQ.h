#ifndef QPOLY
#define QPOLY


#include <gmp.h>
#include <omalloc/omalloc.h>


class Q_poly // Klasse von Q_polynomen mit Typ (Grad, Koeffizienten ganzzahlig)
{

public:
    // Charakteristika der Q_polynome (TO DO??: Dynamisches Array)

    int deg;                // Grad des Q_polynoms
    mpz_t denom;        // Hauptnenner der Koeffizienten
    mpz_t coef[100];        // Feld der Koeffizienten
    //mpz_t coef = reinterpret_cast<mpz_t*> (omAlloc(100*sizeof(mpz_t)));


    // Konstruktoren und Destruktoren
    Q_poly();
    Q_poly( int ,mpz_t, mpz_t*);
    //Q_poly(int_poly,int);
    //~Q_poly();

    //Reduktion modulo p
    void Q_poly_reduce();
    void Q_poly_extend(mpz_t);

    // Arithmetische Operationen


    // Additionen

    void Q_poly_add(const Q_poly , const Q_poly );
    void Q_poly_add_to(const Q_poly);
    void Q_poly_add_mon(const Q_poly,mpz_t, int); //addiert Monome zu Q_polynom
    void Q_poly_add_mon_to(mpz_t,int);
    void Q_poly_add_const( Q_poly, const mpz_t);
    void Q_poly_add_const_to(const mpz_t);

    // Subtraktion

    void Q_poly_sub(const Q_poly , const Q_poly );
    void Q_poly_sub_to(const Q_poly);
    void Q_poly_sub_mon(const Q_poly,mpz_t,int);
    void Q_poly_sub_mon_to(mpz_t,int);
    void Q_poly_sub_const( Q_poly, const mpz_t);
    void Q_poly_sub_const_to(const mpz_t);

    //Multiplikationen

    void Q_poly_mult_n(Q_poly,Q_poly);
    void Q_poly_mult_n_to(const Q_poly);
    void Q_poly_mult_ka(const Q_poly, const Q_poly);
    void Q_poly_scalar_mult(const mpz_t ,const Q_poly);
    void Q_poly_scalar_mult(const Q_poly, const mpz_t);
    void Q_poly_scalar_mult_to(const mpz_t);
    void Q_poly_neg();
    void Q_poly_mon_mult(const Q_poly, const int);
    void Q_poly_mon_mult_to(const int);

    //Divisionen
    void Q_poly_div(Q_poly&, Q_poly&,const Q_poly, const Q_poly);        // exakte Division
    void Q_poly_div_to(Q_poly&, Q_poly&,const Q_poly);                       // To Variante exakte Division
    void Q_poly_scalar_div(const Q_poly, const mpz_t); // Dividiert Polynom durch ganze Zahl
    void Q_poly_scalar_div_to(const mpz_t);
    void Q_poly_div_rem(const Q_poly, const Q_poly);        //Division mit Rest
    void Q_poly_div_rem_to(const Q_poly);
    void Q_poly_mon_div(const Q_poly, const int);        //Division durch Monom
    void Q_poly_mon_div_rem(const Q_poly, const int);

    //Kombinationen

    void Q_poly_multadd_to(const Q_poly, const Q_poly); //a=a*b+c
    void Q_poly_multsub_to(const Q_poly,const Q_poly);  //a=a*b-c
    //Q_poly Q_poly_addmult_to(const Q_poly, const Q_poly);




    // Sonstige Operationen
    void Q_poly_set(const Q_poly);
    void Q_poly_set(const mpz_t);                        // Setzt Q_polynom auf Konstante aus Z
    void Q_poly_set(const mpz_t, const mpz_t);        // Setzt Q_polynom auf Konstante aus Q
    void Q_poly_set_zero();                                // Setzt Q_polynom auf 0
    void Q_poly_horner(mpz_t, const mpz_t);                // Wertet Q_polynom mittels Horner-Schema an einer Stelle aus
    void Q_poly_horner_Q_poly(Q_poly, const Q_poly);        //Setzt Q_polynom in Q_polynom mittels Horner Schema ein
    void Q_poly_gcd(Q_poly,Q_poly);                //Standard GGT
    void Q_poly_extgcd(Q_poly &,Q_poly &,Q_poly &, Q_poly, Q_poly);        //Erweiterter Standard GGT
    int is_equal(Q_poly &);                        // Test auf Gleichheit
    int is_zero() const;                                // Test auf Gleichheit mit 0
    int is_one() const;                                // Test auf Gleichheit mit 1
    int is_monic() const;                                // testet, ob das Q_polynom normiert ist

    // Ein und Ausgabe
    void Q_poly_insert();                        // Eingabe von Q_polynom
    void Q_poly_print();                        //Ausgabe von Q_polynom


};

#endif


