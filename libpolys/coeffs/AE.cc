#include "misc/auxiliary.h"


#ifdef SINGULAR_4_2
#include "AE.h"
#include <math.h>

using namespace std;

//Konstruktoren

int_poly::int_poly()
{
    //coef = reinterpret_cast<mpz_t*> (omAlloc(100*sizeof(mpz_t)));
    deg=-1;
    mpz_init_set_ui(coef[0],0);
}



int_poly::int_poly(int n, mpz_t *a)
{
    //coef = reinterpret_cast<mpz_t*> (omAlloc(100*sizeof(mpz_t)));
    deg=n;

    for ( int i=0;i<=n;i++)
    {
        mpz_init_set(coef[i], a[i]);
    }

}

/*
//Destruktor

int_poly::~int_poly()
{
        delete[] coef;
}

*/




// Arithmetik


//Additionen

//Standard - Addition
void int_poly::poly_add(const int_poly a, const int_poly b)
{
    if (a.deg >=b.deg)
    {

        deg=a.deg;

        for ( int i=0;i<=b.deg;i++)
        {
            mpz_add(coef[i],a.coef[i],b.coef[i]);
        }

        for ( int i=b.deg+1;i<=a.deg;i++)
        {
            mpz_init_set(coef[i],a.coef[i]);
        }
        //Hier nötige Grad Korrektur
        int i;
        i=deg;
        while(mpz_sgn(coef[i])==0 && i>=0)
        {deg--;i--;}
    }

    else {poly_add(b,a);  }

}

//Überschreibende Addition

void int_poly::poly_add_to(const int_poly g)
{
    this->poly_add(*this,g);
}

//Addition einer Konstanten
void int_poly::poly_add_const(int_poly f,const mpz_t a)
{
    if (f.is_zero()==1)
        poly_set(a);
    else
    {
        poly_set(f);
        mpz_add(coef[0],coef[0],a);
        // Grad Korrektur
        if (deg==0 && mpz_sgn(coef[0])==0)
            poly_set_zero();
    }

}


//To Variante Addition einer Konstanten

void int_poly::poly_add_const_to(const mpz_t a)
{
    this->poly_add_const(*this,a);
}

//Monom Addition
void int_poly::poly_add_mon(int_poly f, mpz_t a, int i)
{
    poly_set(f);

    if (i<=deg  && is_zero()==0)
    {
        mpz_add(coef[i],coef[i],a);
        // Grad Korrektur
        if (deg==i && mpz_sgn(coef[i])==0)
            deg--;
    }
    else if (is_zero()==1)
    {
        deg=i;
        for(int j=0;j<=i;j++)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_add(coef[i],coef[i],a);

    }
    else if (i>deg)
    {
        for(int j=i;j>deg;j--)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_add(coef[i],coef[i],a);
        deg=i;
    }
}

//To Variante Monomaddition
void int_poly::poly_add_mon_to(mpz_t a, int i)
{
    if (i<=deg  && is_zero()==0)
    {
        mpz_add(coef[i],coef[i],a);
    }
    else if (is_zero()==1)
    {
        deg=i;
        for(int j=0;j<=i;j++)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_add(coef[i],coef[i],a);

    }
    else if (i>deg)
    {
        for(int j=i;j>deg;j--)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_add(coef[i],coef[i],a);
        deg=i;
    }
    //Hier nötige Grad Korrektur
    int k=deg;
    while(mpz_sgn(coef[k])==0 && k>=0)
    {deg--;k--;}

}

//Subtraktionen

void int_poly::poly_sub(const int_poly a, const int_poly b)
{
    int_poly temp;
    temp.poly_set(b);
    temp.poly_neg();
    poly_add(a,temp);

    // Grad Korrektur
    int i;
    i=deg;
    while(mpz_sgn(coef[i])==0 && i>=0)
    {deg--;i--;}

}


//Überschreibende Subtraktion

void int_poly::poly_sub_to(const int_poly b)
{
    this->poly_sub(*this,b);
}

//Subtraktion einer Konstanten
void int_poly::poly_sub_const(int_poly f,const mpz_t a)
{
    if (f.is_zero()==1)
    {
        poly_set(a);
        poly_neg();
    }
    else
    {
        poly_set(f);
        mpz_sub(coef[0],coef[0],a);

    }
    //Hier nötige Grad Korrektur
    int i=deg;
    while(mpz_sgn(coef[i])==0 && i>=0)
    {deg--;i--;}

}


