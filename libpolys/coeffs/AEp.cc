#include "misc/auxiliary.h"

#ifdef SINGULAR_4_2
#include "AEp.h"

#include <stdio.h>
#include <math.h>


using namespace std;

//Konstruktoren

p_poly::p_poly()
{
    //coef = reinterpret_cast<mpz_t*> (omAlloc(100*sizeof(mpz_t)));
    deg=-1;
    mod=2;
    mpz_init_set_ui(coef[0],0);
}



p_poly::p_poly(int n,int p, mpz_t *a)
{

    deg=n;
    mod=p;

    for ( int i=0;i<=n;i++)
    {
        mpz_mod_ui(a[i],a[i],mod);
        mpz_init_set(coef[i], a[i]);
    }

}

/*
//Destruktor

p_poly::~p_poly()
{
        delete[] coef;
}

*/

//Reduktion modulo p

void p_poly::p_poly_reduce(p_poly f,int p)
{
    if (f.is_zero()==0)
    {

        for (int i=f.deg;i>=0;i--)
        {
            mpz_mod_ui(coef[i],f.coef[i],p);
        }
    }
    //Hier nötige Grad Korrektur
    int k=deg;
    while(mpz_sgn(coef[k])==0 && k>=0)
    {deg--;k--;}
}


// Arithmetik


//Additionen

//Standard - Addition
void p_poly::p_poly_add(const p_poly a, const p_poly b)
{
    if (a.deg >=b.deg)
    {

        deg=a.deg;
        mod=a.mod;

        for ( int i=0;i<=b.deg;i++)
        {
            mpz_add(coef[i],a.coef[i],b.coef[i]);
        }

        for ( int i=b.deg+1;i<=a.deg;i++)
        {
            mpz_init_set(coef[i],a.coef[i]);
        }

        //Hier nötige Grad Korrektur
        int k=deg;
        while(mpz_divisible_ui_p(coef[k],mod)!=0 && k>=0)
        {deg--;k--;}


    }

    else {p_poly_add(b,a);  }

}

//Überschreibende Addition

void p_poly::p_poly_add_to(const p_poly g)
{
    this->p_poly_add(*this,g);
}

//Addition einer Konstanten
void p_poly::p_poly_add_const(p_poly f,const mpz_t a)
{
    if (f.is_zero()==1 && mpz_divisible_ui_p(a,f.mod)==0)
        p_poly_set(a,f.mod);

    else if (mpz_divisible_ui_p(a,f.mod)==0)
    {
        p_poly_set(f);
        mpz_add(coef[0],coef[0],a);
        /*/Hier nötige Grad Korrektur
        int k=deg;
        while(mpz_divisible_ui_p(coef[k],mod)!=0 && k>=0)
        {deg--;k--;}
        */
    }

}


//To Variante Addition einer Konstanten

void p_poly::p_poly_add_const_to(const mpz_t a)
{
    this->p_poly_add_const(*this,a);
}

//Monom Addition
void p_poly::p_poly_add_mon(const p_poly f, mpz_t a, int i)
{
    p_poly_set(f);
    if (i<=deg  && is_zero()==0)
    {
        mpz_add(coef[i],coef[i],a);
    }
    else if (is_zero()==1 && mpz_divisible_ui_p(a,f.mod)==0)
    {
        deg=i;
        for(int j=0;j<=i;j++)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_t temp;
        mpz_init_set_ui(temp,0);
        mpz_mod_ui(temp,a,mod);
        mpz_add(coef[i],coef[i],temp);

    }
    else if(i>deg && mpz_divisible_ui_p(a,f.mod)==0)
    {
        deg=i;
        for(int j=i;j>deg;j--)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_t temp;
        mpz_init_set_ui(temp,0);
        mpz_mod_ui(temp,a,mod);
        mpz_add(coef[i],coef[i],temp);

    }
    /*/Hier nötige Grad Korrektur
    int k=deg;
    while(mpz_divisible_ui_p(coef[k],mod)!=0 && k>=0)
    {deg--;k--;}
    */

}

