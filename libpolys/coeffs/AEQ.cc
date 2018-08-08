#include "misc/auxiliary.h"

#ifdef SINGULAR_4_2
#include "AEQ.h"
#include <stdio.h>
#include <math.h>

using namespace std;

//Konstruktoren

Q_poly::Q_poly()
{
    deg=-1;
    mpz_init_set_ui(denom,1);
    mpz_init_set_ui(coef[0],0);
}



Q_poly::Q_poly(int n,mpz_t d, mpz_t *a)
{
    deg=n;

    mpz_init_set(denom,d);

    for ( int i=0;i<=n;i++)
    {
        mpz_init_set(coef[i], a[i]);
    }
}

/*
//Destruktor

Q_poly::~Q_poly()
{
        delete[] coef;
}

*/

// Kürzen  --  MACHT NOCH MIST!
void Q_poly::Q_poly_reduce()
{
    if (is_zero()==1)
    {
        mpz_init_set_ui(denom,1);
    }
    else
    {
        mpz_t d;
        mpz_init_set(d,denom);
        int i=0;
        while (mpz_cmp_ui(d,1)!=0 && i<=deg)
        {
            mpz_gcd(d,d,coef[i]);
            i++;
        }
        if (mpz_sgn(denom)==-1)
        {
            mpz_neg(d,d);
        }
        if (mpz_cmp_ui(d,1)!=0)
        {
            mpz_div(denom,denom,d);
            for (int j=0;j<=deg;j++)
            {
                mpz_div(coef[j],coef[j],d);
            }
        }
    }
    // Grad-Korrektur
    int j;
    j=deg;
    while(mpz_sgn(coef[j])==0 && j>=0)
    {deg--;j--;}
}

// Koeffizienten mit b erweitern
void Q_poly::Q_poly_extend(mpz_t b)
{
        mpz_mul(denom,denom,b);
        for (int i=0;i<=deg;i++)
        {
                mpz_mul(coef[i],coef[i],b);
        }
}


// Arithmetik


//Additionen

//Standard - Addition
void Q_poly::Q_poly_add(const Q_poly a, const Q_poly b)
{
    if (a.deg >= b.deg)
    {
        deg=a.deg;
        mpz_t atemp, btemp;
        mpz_init_set_ui(atemp,0);
        mpz_init_set_ui(btemp,0);

        for (int i=0;i<=b.deg;i++)
        {
                mpz_mul(atemp,a.coef[i],b.denom);
                mpz_mul(btemp,b.coef[i],a.denom);
                mpz_add(coef[i],atemp,btemp);
        }

        for ( int i=b.deg+1;i<=a.deg;i++)
        {
            mpz_mul(coef[i],a.coef[i],b.denom);
        }
        mpz_mul(denom,a.denom,b.denom);

        // Grad-Korrektur
        int i=deg;
        while(mpz_sgn(coef[i])==0 && i>=0)
        {deg--;i--;}
    }

    else {Q_poly_add(b,a);}

}

//Überschreibende Addition

void Q_poly::Q_poly_add_to(const Q_poly g)
{
    this->Q_poly_add(*this,g);
}

//Addition einer Konstanten
void Q_poly::Q_poly_add_const(Q_poly f, const mpz_t a)
{
    if (f.is_zero()==1)
    {
        Q_poly_set(a,f.denom);
    }
    else
    {
        Q_poly_set(f);
        mpz_t atemp;
        mpz_mul(atemp,a,f.denom);
        mpz_add(coef[0],coef[0],atemp);
        // Grad Korrektur
        if (deg==0 && mpz_sgn(coef[0])==0)
            Q_poly_set_zero();
    }
}


//To Variante Addition einer Konstanten

void Q_poly::Q_poly_add_const_to(const mpz_t a)
{
    this->Q_poly_add_const(*this,a);
}