//To Variante Subtraktion einer Konstanten

void int_poly::poly_sub_const_to(const mpz_t a)
{
    this->poly_sub_const(*this,a);
}


//Monom Subtraktion
void int_poly::poly_sub_mon(const int_poly f , mpz_t a, int i)
{
    poly_set(f);

    if (i<=deg && is_zero()!=1)
    {
        mpz_sub(coef[i],coef[i],a);
        // Grad Korrektur
        if (deg==i && mpz_sgn(coef[i])==0)
            deg--;
    }
    else if (is_zero()==1)
    {
        for(int j=0;j<=i;j++)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_sub(coef[i],coef[i],a);
        deg=i;
    }
    else
    {
        for(int j=i;j>deg;j--)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_sub(coef[i],coef[i],a);
        deg=i;
    }
}

//To Variante Monomaddition
void int_poly::poly_sub_mon_to(mpz_t a, int i)
{

    if (i<=deg && is_zero()!=1)
    {
        mpz_sub(coef[i],coef[i],a);
        // Grad Korrektur
        if (deg==i && mpz_sgn(coef[i])==0)
            deg--;
    }
    else if (is_zero()==1)
    {
        for(int j=0;j<=i;j++)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_sub(coef[i],coef[i],a);
        deg=i;
    }
    else
    {
        for(int j=i;j>deg;j--)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_sub(coef[i],coef[i],a);
        deg=i;
    }
}


//Multiplikationen

//Multiplikation mit Monom
void int_poly::poly_mon_mult(const int_poly f, int n)
{
    if (f.is_zero()==1)
    {
        poly_set_zero();
    }
    else
    {
        deg=f.deg+n;
        for (int i=deg;i>=n;i--)
        {
            mpz_init_set(coef[i],f.coef[i-n]);
        }
        for (int i=n-1;i>=0;i--)
        {
            mpz_init_set_ui(coef[i],0);
        }
    }
}

void int_poly::poly_mon_mult_to(const int n)
{
    this->poly_mon_mult(*this,n);
}


//Multiplikation mit Skalar

void int_poly::poly_scalar_mult(const int_poly g, const mpz_t n)
{
    if (mpz_sgn(n)==0)
        poly_set_zero();
    else
    {
        deg=g.deg;
        mpz_t temp;
        mpz_init_set_ui(temp,0);
        for(int i=0;i<=deg;i++)
        {
            mpz_mul(temp,n,g.coef[i]);
            mpz_init_set(coef[i],temp);
        }
    }
}



void int_poly::poly_scalar_mult(const mpz_t n, const int_poly g)
{
    if (mpz_sgn(n)==0)
        poly_set_zero();
    else
    {
        deg=g.deg;
        mpz_t temp;
        mpz_init_set_ui(temp,0);
        for(int i=0;i<=deg;i++)
        {
            mpz_mul(temp,n,g.coef[i]);
            mpz_init_set(coef[i],temp);
        }
    }
}


void int_poly::poly_scalar_mult_to(const mpz_t n)
{
    this->poly_scalar_mult(*this,n);
}



// Negation

void int_poly::poly_neg()
{
    for (int i=0;i<=deg;i++)
    {
        mpz_neg(coef[i],coef[i]);
    }
}

// Naive Multiplikation
void int_poly::poly_mult_n(int_poly a,int_poly b)
{

    if (a.is_zero()==1 || b.is_zero()==1)
    {
        poly_set_zero();
    }
    else
    {
        mpz_t temp;
        mpz_init_set_ui(temp,0);
        deg = a.deg + b.deg;

        // Kopien atemp und btemp von a bzw. b, mit Nullen aufgefüllt
        int_poly atemp, btemp;
        atemp.poly_set(a);
        btemp.poly_set(b);
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

    }
}

//Überschreibende Multiplikation

void int_poly::poly_mult_n_to(const int_poly g)
{
    this->poly_mult_n(*this,g);

}