//To Variante Monomaddition
void p_poly::p_poly_add_mon_to(mpz_t a, int i)
{

    if (i<=deg  && is_zero()==0)
    {
        mpz_add(coef[i],coef[i],a);
    }
    else if (is_zero()==1 && mpz_divisible_ui_p(a,mod)==0)
    {
        deg=i;
        for(int j=0;j<=i;j++)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_t temp;
        mpz_init_set_ui(temp,0);
        mpz_mod_ui(temp,a,mod);
        mpz_add(coef[i],coef[i],temp);

    }
    else if(i>deg && mpz_divisible_ui_p(a,mod)==0)
    {
        deg=i;
        for(int j=i;j>deg;j--)
        {
            mpz_init_set_ui(coef[j],0);
        }
        mpz_t temp;
        mpz_init_set_ui(temp,0);
        mpz_mod_ui(temp,a,mod);
        mpz_add(coef[i],coef[i],temp);

    }
    /*Hier nötige Grad Korrektur
    int k=deg;
    while(mpz_divisible_ui_p(coef[k],mod)!=0 && k>=0)
    {deg--;k--;} */
}

//Subtraktionen

void p_poly::p_poly_sub(const p_poly a, const p_poly b)
{
    if (a.deg >=b.deg)
    {

        deg=a.deg;
        mod=a.mod;

        for ( int i=0;i<=b.deg;i++)
        {
            mpz_sub(coef[i],a.coef[i],b.coef[i]);
        }

        for ( int i=b.deg+1;i<=a.deg;i++)
        {
            mpz_init_set(coef[i],a.coef[i]);
        }

        //Hier nötige Grad Korrektur
        int k=deg;
        while(mpz_divisible_ui_p(coef[k],mod)!=0 && k>=0)
        {deg--;k--;}

    }

    else {p_poly_sub(b,a);p_poly_neg();  }

}


//Überschreibende Subtraktion

void p_poly::p_poly_sub_to(const p_poly b)
{
    this->p_poly_sub(*this,b);
}

//Subtraktion einer Konstanten
void p_poly::p_poly_sub_const(p_poly f,const mpz_t a)
{
    if (f.is_zero()==1)
    {
        p_poly_set(a,f.mod);
        p_poly_neg();
    }
    else
    {
        p_poly_set(f);
        mpz_sub(coef[0],coef[0],a);
    }
    /*/*Hier nötige Grad Korrektur
    int k=deg;
    while(mpz_divisible_ui_p(coef[k],mod)!=0 && k>=0)
    {deg--;k--;} */

}


//To Variante Subtraktion einer Konstanten

void p_poly::p_poly_sub_const_to(const mpz_t a)
{
    this->p_poly_sub_const(*this,a);
}


//Monom Subtraktion
void p_poly::p_poly_sub_mon(const p_poly f , mpz_t a, int i)
{
    mpz_t temp;
    mpz_neg(temp,a);
    p_poly_add_mon(f,temp,i);
}

//To Variante Monomaddition
void p_poly::p_poly_sub_mon_to(mpz_t a, int i)
{
    mpz_t temp;
    mpz_neg(temp,a);
    p_poly_add_mon_to(temp,i);
}


//Multiplikationen

//Multiplikation mit Monom
void p_poly::p_poly_mon_mult( p_poly f, int n)
{
    if (f.is_zero()==1)
    {p_poly_set_zero();}
    else
    {
        deg=f.deg+n;
        mod=f.mod;
        for (int i=deg;i>=n;i--)
        {
            mpz_init_set(coef[i],f.coef[i-n]);
        }
        for (int i=n-1;i>=0;i--)
        {
            mpz_init_set_ui(coef[i],0);
        }

        /*/Hier nötige Grad Korrektur
    int k=deg;
    while(mpz_divisible_ui_p(coef[k],mod)!=0 && k>=0)
    {deg--;k--;} */
    }
}

void p_poly::p_poly_mon_mult_to(const int n)
{
    this->p_poly_mon_mult(*this,n);
}


//Multiplikation mit Skalar