//Monom Addition
void Q_poly::Q_poly_add_mon(const Q_poly f, mpz_t a, int i)
{
    Q_poly_set(f);
    if (i<=deg  && is_zero()==0)
    {
        mpz_t atemp;
        mpz_init_set_ui(atemp,0);
        mpz_mul(atemp,a,f.denom);
        mpz_add(coef[i],coef[i],atemp);

        // Grad Korrektur

        if (deg==i && mpz_sgn(coef[i])==0)
        {deg--;}
    }
    else if (is_zero()==1)
    {
        deg=i;
        for(int j=0;j<i;j++)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_init_set(coef[i],a);
        mpz_init_set_ui(denom,1);
    }
    else if(i>deg)
    {
        deg=i;
        for(int j=i-1;j>deg;j--)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_t atemp;
        mpz_mul(atemp,a,f.denom);
        mpz_init_set(coef[i],atemp);
    }
}

//To Variante Monomaddition
void Q_poly::Q_poly_add_mon_to(mpz_t a, int i)
{
    this->Q_poly_add_mon(*this,a,i);
}

//Subtraktionen

void Q_poly::Q_poly_sub(const Q_poly a, const Q_poly b)
{
    Q_poly temp;
    temp.Q_poly_set(b);
    temp.Q_poly_neg();
    Q_poly_add(a,temp);
}


//Überschreibende Subtraktion

void Q_poly::Q_poly_sub_to(const Q_poly b)
{
    this->Q_poly_sub(*this,b);
}

//Subtraktion einer Konstanten
void Q_poly::Q_poly_sub_const(Q_poly f,const mpz_t a)
{
    if (f.is_zero()==1)
    {
        Q_poly_set(a);
        Q_poly_neg();
    }
    else
    {
        Q_poly_set(f);
        mpz_t atemp;
        mpz_init_set_ui(atemp,1);
        mpz_mul(atemp,a,f.denom);
        mpz_sub(coef[0],coef[0],atemp);
    }
}


//To Variante Subtraktion einer Konstanten

void Q_poly::Q_poly_sub_const_to(const mpz_t a)
{
    this->Q_poly_sub_const(*this,a);
}


//Monom Subtraktion
void Q_poly::Q_poly_sub_mon(const Q_poly f , mpz_t a, int i)
{
    mpz_t temp;
    mpz_init_set_ui(temp,0);
    mpz_neg(temp,a);
    Q_poly_add_mon(f,temp,i);
}

//To Variante Monomsubtraktion
void Q_poly::Q_poly_sub_mon_to(mpz_t a, int i)
{
    this->Q_poly_sub_mon(*this,a,i);
}


//Multiplikationen

//Multiplikation mit Monom
void Q_poly::Q_poly_mon_mult(const Q_poly f, int n)
{
    deg=f.deg+n;
    mpz_init_set(denom,f.denom);
    for (int i=deg;i>=n;i--)
    {
        mpz_init_set(coef[i],f.coef[i-n]);
    }
    for (int i=n-1;i>=0;i--)
    {
        mpz_init_set_ui(coef[i],0);
    }
}

void Q_poly::Q_poly_mon_mult_to(const int n)
{
    this->Q_poly_mon_mult(*this,n);
}


//Multiplikation mit Skalar

void Q_poly::Q_poly_scalar_mult(const Q_poly g, const mpz_t n)
{
    deg=g.deg;
    mpz_init_set(denom,g.denom);

    mpz_t temp;
    mpz_init_set_ui(temp,0);
    for(int i=0;i<=deg;i++)
    {
        mpz_mul(temp,n,g.coef[i]);
        mpz_init_set(coef[i],temp);
    }
}



void Q_poly::Q_poly_scalar_mult(const mpz_t n, const Q_poly g)
{
    deg=g.deg;
    mpz_init_set(denom,g.denom);

    mpz_t temp;
    mpz_init_set_ui(temp,0);
    for(int i=0;i<=deg;i++)
    {
        mpz_mul(temp,n,g.coef[i]);
        mpz_init_set(coef[i],temp);
    }
}


void Q_poly::Q_poly_scalar_mult_to(const mpz_t n)
{
    this->Q_poly_scalar_mult(*this,n);
}



// Negation

void Q_poly::Q_poly_neg()
{
    mpz_neg(denom,denom);
}