// Karatsuba-Multiplikation (Weimerskirch/Paar Alg. 1), ACHTUNG VORLÄUFIGE VERSION, macht noch Fehler beim Grad und ist unelegant !!!
void int_poly::poly_mult_ka( int_poly A,  int_poly B)
{

    if (A.is_zero()==1 || B.is_zero()==1)
    {
        poly_set_zero();
    }
    else
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
            poly_set(AB);
        }
        else
        {
            // int_polynome A und B aufspalten
            int_poly Au, Al, Bu, Bl;
            Au.poly_mon_div(A,n/2);
            Al.poly_mon_div_rem(A,n/2);
            Bu.poly_mon_div(B,n/2);
            Bl.poly_mon_div_rem(B,n/2);
            int_poly Alu,Blu;
            Alu.poly_add(Al,Au);
            Blu.poly_add(Bl,Bu);

            // Teile rekursiv multiplizieren
            int_poly D0, D1, D01;
            D0.poly_mult_ka(Al,Bl);
            D1.poly_mult_ka(Au,Bu);
            D01.poly_mult_ka(Alu,Blu);

            // Ergebnis zusammensetzen
            int_poly temp;
            D01.poly_sub_to(D0);
            D01.poly_sub_to(D1);
            D01.poly_mon_mult_to(n/2);
            D1.poly_mon_mult_to(n);
            D1.poly_add_to(D01);
            D1.poly_add_to(D0);
            poly_set(D1);
        }
    }
}



//Skalare Divisionen

void int_poly::poly_scalar_div( const int_poly g, const mpz_t n)
{
    deg=g.deg;
    mpz_t temp;
    mpz_init_set_ui(temp,0);
    for(int i=0;i<=deg;i++)
    {
        mpz_divexact(temp,g.coef[i],n);
        mpz_init_set(coef[i],temp);
    }
}


void int_poly::poly_scalar_div_to(const mpz_t n)
{
    this->poly_scalar_div(*this,n);
}

// Division durch Monom -  results in Quotient without remainder
void int_poly::poly_mon_div(const int_poly f, const int n)
{
    if (f.deg<n)
    {
        poly_set_zero();
    }
    else
    {
        deg=f.deg-n;
        for (int i=0;i<=f.deg-n;i++)
        {
            mpz_init_set(coef[i],f.coef[n+i]);
        }
    }
}

// Division durch Monom - Rest
void int_poly::poly_mon_div_rem(const int_poly f, const int n)
{
    if (f.deg<n)
    {
        poly_set(f);
    }
    else
    {
        deg=n-1;
        for (int i=0;i<=n-1;i++)
        {
            mpz_init_set(coef[i],f.coef[i]);
        }
    }
}




//Exakte Division nach Cohen 3.1.1 (works only if B!=0)
void int_poly::poly_div(int_poly &Q,int_poly &R, int_poly A,  int_poly B)
{
    if (B.is_zero()==0)
    {
        //Initialisierungen
        int_poly temp;
        R.poly_set(A);
        Q.poly_set_zero();
        mpz_t a;
        mpz_init_set_ui(a,0);
        int i;

        //Algorithmus TO DO: evtl hier mit auch den Rest ausgeben
        while (R.deg>=B.deg)
        {
            mpz_divexact(a,R.coef[R.deg],B.coef[B.deg]);
            i=R.deg-B.deg;

            temp.poly_mon_mult(B,i);
            temp.poly_scalar_mult_to(a);

            R.poly_sub_to(temp);
            Q.poly_add_mon_to(a,i);
        }
        poly_set(Q);
    }
}


//To Varainte der exakten Division

void int_poly::poly_div_to(int_poly &Q,int_poly &R,const int_poly B)
{
    this->poly_div( Q, R,*this,B);
}

// pseudo Division nach Cohen 3.1.2 (geht eleganter)

void int_poly::poly_pseudodiv_rem( int_poly A,  int_poly B)
{

    if (B.is_zero()==0)
    {
        int_poly temp;
        int_poly R;
        R.poly_set(A);
        int e=A.deg-B.deg+1;
        while (R.deg>=B.deg)
        {

            temp.poly_mon_mult(B,R.deg-B.deg);
            temp.poly_scalar_mult_to(R.coef[R.deg]);
            R.poly_scalar_mult_to(B.coef[B.deg]);
            R.poly_sub_to(temp);
            e--;

        }
        mpz_t q;
        mpz_init_set(q,B.coef[B.deg]);
        mpz_pow_ui(q,q,e);
        R.poly_scalar_mult_to(q);
        poly_set(R);
    }
}