void p_poly::p_poly_scalar_mult(const p_poly g, const mpz_t n)
{
    if (mpz_divisible_ui_p(n,g.mod)!=0)
        p_poly_set_zero();
    else
    {
        deg=g.deg;
        mod=g.mod;

        mpz_t temp;
        mpz_init_set_ui(temp,0);
        for(int i=0;i<=deg;i++)
        {
            mpz_mul(temp,n,g.coef[i]);
            mpz_init_set(coef[i],temp);
        }
    }
}



void p_poly::p_poly_scalar_mult(const mpz_t n, const p_poly g)
{
    if (mpz_divisible_ui_p(n,g.mod)!=0)
        p_poly_set_zero();
    else
    {
        deg=g.deg;
        mod=g.mod;


        mpz_t temp;
        mpz_init_set_ui(temp,0);
        for(int i=0;i<=deg;i++)
        {
            mpz_mul(temp,n,g.coef[i]);
            mpz_init_set(coef[i],temp);
        }
        /*/Hier nötige Grad Korrektur
        int k=deg;
        while(mpz_divisible_ui_p(coef[k],mod)!=0 && k>=0)
        {deg--;k--;} */
    }
}


void p_poly::p_poly_scalar_mult_to(const mpz_t n)
{
    this->p_poly_scalar_mult(*this,n);
}



// Negation

void p_poly::p_poly_neg()
{
    for (int i=0;i<=deg;i++)
    {
        mpz_neg(coef[i],coef[i]);
    }
}

// Naive Multiplikation
void p_poly::p_poly_mult_n(p_poly a,p_poly b)
{
    //Reduktion mod p
    a.p_poly_reduce(a,a.mod);
    b.p_poly_reduce(b,b.mod);

    if (a.is_zero()==1 || b.is_zero()==1)
        p_poly_set_zero();
    else
    {
        mpz_t temp;
        mpz_init_set_ui(temp,0);
        deg = a.deg + b.deg;

        // Kopien atemp und btemp von a bzw. b, mit Nullen aufgefüllt
        p_poly atemp, btemp;
        atemp.p_poly_set(a);
        btemp.p_poly_set(b);
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

        /*/Hier nötige Grad Korrektur
        int k=deg;
        while(mpz_divisible_ui_p(coef[k],mod)!=0 && k>=0)
        {deg--;k--;} */


    }
}


//Überschreibende Multiplikation

void p_poly::p_poly_mult_n_to(const p_poly g)
{
    this->p_poly_mult_n(*this,g);

}

// Karatsuba-Multiplikation (Weimerskirch/Paar Alg. 1), ACHTUNG VORLÄUFIGE VERSION, macht noch Fehler beim Grad und ist unelegant !!!
void p_poly::p_poly_mult_ka( p_poly A,  p_poly B)
{

    if (A.is_zero()==1 || B.is_zero()==1)
    {
        p_poly_set_zero();
    }
    else
    {
        //Reduktion mod p
        A.p_poly_reduce(A,A.mod);
        B.p_poly_reduce(B,B.mod);

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
            p_poly_set(AB,A.mod);
            this->p_poly_reduce(*this,A.mod);
        }
        else
        {
            // p_polynome A und B aufspalten
            p_poly Au, Al, Bu, Bl;
            Au.p_poly_mon_div(A,n/2);
            Al.p_poly_mon_div_rem(A,n/2);
            Bu.p_poly_mon_div(B,n/2);
            Bl.p_poly_mon_div_rem(B,n/2);
            p_poly Alu,Blu;
            Alu.p_poly_add(Al,Au);
            Blu.p_poly_add(Bl,Bu);

            // Teile rekursiv multiplizieren
            p_poly D0, D1, D01;
            D0.p_poly_mult_ka(Al,Bl);
            D1.p_poly_mult_ka(Au,Bu);
            D01.p_poly_mult_ka(Alu,Blu);

            // Ergebnis zusammensetzen
            p_poly temp;
            D01.p_poly_sub_to(D0);
            D01.p_poly_sub_to(D1);
            D01.p_poly_mon_mult_to(n/2);
            D1.p_poly_mon_mult_to(n);
            D1.p_poly_add_to(D01);
            D1.p_poly_add_to(D0);
            p_poly_set(D1);

        }

        //Hier nötige Grad Korrektur
        int k=deg;
        while(mpz_divisible_ui_p(coef[k],mod)!=0 && k>=0)
        {deg--;k--;}
    }
}