// Naive Multiplikation
void Q_poly::Q_poly_mult_n(Q_poly a,Q_poly b)
{

    if (a.is_zero()==1 || b.is_zero()==1)
        Q_poly_set_zero();
    else
    {
        mpz_t temp;
        mpz_init_set_ui(temp,0);
        deg = a.deg + b.deg;

        // Kopien atemp und btemp von a bzw. b, mit Nullen aufgefüllt
        Q_poly atemp, btemp;
        atemp.Q_poly_set(a);
        btemp.Q_poly_set(b);
        for(int i=a.deg+1;i<=deg;i++)
        {
            mpz_init_set_ui(atemp.coef[i],0);
        }
        for(int i=b.deg+1;i<=deg;i++)
        {
            mpz_init_set_ui(btemp.coef[i],0);
        }
        atemp.deg = deg;
        btemp.deg = deg;

        // Multiplikationsalgorithmus
        for (int k=0; k<=deg; k++)
        {
            mpz_init_set_ui(coef[k],0);        // k-ter Koeffizient zunächst 0
            for (int i=0; i<=k; i++)        // dann schrittweise Summe der a[i]*b[k-i]/
            {
                mpz_mul(temp,atemp.coef[i],btemp.coef[k-i]);
                mpz_add(coef[k],coef[k],temp);
            }
        }
        mpz_mul(denom,a.denom,b.denom);
    }
}

//Überschreibende Multiplikation

void Q_poly::Q_poly_mult_n_to(const Q_poly g)
{
    this->Q_poly_mult_n(*this,g);
}

// Karatsuba-Multiplikation (Weimerskirch/Paar Alg. 1), ACHTUNG VORLÄUFIGE VERSION, macht noch Fehler beim Grad und ist unelegant !!!
void Q_poly::Q_poly_mult_ka(const Q_poly A, const Q_poly B)
{
    // Größeren Grad feststellen
    int n;
    if(A.deg>=B.deg){n=A.deg+1;}
    else{n=B.deg+1;}
    // n auf nächste 2er-Potenz setzen (VORLÄUFIG!)
    n = static_cast<int>(ceil(log(n)/log(2)));
    n = static_cast<int>(pow(2,n));

    if (n==1)
    {
        mpz_t AB;
        mpz_mul(AB,A.coef[0],B.coef[0]);
        Q_poly_set(AB,A.denom);
    }
    else
    {
        // Q_polynome A und B aufspalten
        Q_poly Au, Al, Bu, Bl;
        Au.Q_poly_mon_div(A,n/2);
        Al.Q_poly_mon_div_rem(A,n/2);
        Bu.Q_poly_mon_div(B,n/2);
        Bl.Q_poly_mon_div_rem(B,n/2);
        Q_poly Alu,Blu;
        Alu.Q_poly_add(Al,Au);
        Blu.Q_poly_add(Bl,Bu);

        // Teile rekursiv multiplizieren
        Q_poly D0, D1, D01;
        D0.Q_poly_mult_ka(Al,Bl);
        D1.Q_poly_mult_ka(Au,Bu);
        D01.Q_poly_mult_ka(Alu,Blu);

        // Ergebnis zusammensetzen
        Q_poly temp;
        D01.Q_poly_sub_to(D0);
        D01.Q_poly_sub_to(D1);
        D01.Q_poly_mon_mult_to(n/2);
        D1.Q_poly_mon_mult_to(n);
        D1.Q_poly_add_to(D01);
        D1.Q_poly_add_to(D0);
        Q_poly_set(D1);
    }
}



//Skalare Divisionen

void Q_poly::Q_poly_scalar_div(const Q_poly g, const mpz_t n)
{
    if (mpz_sgn(n)!=0) // überprüft Teilung durch 0
    {
        Q_poly_set(g);
        mpz_mul(denom,g.denom,n);
    }
}


void Q_poly::Q_poly_scalar_div_to(const mpz_t n)
{
    this->Q_poly_scalar_div(*this,n);
}

// Division durch Monom - Quotient
void Q_poly::Q_poly_mon_div(const Q_poly f, const int n)
{
    if (f.deg<n)
    {
        Q_poly_set_zero();
    }
    else
    {
        deg=f.deg-n;
        mpz_init_set(denom,f.denom);

        for (int i=0;i<=f.deg-n;i++)
        {
            mpz_init_set(coef[i],f.coef[n+i]);
        }
    }
}