//To Variante Algo 3.1.2 nach Cohen

void int_poly::poly_pseudodiv_rem_to(const int_poly B)
{
    this->poly_pseudodiv_rem(*this,B);
}


//Pseudodivision nach Kaplan, M. Computeralgebra 4.6 welche q^e*A=Q*B+R
//berechnet und entsprechendes in Q und R hineinschreibt

void int_poly::poly_pseudodiv(int_poly &Q, int_poly &R, int_poly A,  int_poly B)
{

    if (B.is_zero()==0)
    {
        // Initialisierungen: Vergiss zunächst die Hauptnenner von A und B (--> R bzw. Bint)
        int_poly temp;
        R.poly_set(A);


        int k = A.deg - B.deg;

        //Initialisiere Q mit 0en
        Q.deg=k;
        for (int i=0;i<=k;i++)
        {
            mpz_init_set_ui(Q.coef[i],0);
        }


        // Algorithmus
        while (k>=0)
        {

            mpz_set(Q.coef[k],R.coef[R.deg]);

            temp.poly_mon_mult(B,k);
            temp.poly_scalar_mult_to(R.coef[R.deg]);

            R.poly_scalar_mult_to(B.coef[B.deg]);
            R.poly_sub_to(temp);

            k=R.deg-B.deg;
        }

        int delta;
        delta=0;
        mpz_t dummy;
        mpz_init_set_ui(dummy,0);

        for (int i=0;i<=A.deg-B.deg;i++)
        {
            if (mpz_cmp_ui(Q.coef[i],0)!=0)
            {
                mpz_pow_ui(dummy,B.coef[B.deg],delta);
                mpz_mul(Q.coef[i],Q.coef[i],dummy);
                delta++;
            }

        }

    }


}


//To Variante Algo 3.1.2 nach Cohen

void int_poly::poly_pseudodiv_to(int_poly &Q, int_poly &R, int_poly B)
{
    this->poly_pseudodiv(Q, R,*this,B);
}

// Kombinationen

// a := a*b + c
void int_poly::poly_multadd_to(const int_poly b, const int_poly c)
{
    poly_mult_n_to(b);
    poly_add_to(c);
}

//a=a*b-c
void int_poly::poly_multsub_to(const int_poly b, const int_poly c)
{
    poly_mult_n_to(b);
    poly_sub_to(c);
}



/*
// a := (a+b)* c
void int_poly::poly_addmult_to(const int_poly b, const int_poly c)
{
        int_poly a(deg,coef);
        a.poly_add_to(b);
        a.poly_mult_n_to(c);
        poly_set(a);
}
*/

// Eigenschaften

// Content (Cohen 3.2.7), schreibt Ergebnis ins Argument cont
void int_poly::poly_cont(mpz_t& cont)
{
    if (is_zero()==1)
    {
        mpz_init_set_ui(cont,0);
    }
    else
    {
        mpz_t temp;
        int i=1;
        mpz_init_set(temp,coef[0]);
        while (mpz_cmp_ui(temp,1)!=0 && i<=deg)
        {
            mpz_gcd(temp,temp,coef[i]);
            i++;
        }
        mpz_init_set(cont,temp);
    }
}


// Primitive Part (Cohen 3.2.7) unter Verwendung von mpz_divexact
// da wir wissen,dass der Inhalt alle Elemente teilt
//ÜBERSCHREIBT DAS int_polyNOM WELCHES DEN BEFEHL AUFRUFT!!!!


void int_poly::poly_pp(int_poly f)
{
    mpz_t cont;
    f.poly_cont(cont); // cont ist auf den Inhalt von f gesetzt.

    if (mpz_cmp_ui(cont,1)==0)
        poly_set(f);
    else
    {
        deg=f.deg;
        for (int i=0;i<=deg;i++)
        {
            mpz_init_set_ui(coef[i],0);
            mpz_divexact(coef[i],f.coef[i],cont);
        }

    }
}