//Skalare Divisionen

void p_poly::p_poly_scalar_div( const p_poly g, const mpz_t n)
{

    if (mpz_divisible_ui_p(n,g.mod)==0) // überprüft invertierbarkeit
    {
        deg=g.deg;
        mod=g.mod;


        mpz_t temp;
        mpz_t p;
        mpz_init_set_ui(temp,0);
        mpz_init_set_ui(p,mod);
        for(int i=0;i<=deg;i++)
        {
            mpz_invert(temp,n,p);
            mpz_mul(temp,g.coef[i],temp);
            mpz_init_set(coef[i],temp);
        }

        /*/Hier nötige Grad Korrektur
        int k=deg;
        while(mpz_divisible_ui_p(coef[k],mod)!=0 && k>=0)
        {deg--;k--;} */
    }
}


void p_poly::p_poly_scalar_div_to(const mpz_t n)
{
    this->p_poly_scalar_div(*this,n);
}

// Division durch Monom - Quotient
void p_poly::p_poly_mon_div(const p_poly f, const int n)
{
    if (f.deg<n)
    {
        p_poly_set_zero();
    }
    else
    {
        deg=f.deg-n;
        mod=f.mod;

        for (int i=0;i<=f.deg-n;i++)
        {
            mpz_init_set(coef[i],f.coef[n+i]);
        }
    }
}

// Division durch Monom - Rest
void p_poly::p_poly_mon_div_rem(const p_poly f, const int n)
{
    if (f.deg<n)
    {
        p_poly_set(f);
    }
    else
    {
        deg=n-1;
        mod=f.mod;


        for (int i=0;i<=n-1;i++)
        {
            mpz_init_set(coef[i],f.coef[i]);
        }
    }
}




//Euklidische Division nach Cohen Algo 3.1.1 (degA muss größer gleich deg B sein)!!

void p_poly::p_poly_div_rem( p_poly A,  p_poly B)
{

    if (B.is_zero()==0)
    {
        //Reduktion mod p
        A.p_poly_reduce(A,A.mod);
        B.p_poly_reduce(B,B.mod);

        p_poly R;
        p_poly temp;
        R.p_poly_set(A);
        mpz_t a;
        mpz_t u;
        mpz_t p;
        mpz_init_set_ui(p,A.mod);
        mpz_init_set_ui(a,0);
        mpz_init_set_ui(u,0);
        int i;

        mpz_invert(u,B.coef[B.deg],p); // Inverse von lc(B)


        while (R.deg>=B.deg)
        {

            mpz_mul(a,R.coef[R.deg],u);
            i=R.deg-B.deg;

            temp.p_poly_mon_mult(B,i);
            temp.p_poly_scalar_mult_to(a);

            R.p_poly_sub_to(temp);

        }
        p_poly_set(R);

        /*/Hier nötige Grad Korrektur
    int k=deg;
    while(mpz_divisible_ui_p(coef[k],mod)!=0 && k>=0)
    {deg--;k--;}*/
    }
}
//To Variante von Algo 3.1.1 im Cohen

void p_poly::p_poly_div_rem_to(const p_poly B)
{
    this->p_poly_div_rem(*this,B);


}