// Division durch Monom - Rest
void Q_poly::Q_poly_mon_div_rem(const Q_poly f, const int n)
{
    if (f.deg<n)
    {
        Q_poly_set(f);
    }
    else
    {
        // Grad-Korrektur ist inklusive
        deg=n-1;
        int j=deg;
        while(mpz_sgn(f.coef[j])==0 && j>=0)
        {
            deg--;
            j--;
            mpz_init_set_ui(coef[j],0);
        }
        for (int i=j;i>=0;i--)
        {
            mpz_init_set(coef[i],f.coef[i]);
        }
        mpz_init_set(denom,f.denom);
    }
}




// Euklidische Division nach Cohen Algo 3.1.1 (degA muss größer gleich deg B sein)!!

void Q_poly::Q_poly_div_rem(const Q_poly A, const Q_poly B)
{

    // Initialisierungen: Vergiss zunächst die Hauptnenner von A und B (--> R bzw. Bint)
    Q_poly temp, Bint;
    Q_poly_set(A);
    mpz_init_set_ui(denom,1);
    Bint.Q_poly_set(B);
    mpz_init_set_ui(Bint.denom,1);
    int e = A.deg - B.deg + 1;

    // Algorithmus
    while (deg>=B.deg)
    {
        temp.Q_poly_mon_mult(Bint,deg-B.deg);
        temp.Q_poly_scalar_mult_to(coef[deg]);

        Q_poly_scalar_mult_to(B.coef[B.deg]);
        Q_poly_sub_to(temp);

        e--;
    }

    // Terminierung
    mpz_t d,q;
    mpz_init_set(d,B.coef[B.deg]);
    if (e>0)
    {
        mpz_pow_ui(q,d,e);
        Q_poly_scalar_mult_to(q);
    }
    else if (e<0)
    {
        mpz_pow_ui(q,d,-e);
        Q_poly_scalar_div_to(q);
    }

    mpz_pow_ui(d,d,A.deg-B.deg+1);
    mpz_mul(denom,denom,d);

    // Hauptnenner von A und B berücksichtigen
    mpz_mul(denom,denom,A.denom);
    Q_poly_scalar_mult_to(B.denom);
}


//To Variante von Algo 3.1.1 im Cohen

void Q_poly::Q_poly_div_rem_to(const Q_poly B)
{
    this->Q_poly_div_rem(*this,B);
}


// Division nach Cohen 3.1.2 (gibt R und Q aus) --> Führt Pseudo-Division durch, korrigiert den Faktor aber im Nenner
void Q_poly::Q_poly_div(Q_poly &Q, Q_poly &R, const Q_poly A, const Q_poly B)
{

    // Initialisierungen: Vergiss zunächst die Hauptnenner von A und B (--> R bzw. Bint)
    Q_poly temp, Bint;
    R.Q_poly_set(A);
    mpz_init_set_ui(R.denom,1);
    Q.Q_poly_set_zero();
    Bint.Q_poly_set(B);
    mpz_init_set_ui(Bint.denom,1);
    int e = A.deg - B.deg + 1;

    // Algorithmus
    while (R.deg>=B.deg)
    {
        temp.Q_poly_mon_mult(Bint,R.deg-B.deg);
        temp.Q_poly_scalar_mult_to(R.coef[R.deg]);

        Q.Q_poly_scalar_mult_to(B.coef[B.deg]);
        Q.Q_poly_add_mon_to(R.coef[R.deg],R.deg-B.deg);

        R.Q_poly_scalar_mult_to(B.coef[B.deg]);
        R.Q_poly_sub_to(temp);

        e--;
    }

    // Terminierung
    mpz_t d,q;
    mpz_init_set(d,B.coef[B.deg]);
    if (e>0)
    {
        mpz_pow_ui(q,d,e);
        R.Q_poly_scalar_mult_to(q);
        Q.Q_poly_scalar_mult_to(q);
    }
    else if (e<0)
    {
        mpz_pow_ui(q,d,-e);
        R.Q_poly_scalar_div_to(q);
        Q.Q_poly_scalar_div_to(q);
    }

    mpz_pow_ui(d,d,A.deg-B.deg+1);
    mpz_mul(R.denom,R.denom,d);
    mpz_mul(Q.denom,Q.denom,d);

    // Hauptnenner von A und B berücksichtigen
    mpz_mul(R.denom,R.denom,A.denom);
    mpz_mul(Q.denom,Q.denom,A.denom);
    R.Q_poly_scalar_mult_to(B.denom);
    Q.Q_poly_scalar_mult_to(B.denom);
}