//Sonstiges
void int_poly::poly_horner(mpz_t erg, const mpz_t u)
{
    mpz_init_set(erg,coef[deg]);
    for (int i=deg;i>=1;i--)
    {
        mpz_mul(erg,erg,u);
        mpz_add(erg,erg,coef[i-1]);
    }
}

// int_polynom in int_polynom einsetzen(Horner-Schema) KRITISCHE EINGABE x^2, x^2  ....

void int_poly::poly_horner_int_poly(const int_poly A,const int_poly B)
{
    poly_set(A.coef[A.deg]);
    for (int i=A.deg;i>=1;i--)
    {
        poly_mult_n_to(B);
        poly_add_const_to(A.coef[i-1]);
    }
}



//Hilfsfunktionen


//setze int_polynom auf int_polynom b
void int_poly::poly_set(const int_poly b)
{
    deg=b.deg;
    for(int i=0;i<=deg;i++)
    {
        mpz_init_set(coef[i],b.coef[i]); // Hier wird init set dringendst benötigt
    }

}

// setze int_polynom auf konstantes int_polynom b
void int_poly::poly_set(const mpz_t b)
{
    deg=0;
    mpz_init_set(coef[0],b);
}


//setze int_polynom auf Nullint_polynom
void int_poly::poly_set_zero()
{
    deg = -1;
}


//Vergleiche ob 2 int_polynome gleich return 1 falls ja sont 0

int int_poly::is_equal(const int_poly g) const
{
    if (deg!=g.deg)
        return 0;
    else

        for (int i=deg;i>=0; i--)
        {
        if (mpz_cmp(coef[i],g.coef[i])!=0)
        {return 0;}
    }
    return 1;
}

//Überprüft ob das int_polynom 0 ist

int int_poly::is_zero() const
{
    if (deg<0)
        return 1;
    else
        return 0;

}

int int_poly::is_one() const
{
    if (deg==0)
    {
        if (mpz_cmpabs_ui(coef[0],1)==0) { return 1; }
        else { return 0; }
    }
    else { return 0; }
}

int int_poly::is_monic() const
{
    if (mpz_cmpabs_ui(coef[deg],1)==0)
        return 1;
    else
        return 0;
}

// klassischer GGT nach Cohen 3.2.1

void int_poly::poly_gcd( int_poly A,  int_poly B)
{
    if (A.deg<B.deg)
        poly_gcd(B,A);
    else if (B.is_zero()==1)
        poly_set(A);
    else if (B.is_monic()==0)
    {
        //cout << "Subresultanten GGT wird benutzt"<<endl;
        poly_subgcd(A,B);
    }
    else
    {
        int_poly Q;
        int_poly App;
        int_poly Bpp;
        int_poly R;
        App.poly_set(A);
        Bpp.poly_set(B);

        while (Bpp.is_zero()==0)
        {
            R.poly_div(Q,R,App,Bpp);
            App.poly_set(Bpp);
            Bpp.poly_set(R);
        }
        poly_set(App);
    }

}

// GGT nach Cohen, Algorithmus 3.2.10 (Primitive int_polynomial GCD) TO DO: Optimierung bzgl. Mehrfachberechnung)
// Bpp ist das B in den Schritten ab 2


void int_poly::poly_ppgcd(int_poly A,int_poly B)
{
    if(A.deg<B.deg)
    {
        poly_ppgcd(B,A);

    }
    else if(B.is_zero()==1)
    {
        poly_set(A);

    }
    else
    {
        //Initialisierung und Reduktionen
        mpz_t a;
        mpz_init_set_ui(a,0);
        mpz_t b;
        mpz_init_set_ui(b,0);
        mpz_t d;
        mpz_init_set_ui(d,0);
        A.poly_cont(a);
        B.poly_cont(b);
        mpz_gcd(d,a,b);

        int_poly App;
        int_poly Bpp;
        int_poly R;

        //Erster Schritt im Algo
        App.poly_pp(A);
        Bpp.poly_pp(B);
        R.poly_pseudodiv_rem(App,Bpp);

        //Algo

        while (R.deg>0)
        {
            App.poly_set(Bpp);
            Bpp.poly_pp(R);
            R.poly_pseudodiv_rem(App,Bpp);
        }

        if (R.is_zero()==0)
        {
            deg=0;
            mpz_init_set(coef[0],d);
        }
        else
        {
            poly_set(Bpp);
            poly_scalar_mult_to(d);
        }
    }
}
// To Variante ppgcd