//Exakte Division nach Cohen 3.1.1
void p_poly::p_poly_div(p_poly &Q, p_poly &R, p_poly A,  p_poly B)
{
    if (B.is_zero()==0)
    {
        //Reduktion mod p
        A.p_poly_reduce(A,A.mod);
        B.p_poly_reduce(B,B.mod);

        //Initialisierungen
        p_poly temp;
        R.p_poly_set(A);
        Q.p_poly_set_zero();
        Q.mod=A.mod;

        mpz_t a;
        mpz_t b;
        mpz_t p;
        mpz_init_set_ui(p,A.mod);
        mpz_init_set_ui(a,0);
        mpz_init_set_ui(b,0);
        int i;
        mpz_invert(b,B.coef[B.deg],p);

        //Algorithmus TO DO: evtl hier mit auch den Rest ausgeben
        while (R.deg>=B.deg)
        {

            mpz_mul(a,R.coef[R.deg],b);
            i=R.deg-B.deg;

            temp.p_poly_mon_mult(B,i);
            temp.p_poly_scalar_mult_to(a);

            R.p_poly_sub_to(temp);

            Q.p_poly_add_mon_to(a,i);

            R.p_poly_reduce(R,R.mod);
            Q.p_poly_reduce(Q,Q.mod);
        }

        /*/Hier nötige Grad Korrektur Q
    int k=Q.deg;
    while(mpz_divisible_ui_p(Q.coef[k],Q.mod)!=0 && k>=0)
    {Q.deg--;k--;}*/

        /*/Hier nötige Grad Korrektur R
    k=R.deg;
    while(mpz_divisible_ui_p(R.coef[k],R.mod)!=0 && k>=0)
    {R.deg--;k--;} */
    }
}


//To Varainte der exakten Division

void p_poly::p_poly_div_to(p_poly &Q,p_poly &R, p_poly B)
{
    this->p_poly_div(Q ,R,*this,B);
}


// Kombinationen

// a := a*b + c
void p_poly::p_poly_multadd_to(const p_poly b, const p_poly c)
{
    p_poly_mult_n_to(b);
    p_poly_add_to(c);
}

//a=a*b-c
void p_poly::p_poly_multsub_to(const p_poly b, const p_poly c)
{
    p_poly_mult_n_to(b);
    p_poly_sub_to(c);
}



/*
// a := (a+b)* c
void p_poly::poly_addmult_to(const p_poly b, const p_poly c)
{
        p_poly a(deg,coef);
        a.poly_add_to(b);
        a.poly_mult_n_to(c);
        poly_set(a);
}
*/



//Sonstiges
void p_poly::p_poly_horner(mpz_t erg, const mpz_t u)
{
    if (is_zero()==0)
    {
        mpz_init_set(erg,coef[deg]);
        for (int i=deg;i>=1;i--)
        {
            mpz_mul(erg,erg,u);
            mpz_add(erg,erg,coef[i-1]);
        }

        //Reduktion
        mpz_mod_ui(erg,erg,mod);
    }
    else
    {
        mpz_set_ui(erg,0);
    }
}

// p_polynom in p_polynom einsetzen(Horner-Schema) KRITISCHE EINGABE x^2, x^2  ....

void p_poly::p_poly_horner_p_poly( p_poly A, p_poly B)
{
    //Reduktion mod p
    A.p_poly_reduce(A,A.mod);
    B.p_poly_reduce(B,B.mod);

    p_poly_set(A.coef[A.deg],A.mod);
    for (int i=A.deg;i>=1;i--)
    {
        p_poly_mult_n_to(B);
        p_poly_add_const_to(A.coef[i-1]);
    }
    /*/Hier nötige Grad Korrektur
    int i=deg;
    while(mpz_divisible_ui_p(coef[i],mod)!=0 && i>=0)
    {deg--;i--;} */
}



//Hilfsfunktionen


//setze p_polynom auf p_polynom b
void p_poly::p_poly_set(const p_poly b)
{
    deg=b.deg;
    mod=b.mod;


    for(int i=0;i<=deg;i++)
    {
        mpz_init_set(coef[i],b.coef[i]); // Hier wird init set dringendst benötigt
    }

}

// setze p_polynom auf konstantes p_polynom b
void p_poly::p_poly_set(const mpz_t b,int p)
{
    deg=0;
    mod=p;

    if (mpz_divisible_ui_p(b,mod)!=0)
        p_poly_set_zero();
    else
    {
        mpz_t temp;
        mpz_init_set(temp,b);
        mpz_mod_ui(temp,temp,p);
        mpz_init_set(coef[0],b);
    }
}


//setze p_polynom auf Nullpolynom
void p_poly::p_poly_set_zero()
{
    deg = -1;
}


//Vergleiche ob 2 p_polynome gleich return 1 falls ja sont 0

int p_poly::is_equal(const p_poly g) const
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

//Überprüft ob das p_polynom 0 ist