//To Variante der exakten Division

void Q_poly::Q_poly_div_to(Q_poly &Q,Q_poly &R,const Q_poly B)
{
    this->Q_poly_div(Q,R,*this,B);
}


// Kombinationen

// a := a*b + c
void Q_poly::Q_poly_multadd_to(const Q_poly b, const Q_poly c)
{
    Q_poly_mult_n_to(b);
    Q_poly_add_to(c);
}

//a=a*b-c
void Q_poly::Q_poly_multsub_to(const Q_poly b, const Q_poly c)
{
    Q_poly_mult_n_to(b);
    Q_poly_sub_to(c);
}



/*
// a := (a+b)* c
void Q_poly::poly_addmult_to(const Q_poly b, const Q_poly c)
{
        Q_poly a(deg,coef);
        a.poly_add_to(b);
        a.poly_mult_n_to(c);
        poly_set(a);
}
*/



//Sonstiges
void Q_poly::Q_poly_horner(mpz_t erg, const mpz_t u)
{
    mpz_init_set(erg,coef[deg]);
    for (int i=deg;i>=1;i--)
    {
        mpz_mul(erg,erg,u);
        mpz_add(erg,erg,coef[i-1]);
    }

// erg noch durch denom dividieren

}

// Q_polynom in Q_polynom einsetzen(Horner-Schema) KRITISCHE EINGABE x^2, x^2  ....

void Q_poly::Q_poly_horner_Q_poly(const Q_poly A,const Q_poly B)
{
    Q_poly_set(A.coef[A.deg],A.denom);
    for (int i=A.deg;i>=1;i--)
    {
        Q_poly_mult_n_to(B);
        Q_poly_add_const_to(A.coef[i-1]);
    }

  // Nenner anpassen

}



//Hilfsfunktionen


//setze Q_polynom auf Q_polynom b
void Q_poly::Q_poly_set(const Q_poly b)
{
    deg=b.deg;
    mpz_init_set(denom,b.denom);

    for(int i=0;i<=deg;i++)
    {
        mpz_init_set(coef[i],b.coef[i]); // Hier wird init set dringendst benötigt
    }
}


// setze Q_polynom auf konstantes Q_polynom b/d
void Q_poly::Q_poly_set(const mpz_t b, const mpz_t d)
{
    deg=0;
    mpz_init_set(denom,d);
    mpz_init_set(coef[0],b);
}

// setze Q_polynom auf konstantes Z_polynom b
void Q_poly::Q_poly_set(const mpz_t b)
{
    deg=0;
    mpz_init_set_ui(denom,1);
    mpz_init_set(coef[0],b);
}


//setze Q_polynom auf Nullpolynom
void Q_poly::Q_poly_set_zero()
{
    deg = -1;
}


// Vergleiche ob zwei Q_polynome gleich --> return 1 falls ja sont 0

int Q_poly::is_equal(Q_poly &g)
{
    if (deg!=g.deg)
    {
      return 0;
    }
    else
    {
      g.Q_poly_reduce();
      Q_poly_reduce();
      for (int i=deg;i>=0; i--)
      {
        if (mpz_cmp(coef[i],g.coef[i])!=0)
          {return 0;}
      }
      return 1;
    }
}

//Überprüft ob das Q_polynom 0 ist
int Q_poly::is_zero() const
{
    if (deg<0)
        return 1;
    else
        return 0;

}


//Überprüft ob das Q_polynom 1 ist
int Q_poly::is_one() const
{
    if (deg==0)
    {
        if (mpz_cmp(coef[0],denom)==0) { return 1; }
        else { return 0; }
    }
    else { return 0; }
}