void int_poly::poly_ppgcd_to(int_poly B)
{
    this->poly_ppgcd(*this,B);
}



// GGT nach Cohen, Algorithmus 3.3.1 (Subresultant int_polynomial GCD) TO DO: Optimierung bzgl. Mehrfachberechnung)
// Bpp ist das B in den Schritten ab 2
void int_poly::poly_subgcd(int_poly A, int_poly B)
{
    //Initialisierung und Reduktionen
    if(A.deg<B.deg)
    {
        poly_subgcd(B,A);

    }
    else if(B.is_zero()==1)
    {
        poly_set(A);

    }
    else
    {
        mpz_t a;
        mpz_init_set_ui(a,0);
        mpz_t b;
        mpz_init_set_ui(b,0);
        mpz_t d;
        mpz_init_set_ui(d,0);
        mpz_t h;
        mpz_init_set_ui(h,1);
        mpz_t g;
        mpz_init_set_ui(g,1);
        mpz_t temp1;
        mpz_init_set_ui(temp1,0);
        mpz_t temp2;
        mpz_init_set_ui(temp2,0);

        A.poly_cont(a);
        B.poly_cont(b);
        mpz_gcd(d,a,b);

        int_poly App;
        int_poly Bpp;
        int_poly R;

        //Erster Schritt im Algo
        int delta;
        App.poly_pp(A);
        Bpp.poly_pp(B);
        R.poly_pseudodiv_rem(App,Bpp);

        //Algo

        while (R.deg>0)
        {
            delta =App.deg-Bpp.deg;

            mpz_pow_ui(temp1,h,delta);
            mpz_mul(temp2,temp1,g);
            App.poly_set(Bpp);
            Bpp.poly_pp(R);
            Bpp.poly_scalar_div_to(temp2);

            mpz_set(g,App.coef[App.deg]);
            mpz_pow_ui(temp1,h,1-delta);
            mpz_pow_ui(temp2,g,delta);
            mpz_mul(h,temp1,temp2);


            App.poly_set(Bpp);
            Bpp.poly_pp(R);
            R.poly_pseudodiv_rem(App,Bpp);

        }

        if (R.is_zero()==0)
        {
            deg=0;
            mpz_init_set(coef[0],d);
        }
        else
        {
            poly_set(Bpp);
            poly_cont(temp1);
            poly_scalar_mult_to(d);
            poly_scalar_div_to(temp1);
        }
    }
}

// To Varianta Subresultanten

void int_poly::poly_subgcd_to(int_poly B)
{
    this->poly_subgcd(*this,B);
}