int p_poly::is_zero() const
{
    if (deg<0)
        return 1;
    else
        return 0;

}

int p_poly::is_one() const
{
    if (deg==0)
    {
        if (mpz_cmp_ui(coef[0],1)==0) { return 1; }
        else { return 0; }
    }
    else { return 0; }
}

int p_poly::is_monic() const
{
    if (mpz_cmpabs_ui(coef[deg],1)==0)
        return 1;
    else
        return 0;
}

// klassischer GGT nach Cohen 3.2.1

void p_poly::p_poly_gcd( p_poly A,  p_poly B)
{

    //Reduktion mod p
    A.p_poly_reduce(A,A.mod);
    B.p_poly_reduce(B,B.mod);

    if (A.deg<B.deg)
        p_poly_gcd(B,A);
    else if (B.is_zero()==1)
        p_poly_set(A);
    else
    {
        p_poly App;
        p_poly Bpp;
        p_poly R;
        App.p_poly_set(A);
        Bpp.p_poly_set(B);

        while (Bpp.is_zero()==0)
        {
            R.p_poly_div_rem(App,Bpp);
            App.p_poly_set(Bpp);
            Bpp.p_poly_set(R);
        }
        p_poly_set(App);
    }

}

//Nach nach Fieker 2.12 Symbolisches Rechnen (2012)
// gibt g=s*A+t*B aus
void p_poly::p_poly_extgcd(p_poly &s,p_poly &t,p_poly &g, p_poly A, p_poly B)
{

    //Reduktion mod p
    A.p_poly_reduce(A,A.mod);
    B.p_poly_reduce(B,B.mod);


    if (A.deg<B.deg)
        p_poly_extgcd(t,s,g,B,A);
    else if (B.is_zero()==1)
    {
        g.p_poly_set(A);
        t.p_poly_set_zero();

        mpz_t temp;
        mpz_init_set_ui(temp,1);
        s.p_poly_set(temp,A.mod);
    }

    else
    {
        mpz_t temp;
        mpz_init_set_ui(temp,1);

        p_poly R1;
        R1.p_poly_set(A);
        p_poly R2;
        R2.p_poly_set(B);
        p_poly R3;
        R3.mod=A.mod;


        p_poly S1;
        S1.p_poly_set(temp,A.mod);
        p_poly S2;
        S2.p_poly_set_zero();
        S2.mod=A.mod;
        p_poly S3;
        S3.mod=A.mod;

        p_poly T1;
        T1.p_poly_set_zero();
        T1.mod=A.mod;
        p_poly T2;
        T2.p_poly_set(temp,A.mod);
        p_poly T3;
        T3.mod=A.mod;

        p_poly Q;

        while (R2.is_zero()!=1)
        {
            p_poly_div(Q,R3,R1,R2);

            S3.p_poly_mult_n(Q,S2);
            S3.p_poly_neg();
            S3.p_poly_add_to(S1);

            T3.p_poly_mult_n(Q,T2);
            T3.p_poly_neg();
            T3.p_poly_add_to(T1);

            R1.p_poly_set(R2);
            R2.p_poly_set(R3);

            S1.p_poly_set(S2);
            S2.p_poly_set(S3);

            T1.p_poly_set(T2);
            T2.p_poly_set(T3);
        }
        t.p_poly_set(T1);
        s.p_poly_set(S1);
        g.p_poly_set(R1);
    }
}


//Ein & Ausgabe

//Eingabe

void p_poly::p_poly_insert()
{
#if 0
    cout << "Bitte geben Sie ein p_polynom ein! Zunächst den Grad: " << endl;
    cin >> deg;
    cout << "Jetzt den modul: " << endl;
    cin >> mod;

    for ( int i=0; i<=deg;i++)
    {
        mpz_init_set_ui(coef[i],0);
        printf("Geben Sie nun f[%i] ein:",i);
        mpz_inp_str(coef[i],stdin, 10);
    }
    //Reduktion
    this->p_poly_reduce(*this,mod);
#endif
}


//Ausgabe
void p_poly::p_poly_print()
{
#if 0

    //Reduktion
    this->p_poly_reduce(*this,mod);


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