int Q_poly::is_monic() const
{
    if (mpz_cmp(coef[deg],denom)==0)
        return 1;
    else
        return 0;
}

// klassischer GGT nach Cohen 3.2.1

void Q_poly::Q_poly_gcd(Q_poly A, Q_poly B)
{

    if (A.deg<B.deg)
        Q_poly_gcd(B,A);
    else if (B.is_zero()==1)
        Q_poly_set(A);
    else
    {
        Q_poly App;
        Q_poly Bpp;
        Q_poly R;
        App.Q_poly_set(A);
        Bpp.Q_poly_set(B);
        mpz_init_set_ui(App.denom,1);
        mpz_init_set_ui(Bpp.denom,1);

        while (Bpp.is_zero()==0)
        {
            R.Q_poly_div_rem(App,Bpp);
            App.Q_poly_set(Bpp);
            Bpp.Q_poly_set(R);
        }
        mpz_init_set(App.denom,App.coef[App.deg]);
        Q_poly_set(App);
    }
}


// Nach nach Fieker 2.12 Symbolisches Rechnen (2012) MACHT PROBLEME
// gibt g=s*A+t*B aus
void Q_poly::Q_poly_extgcd(Q_poly &s,Q_poly &t,Q_poly &g, Q_poly A, Q_poly B)
{
    if (A.deg<B.deg)
        Q_poly_extgcd(t,s,g,B,A);
    else if (B.is_zero()==1)
    {
        g.Q_poly_set(A);
        t.Q_poly_set_zero();

        mpz_t temp;
        mpz_init_set_ui(temp,1);
        s.Q_poly_set(temp,A.denom);
    }

    else
    {
        mpz_t temp;
        mpz_init_set_ui(temp,1);

        Q_poly R1;
        R1.Q_poly_set(A);
        Q_poly R2;
        R2.Q_poly_set(B);
        Q_poly R3;

        Q_poly S1;
        S1.Q_poly_set(temp,A.denom);
        Q_poly S2;
        S2.Q_poly_set_zero();
        Q_poly S3;

        Q_poly T1;
        T1.Q_poly_set_zero();
        Q_poly T2;
        T2.Q_poly_set(temp,A.denom);
        Q_poly T3;

        Q_poly Q;

        while (R2.is_zero()!=1)
        {
            Q_poly_div(Q,R3,R1,R2);

            S3.Q_poly_mult_n(Q,S2);
            S3.Q_poly_neg();
            S3.Q_poly_add_to(S1);

            T3.Q_poly_mult_n(Q,T2);
            T3.Q_poly_neg();
            T3.Q_poly_add_to(T1);

            R1.Q_poly_set(R2);
            R2.Q_poly_set(R3);

            S1.Q_poly_set(S2);
            S2.Q_poly_set(S3);

            T1.Q_poly_set(T2);
            T2.Q_poly_set(T3);
        }
        t.Q_poly_set(T1);
        s.Q_poly_set(S1);
        g.Q_poly_set(R1);
    }
}


//Ein & Ausgabe

//Eingabe

void Q_poly::Q_poly_insert()
{
#if 0
    cout << "Bitte geben Sie ein Q_polynom ein! Zunächst den Grad: " << endl;
    cin >> deg;
    mpz_init_set_ui(denom,1);
    cout << "Jetzt den Hauptnenner: " << endl;
    mpz_inp_str(denom,stdin, 10);

    for ( int i=0; i<=deg;i++)
    {
        mpz_init_set_ui(coef[i],0);
        printf("Geben Sie nun f[%i] ein:",i);
        mpz_inp_str(coef[i],stdin, 10);
    }
#endif
}


//Ausgabe
void Q_poly::Q_poly_print()
{
#if 0
    if (is_zero()==1)
        cout << "0" << "\n" <<endl;
    else
    {
        printf("(");
        for (int i=deg;i>=1;i--)
        {
            mpz_out_str(stdout,10,coef[i]);
            printf("X%i+",i);
        }
        mpz_out_str(stdout,10,coef[0]);
        printf(")/");
        mpz_out_str(stdout,10,denom);
        printf("\n");
    }
#endif
}

#endif