//Extended Subresultant GCD; see Kaplan, M. Computeralgebra, chapter 4.6
//returns g=r*A+t*B IT WORKS DONT TOUCH IT!!!!!!!!
void int_poly::poly_extsubgcd(int_poly& r, int_poly& t,int_poly &g,int_poly A,int_poly B)
{
    if (A.deg<B.deg)
        poly_extsubgcd(t,r,g,B,A);
    else if (B.is_zero()==1)
    {
        g.poly_set(A);
        t.poly_set_zero();

        mpz_t temp;
        mpz_init_set_ui(temp,1);
        r.poly_set(temp);
    }

    else
    {
        //Initialization (temp will be -1 in the algorithm)
        mpz_t temp;
        mpz_t temp2;
        mpz_t psi;
        int alpha;
        int delta;
        int delta2;
        mpz_t base; //will be always (-1)^ ...
        mpz_t base2; //will be always (-1)^ .../LK ...
        mpz_t base3;
        mpz_init_set_ui(temp,1);
        mpz_init_set_ui(base,1);
        mpz_init_set_ui(base2,1);
        mpz_init_set_ui(base3,1);
        mpz_init_set_ui(psi,1);
        delta=A.deg-B.deg;
        delta2=delta;
        alpha=delta2+1;

        int_poly dummy; // is needed in the main algorithm for -q*r_i resp. -q*t_i
        dummy.poly_set_zero();

        int_poly dummy2; // is needed in the main algorithm for LK * r_i resp LK* t_i
        dummy2.poly_set_zero();

        int_poly f1;
        int_poly f2;
        int_poly f3;
        int_poly f;

        int_poly q;

        int_poly r1;
        int_poly r2;
        int_poly r3;

        int_poly t1;
        int_poly t2;
        int_poly t3;

        f1.poly_set(A);
        f2.poly_set(B);
        f.poly_set_zero();

        r1.poly_set(temp);
        r2.poly_set_zero();

        t1.poly_set_zero();
        t2.poly_set(temp);

        mpz_set_si(temp,-1);

        //Calculating first step
        mpz_init_set_ui(temp2,0);
        mpz_pow_ui(temp2,f2.coef[f2.deg],alpha);
        f.poly_scalar_mult(f1,temp2);


        A.poly_div(q,f3,f,f2);
        mpz_pow_ui(base,temp,alpha);
        f3.poly_scalar_mult_to(base);


        r3.poly_set(base);
        mpz_pow_ui(base2,f2.coef[f2.deg],alpha);
        r3.poly_scalar_mult_to(base2);


        mpz_pow_ui(base,temp,delta);
        t3.poly_set(base);
        t3.poly_mult_n_to(q);



        //Correcting the polynomials and constants

        f1.poly_set(f2);
        f2.poly_set(f3);

        r1.poly_set(r2);
        r2.poly_set(r3);

        t1.poly_set(t2);
        t2.poly_set(t3);

        delta=delta2;
        delta2=f1.deg-f2.deg;
        alpha=delta2+1;

        //Main Algorithm
        while (f2.is_zero()==0)
        {


            //Step 1.1+1.2: base and base 2 will be psi^ ... and LK^...

            mpz_pow_ui(temp2,f2.coef[f2.deg],alpha);
            f.poly_scalar_mult(f1,temp2);
            A.poly_div(q,f3,f,f2);


            mpz_pow_ui(base,psi,delta);
            mpz_pow_ui(base2,f1.coef[f1.deg],delta);


            mpz_mul(base2,base2,psi);
            mpz_divexact(psi,base2,base);

            //Step 1.3

            mpz_pow_ui(base,temp,alpha);
            mpz_pow_ui(base2,psi,delta2);
            mpz_mul(base2,base2,f1.coef[f1.deg]);
            f3.poly_scalar_div_to(base2);
            f3.poly_scalar_mult_to(base);


            //Step 1.4

            mpz_pow_ui(base3,f2.coef[f2.deg],alpha);

            //computing r_i
            dummy.poly_mult_n(q,r2);
            dummy2.poly_scalar_mult(r1,base3);
            r3.poly_sub(dummy2,dummy);
            r3.poly_scalar_mult_to(base);
            r3.poly_scalar_div_to(base2);

            //computing t_i
            dummy.poly_mult_n(q,t2);
            dummy2.poly_scalar_mult(t1,base3);
            t3.poly_sub(dummy2,dummy);
            t3.poly_scalar_mult_to(base);
            t3.poly_scalar_div_to(base2);

            //Correcting the polynomials and constants

            f1.poly_set(f2);
            f2.poly_set(f3);

            r1.poly_set(r2);
            r2.poly_set(r3);

            t1.poly_set(t2);
            t2.poly_set(t3);

            delta=delta2;
            delta2=f1.deg-f2.deg;
            alpha=delta2+1;

        }

        //Computing result
        g.poly_set(f1);
        r.poly_set(r1);
        t.poly_set(t1);

    }


}

//Ein & Ausgabe

//Eingabe

void int_poly::poly_insert()
{
#if 0
    cout << "Bitte geben Sie ein int_polynom ein! Zunächst den Grad: " << endl;
    cin >> deg;


    for ( int i=0; i<=deg;i++)
    {
        mpz_init_set_ui(coef[i],0);
        printf("Geben Sie nun f[%i] ein:",i);
        mpz_inp_str(coef[i],stdin, 10);
    }
#endif
}


//Ausgabe
void int_poly::poly_print()
{
#if 0
    if (is_zero()==1)
        cout << "0" << "\n" <<endl;
    else
    {
        for (int i=deg;i>=1;i--)
        {
            mpz_out_str(stdout,10, coef[i]);
            printf("X%i+",i);
        }
        mpz_out_str(stdout,10, coef[0]);
        printf("\n");
    }
#endif
}

#endif
