
#include "canonicalform.h"
#ifdef HAVE_BIFAC
#ifndef NOSTREAMIO
#include<fstream>
#endif
#include<math.h>
#include <sys/timeb.h>



static void
fillVarsRec ( const CanonicalForm & f, int * vars )
{
  int n;
  if ( (n = f.level()) > 0 )
  {
    vars[n] = 1;
    CFIterator i;
    for ( i = f; i.hasTerms(); ++i )
      fillVarsRec( i.coeff(), vars );
  }
}

int ExtensionLevel();
void Reduce( bool);

CanonicalForm MYGCD( const CanonicalForm& f, const CanonicalForm& g);


CanonicalForm MyContent( const CanonicalForm& F, const Variable& x)
{
  CanonicalForm r,t;
  CanonicalForm g=F;
  CanonicalForm one=1;

  if( F.isZero() ) return 0;
  if( F.inBaseDomain() )  return F; 

  if( level(F) < 0 )  return 1;  

  r = LC( F, x);

  g = g - power(x,degree(g,x))*r;

  while( g.isZero() != 1 && r!= 1 && r!=-1 )
  {
    t = LC(g, x);
    if( t == 1 || t == -1 ) return 1; 
    r = MYGCD( r, t);
    if( r == 1 ) return 1; 
    g = g - power(x,degree(g,x))*t; 
  }
  return r;
}

void CurrentExtension()
{
  Variable x('x');
  int i;
#ifndef NOSTREAMIO
  cout << "Current Extension: "<<endl;
#endif
  for (i = ExtensionLevel();i>0;i--)
  {
    Variable l(-i);
#ifndef NOSTREAMIO
    cout << "Variable: "<<l<<" Level: "<<l.level()<<" Minimal Polynom: "<<getMipo(l,x)<<endl;
#endif
  }
}

/*Liefert den ggt aller numerischen Koeffizienten einer Canonischen Form */

CanonicalForm MyNum(const CanonicalForm & a)
{
  bool bruch = isOn(SW_RATIONAL);
  Off (SW_RATIONAL);

  CanonicalForm dummy =0;
  CanonicalForm dummy2;

  CFIterator F =a;

  for ( ; F.hasTerms(); F++)
  {
    if (F.coeff().inBaseDomain())
    {
      dummy2 = F.coeff().num();
      if (dummy == 0)
      {
        dummy = dummy2;
      }
      else
      {
        dummy = gcd(dummy, dummy2);
      }
    }
    else
    {
      dummy2 = MyNum(F.coeff());
      if (dummy == 0)
      {
        dummy = dummy2;
      }
      else
      {
        dummy = gcd(dummy, dummy2);
      }
    }
  }
  if (bruch)
    On (SW_RATIONAL);
  else
    Off(SW_RATIONAL);
  return dummy;
}

/* Liefert den kgV aller Nenner der Koeffizenten einer Canonischen Form */

CanonicalForm MyDen(const CanonicalForm & a)
{
  bool bruch = isOn(SW_RATIONAL);
  Off (SW_RATIONAL);

  CanonicalForm dummy(1);
  CanonicalForm dummy2;

  CFIterator F =a;

  for ( ; F.hasTerms(); F++)
  {
    if (F.coeff().inBaseDomain())
    {
      dummy2 = gcd(dummy,F.coeff().den());
      dummy = dummy * F.coeff().den()/dummy2;
    }
    else
    {
      dummy2 = MyDen(F.coeff());
      dummy = dummy*dummy2/gcd(dummy,dummy2);
    }
  }
  if (bruch)
    On (SW_RATIONAL);
  else
    Off(SW_RATIONAL);
  return dummy;
}

/* Liefert die normierte Canonische Form a zurück, wenn LC(a) kein Nullteiler in Characteristic p ist */
/* sonst -1*/
CanonicalForm MyMonic(const CanonicalForm & a, const CanonicalForm & r, const int & l)
{
  bool bruch = isOn(SW_RATIONAL);
  int zaehler;
  int Level = l;
  CanonicalForm dummy, dummy1, dummy2;
  CanonicalForm g = a;
  CanonicalForm p = r;

  On (SW_RATIONAL);

  if (Level == g.level())
  {
    dummy = 1/g.LC();
  }
  else
  {
    dummy = 1/g;
  }
  dummy1 = MyDen(dummy);
  dummy2 =dummy1;
  zaehler =1;

  Off (SW_RATIONAL);

  while ((mod(dummy2,p) != 1) && (mod(dummy2,p) !=0))
  {
    dummy2 = dummy2*dummy1;
    dummy2 =mod(dummy2,p);
    zaehler++;
  }
  if (mod(dummy2,p).isZero())
  {
    if (bruch)
      On (SW_RATIONAL);
    else
      Off(SW_RATIONAL);
    return -1;
  }
  else
  {
    zaehler--;
    dummy2 = power(dummy1,zaehler);
    dummy2 = mod(dummy2,p);
    dummy*= dummy1;
    dummy*= dummy2;
    g =mod(g*dummy,p);
    if (bruch)
      On (SW_RATIONAL);
    else
      Off(SW_RATIONAL);
    return g;
  }
}



/*Berechnet den ggT der Formen a und b in Characteristic p*/

CanonicalForm MyGCDlocal (const CanonicalForm & a, const CanonicalForm & b, const CanonicalForm & r, const int & l)
{
  bool bruch = isOn(SW_RATIONAL);
  Off(SW_RATIONAL);
  CanonicalForm Rest, Result;

  CanonicalForm f=a;
  CanonicalForm g=b;
  CanonicalForm p=r;
  int Level =l;
  f = mod(f,p);
  g = mod(g,p);

  Rest=g;

  while (!Rest.isZero())
  {
    g = MyMonic(g,p,Level);

    if (g == -1)
    {
      if (bruch)
        On(SW_RATIONAL);
      else
        Off(SW_RATIONAL);
      return -1;
    }
    else
    {
      Result =g;
      Rest = f%g;
      f = g;
      g = Rest;
      f =mod(f,p);
      g =mod(g,p);
      Rest =mod(Rest,p);
    }
  }
  if (bruch)
    On(SW_RATIONAL);
   else
    Off(SW_RATIONAL);
  return Result;
}

/* Chinese Remaindering für a mod m und b mod l */

CanonicalForm MyChiRem(const CanonicalForm & a,const CanonicalForm & m,const CanonicalForm & b,const CanonicalForm & l)
{
 bool bruch = isOn(SW_RATIONAL);

 CanonicalForm u,v,Runner;
 CanonicalForm Result(0);
 CanonicalForm LeadTerm;
 CanonicalForm x1=a;
 CanonicalForm m1 = m;
 CanonicalForm x2=b;
 CanonicalForm m2 = l;

 while (!x1.isZero() || !x2.isZero())
 {
  if (x1.degree() > x2.degree())
  {
    LeadTerm = power(x1.mvar(),x1.degree());
    u = x1.LC()*LeadTerm;
    v = 0;
    x1 = x1-u;
  }
  else
  {
    if (x1.degree() < x2.degree())
    {
     u = 0;
     LeadTerm = power(x2.mvar(),x2.degree());
     v = x2.LC()*LeadTerm;
     x2 = x2-v;
    }
    else
    {
     if (x1.degree() == x2.degree())
     {
      LeadTerm = power(x2.mvar(),x2.degree());
      u = x1.LC()*LeadTerm;
      v = x2.LC()*LeadTerm;
      x1 = x1-u;
      x2 = x2-v;
     }
    }
  }

  if (u.LC().inBaseDomain() && v.LC().inBaseDomain())
  {
    Runner = u.LC();
    Off(SW_RATIONAL);
    while(mod(Runner,m2) !=v.LC())
    {
      Runner = Runner+m1;
    }

    Result = Result+Runner*LeadTerm;

  }
  else
  {
   Result = Result + MyChiRem(u.LC(),m1, v.LC(), m2)*LeadTerm;
  }
 }
if (bruch)
       On(SW_RATIONAL);
      else
       Off(SW_RATIONAL);
return Result;
}

/*Rational Rekonstruction für a mod b*/

CanonicalForm MyRatRed(const CanonicalForm & a,const CanonicalForm & b)
{
 bool bruch = isOn(SW_RATIONAL);

 CanonicalForm f,dummy,dummy1,dummy2, Wurzel;
 CanonicalForm  q,u0,u1,v0,v1;
 CanonicalForm Result(0);

 CFIterator F =a;

 for ( ; F.hasTerms(); F++)
   {
    if (F.coeff().inBaseDomain())
    {
     Wurzel = sqrt(b);
     u0 =b;
     u1= F.coeff();
     v1 = 1;
     v0 = 0;

     int i=0;

     while(!(u1<Wurzel))
      {
       Off(SW_RATIONAL);
       q=u0/u1;
       dummy = u0-q*u1;
       u0=u1;
       u1=dummy;
       dummy = v0+q*v1;
       v0=v1;
       v1=dummy;
       i++;
      }
      f = -1;

      On(SW_RATIONAL);
      dummy2 = u1/v1;

      f = power(f,i)*dummy2;

      if (f.isZero())
      {
       Off(SW_RATIONAL);

       if (!mod(F.coeff(),b).isZero())
       {
        if (bruch)
         On(SW_RATIONAL);
        else
         Off(SW_RATIONAL);
        return -1;
       }

      }
      Result = Result+f*power(a.mvar(),F.exp());

    }
    else
    {
     dummy1 = MyRatRed(F.coeff(),b);
     if (dummy1 == -1)
     {
      if (bruch)
       On(SW_RATIONAL);
      else
       Off(SW_RATIONAL);
      return -1;
     }
     else
     Result = Result + dummy1*power(a.mvar(),F.exp());
    }
  }
if (bruch)
       On(SW_RATIONAL);
      else
       Off(SW_RATIONAL);
return Result;
}

/*Berechnet lokale ggT's der Formen a und b und liftet sie wieder hoch*/

CanonicalForm MyGCDmod( const CanonicalForm & a,const CanonicalForm & b)
{
 bool bruch = isOn(SW_RATIONAL);
 // cout << "enter MyGCD mit a= "<<a<<endl;
 // cout << "und b= "<<b<<endl;
CanonicalForm LeadA, LeadB;
CanonicalForm Kandidat,Kandidat1;
CanonicalForm f,g, Result;
CanonicalForm NennerA =1;
CanonicalForm NennerB=1;
CanonicalForm ZahlerA=1;
CanonicalForm ZahlerB=1;
int treffer = 0;
int Level;
CanonicalForm Modulo;
int i = 0;
bool TryAgain = 1;
int Primes[1228];



for (int i = 0;i <1228;i++)
{
 Primes[i]=cf_getPrime(i+1);

}
Level=a.level();

if (a.degree() > b.degree())
 {
 f = a;
        g = b;
 }
 else
 {
 g = a;
        f = b;
 }

 if (g.isZero())
 {
  if (f.isZero()) return CanonicalForm(1);
  return f/f.LC();
 }

 NennerA = MyDen(f);
 NennerB = MyDen(g);

 f = f*NennerA;
 g = g*NennerB;

 ZahlerA = MyNum(f);
 ZahlerB = MyNum(g);

 f=f/ZahlerA;
 g=g/ZahlerB;

 LeadA = f.LC();
 while (!LeadA.inBaseDomain())
 {
    LeadA =LeadA.LC();
 }
 LeadB = g.LC();
 while (!LeadB.inBaseDomain())
 {
    LeadB =LeadB.LC();
 }

 Off (SW_RATIONAL);


 while (TryAgain && i < 1228)
 {

  CanonicalForm p(Primes[i]);
  // cout << "p: "<<p<<endl;
  i++;
  if ( (mod(LeadA,p) != 0) && (mod(LeadB,p) != 0))
  {
    Result = MyGCDlocal(f,g,p,Level);

    if (Result !=-1)
    {

      if (Result == 1)
      {
        if (bruch)
         On(SW_RATIONAL);
        else
         Off(SW_RATIONAL);
        return Result;
      }
      else
      {
        if (treffer == 0 || Kandidat.degree() > Result.degree())
        {
         treffer = 1;
         Kandidat = Result;
         Modulo = p;
        }
        else
        {
         if (Kandidat.degree() == Result.degree())
         {
          Kandidat = MyChiRem(Kandidat,Modulo,Result,p);
          Modulo = Modulo*p;
          treffer++;
         }
      }
      if (mod(treffer,4) ==1)
      {
        Kandidat1=MyRatRed(Kandidat, Modulo);


        if (Kandidat1 !=-1)
        {
            Off(SW_RATIONAL);
            if (mod(f,Kandidat1) == 0 && mod(g,Kandidat1) == 0)
            {
             break;
            }

        }
      }


     }
   }
  }
  else
  {

  }

 }

 if (bruch)
       On(SW_RATIONAL);
      else
       Off(SW_RATIONAL);
 return Kandidat1;
}

/* Berechnet die Norm eines Form h über zwei Körpererweiterungen und faktorisiert sie*/

CFFList FactorizeNorm (const CanonicalForm & h, const int & i )
{
 bool bruch = isOn(SW_RATIONAL);
 if (i ==0)
 { return factorize(h);
 }


 CanonicalForm g =h;

  Variable x =g.mvar();

 int AnzExt = i;  // Über welcher Erweiterung arbeite ich gerade ...
 Variable l(-AnzExt);           //... und welche algebr. Variable gehört dazu ?
 Variable y('_');


 CanonicalForm MiPo, Norm, NormAbl, Factor_Norm,dummy1, dummy2, Nenner,LeaC;

 CFFList Result;
 CFFList dummy;

 bool is = true;

 int k = 0;
 g = g(y,l);                    //die algeb. Variable wird durch eine Polynomvariable ersetzt
 MiPo = getMipo(l,y);

 Norm = resultant(MiPo,g,y);   //norm von g als Funk. in x und y (l->y)  bzgl y
 NormAbl = Norm.deriv();
 // Off(SW_RATIONAL);
 is = !MyGCDmod(Norm,NormAbl).inBaseDomain(); //ist die Norm quadratfrei ?
 while (is)
  {
    k++;
    CanonicalForm t = g;
                  t = t(x-k*y,x);        //wenn nicht, wird g gestört und die neue Norm berechnet

    On(SW_RATIONAL);
    Norm = resultant(MiPo,t,y);  //Problem tritt hier auf, bei AnzExt = 1
    Off(SW_RATIONAL);
    NormAbl = Norm.deriv();
    is = ! MyGCDmod(Norm,NormAbl).inBaseDomain();
    //cout << "ggt der Norm: "<< MyGCDmod(Norm,NormAbl)<<endl;
  }
  AnzExt--;
  if (bruch)
   On(SW_RATIONAL);
  else
   Off(SW_RATIONAL);
  if (AnzExt == 0)  //sind alle Erweiterungen abgearbeitet...
   {
    Result = factorize(Norm);        //... wird die Norm Faktorisiert
   }
  else
   {
    Result = FactorizeNorm(Norm, AnzExt);  //wenn nicht, kommt die nächste erweiterung dran
   }
   CFFListIterator J=Result;
   for ( ; J.hasItem(); J++)
    { 
     Factor_Norm = J.getItem().factor();
     Factor_Norm = Factor_Norm(x+k*l,x);   // die Störungen werden rückgänig gemacht
     dummy.append(CFFactor(Factor_Norm));
    }
 return dummy;
}


/* Bereitet die Form h vor, ruft FactorizeNorm(h) auf und rekonstruiert daraus die
Faktoren von h */

CFFList MyFactorize(const CanonicalForm & h)
{
 bool bruch = isOn(SW_RATIONAL);

 CanonicalForm g = h;
 CFFList FacNorm, Result;  // Faktorisierung der Norm und das Ergebniss
 CanonicalForm Abl_g, LeaCoeff_g, normiert_g, g_quadrat ; //Ableitung, führender Koeff. und Normierung von g
 CanonicalForm Norm,NormAbl;
 CanonicalForm Factor_Norm, Fac;
 CanonicalForm dummy, g_origin;
 CanonicalForm Nenner,warte;

 Variable x = g.mvar();

 int exp =0;
 int DegAlt, DegNeu;
 On(SW_RATIONAL);

 /* Initzialisierung, faktorisiert wird CF g */
 g_origin = g;
 LeaCoeff_g = g.LC();
 //g /=LeaCoeff_g;
 Nenner = MyDen(g);
 g *= power(Nenner, g.degree());
 g *= power(LeaCoeff_g,g.degree()-1);
 g = g(x/(Nenner*LeaCoeff_g),x);
 Abl_g = g.deriv();
 DegAlt = g.degree();
 g_quadrat=g;
 g /= MyGCDmod(g,Abl_g);        // g wird quadratfrei gemacht
 DegNeu = g.degree();

 //g = g/LeaCoeff_g;          // und normiert
 //CurrentExtension();
 FacNorm = FactorizeNorm(g,ExtensionLevel());
 CFFListIterator J=FacNorm;
 J.lastItem();
 // g = g*MyDen(g);
 //
  g = h ;

 for ( ; J.hasItem(); J--)   // Iteration über die Faktoren der Norm
  {
          Factor_Norm = J.getItem().factor();

   Fac =  MyGCDmod(g,Factor_Norm);       //Ergebniss wird hochgeliftet

          Fac = Fac/Fac.LC();                        // und normiert

/* Ermittlung der Exponenten der einzelnen Faktoren */

   exp = 1;              // für den FaKtor mit Grad 0
   dummy = g_quadrat;

          if (!Fac.inBaseDomain())      // echter Faktor ?
         {
  exp = 0;
         while ( 0==dummy%Fac && !dummy.inBaseDomain()) // Wie oft Teilt der Faktor das Polynom ?
           {
      dummy =dummy/Fac;
             exp++;
           }
  Fac = Fac(x*(Nenner*LeaCoeff_g),x);

    Fac /= Fac.LC();
   }

         else
            {
             Fac *= LeaCoeff_g;
             g *= LeaCoeff_g;
            }

          g /=power(Fac,exp);

          Result.append(CFFactor( Fac, exp ));    // Faktor wird an Result gehängt
  }
  if (bruch)
       On(SW_RATIONAL);
      else
       Off(SW_RATIONAL);
  return Result;    // und zurückgegeben
}

CFFList AbsFactorize(const CanonicalForm  & a)
{
CanonicalForm f = a;
CanonicalForm Factor, NewFactor,dummy3, Nenner,LeadC;
CanonicalForm Coeff=f.LC();

Variable x =a.mvar();
CFFList dummy, dummy2;
CFFList result, empty;
empty.append(CFFactor(1,1));
bool NewRoot = false;
bool fertig;

LeadC = f.LC();
f *= power(LeadC, f.degree()-1);
Nenner = MyDen(f);
f *= power(Nenner, f.degree());
f = f(x/(Nenner*LeadC), x);
result = MyFactorize(f);

CFFListIterator L = result;
  fertig = true;
  for(; L.hasItem();L++)
     {
      if (L.getItem().factor().degree() >1)
       {
        fertig = false;
       }
     }

while(!fertig)
{
 dummy = result;
 CFFListIterator J = dummy;
 result = empty;
for ( ; J.hasItem(); J++)   // Iteration über die Faktoren der Norm
  {
          Factor = J.getItem().factor();

          if (Factor.degree() != 0 && Factor.degree() != 1 && !NewRoot)
           {
            Reduce(false);
            Variable u = rootOf(Factor);
            Reduce(true);
            NewRoot = true;
            result.append(CFFactor((x-u),1));
            Factor /= (x-u);
           }


           if (Factor.degree() != 0 && Factor.degree() != 1 && NewRoot)
           {
            dummy2 = MyFactorize(Factor);

            CFFListIterator H = dummy2;
            for ( ; H.hasItem(); H++)   // Iteration über die Faktoren der Norm
              {
               NewFactor = H.getItem().factor();
               if (!NewFactor.inBaseDomain())
                 {
                  result.append(CFFactor(NewFactor, H.getItem().exp()*J.getItem().exp()));
                 }
        else
          {
    Coeff *=H.getItem().factor();
   }
              }
           }
          if ( Factor.degree() == 0)
           {
            Coeff *=Factor;
           }
          if( Factor.degree() == 1)
           {
            result.append(CFFactor(Factor,J.getItem().exp()));
           }
  }
  NewRoot = false;
  CFFListIterator K = result;
  fertig = true;

  for(; K.hasItem();K++)
     {

      if (K.getItem().factor().degree() >1)
       {
        fertig = false;
       }
     }
 }
CFFList result2;
//result2.append(CFFactor(Coeff));
CFFListIterator K = result;
for(; K.hasItem();K++)
     {
      dummy3 = K.getItem().factor();
      if (dummy3.degree() == 0)
 { dummy3 *= Coeff;
  }
      else
       {
      dummy3 = dummy3(x*Nenner*LeadC,x);
 dummy3 /= dummy3.LC();
 }

      result2.append(CFFactor(dummy3,K.getItem().exp()));
     }
return result2;
}


//
//
CanonicalForm Bigcd( const CanonicalForm& f, const CanonicalForm& g)
{


 if( f.level() < 0 ) return 1;
 if( g.level() < 0 ) return 1;

    CFArray A;

    int i=0;
    int r;

    Variable x = f.mvar();
    Variable y = g.mvar();

    // Wahl als Hauptvariable ?
    //
    if( x.level() >= y.level() ) x = y;

    CanonicalForm Cf, Cg, gamma, c,T;
    CanonicalForm F=f;
    CanonicalForm G=g;

    Cf = MyContent(f,x); //changed
    Cg = MyContent(g,x); //changed
    F = F/Cf;
    G = G/Cg;
    gamma = MyGCDmod( LC(F,x), LC(G,x) );

    A = subResChain(F,G,x);

    c = MyGCDmod( Cf, Cg );

    r = A.size();

    while( A[i].isZero() ) i++;

    F = A[i];

    if( degree(F,x) == 0 )  
  if( c.level() < 0 ) return 1; else return c; 

    F = gamma*F/LC(F, x);
    F = F/content(F,x);

    F = c*F;

    c = F.LC();

   while( c.level()>0 ) c = c.LC();

    F=F/c;

 if( F.level() < 0 ) return 1;

    return F;
}




CanonicalForm MYGCD( const CanonicalForm& f, const CanonicalForm& g)
{

 // FIX ME: CONSTANT FIELD
 //
 //
 //
 //
 //
 if( f.level() < 0 && g.level() < 0) return 1;
 if( (f.level() < 0 && g.level() > 0) ||
     (f.level() > 0 && g.level() <0 )     ) return 1;

 int i;

 CFList L;
 for (i=1; i<= level(f); i++)
  if( f != f(0,i) )
  L.append(i);

 int nvf = L.length();

 for (i=1; i<= level(g); i++)
  if( g != g(0,i) )
     L.append(i);
  
 int nvg = L.length();

 

 if( f.level() < 0 && g.level() < 0 )  { ;
  return 1; }
    
 CFArray A;
    
 i=0;
    int r;

    Variable x = f.mvar();
    Variable y = g.mvar();

    // Wahl als Hauptvariable ?
    //
    if( x.level() >= y.level() ) x = y;

    CanonicalForm Cf, Cg, gamma, c,T;
    CanonicalForm F=f;
    CanonicalForm G=g;


    Cf = MyContent(f,x);
    Cg = MyContent(g,x);
    F = F/Cf;
    G = G/Cg;

 if( nvf <= 1 && nvg <=1 )
 {
  gamma = MyGCDmod( LC(F,x), LC(G,x) ); 
  c = MyGCDmod( Cf, Cg );
 }
 else
 {
   gamma = MYGCD( LC(F,x), LC(G,x) );
  c = MYGCD( Cf, Cg );
 }
    A = subResChain(F,G,x);

    r = A.size();

    while( A[i].isZero() ) i++;

    F = A[i];

    if( degree(F,x) == 0 )  
  if( c.level() < 0 ) return 1; else return c; 

    F = gamma*F/LC(F, x);
    F = F/MyContent(F,x);

    F = c*F;

    c = F.LC();

   while( c.level()>0 ) c = c.LC();

    F=F/c;

 //if( F.level() < 0 ) return 1;

    return F;
}



CFFList Mysqrfree_local( const CanonicalForm& F, const Variable& v)
{
 int i=1;
 CanonicalForm f=F;
 CanonicalForm g, qi, fp, wp, temp1, temp2, temp3, temp4,  pA;
 CFFList L;


 g = MyContent(f,v);

 if( g != 1 )
 L.append( CFFactor(g,1) );

 pA = f/g;

 fp = deriv( pA, v);

 temp1 = MYGCD( pA, fp );

 if( temp1 == 1 ){ L.append( CFFactor(pA,1) ); return L; }
 else
 {
  temp2 = pA/temp1;
  temp3 = fp/temp1;
  wp = deriv(temp2,v);
  temp4 = temp3 - wp;

  while( !temp4.isZero() )
  {
   CanonicalForm qi = MYGCD( temp2, temp4);
   if( qi != 1 ) L.append( CFFactor( qi, i ) ); 
   i++;
   temp2 = temp2/qi;
   temp3 = temp4/qi;
   temp4 = temp3-deriv(temp2, v);
  }

 if( temp2 != 1 ) L.append( CFFactor( temp2, i ) );

 }

 return L;
}

CFFList Mysqrfree( const CanonicalForm& F )
{
 CFFList L, M, V;
 CFFList N;
 CanonicalForm vars=getVars(F);
 Variable v;
 CanonicalForm s;
 bool b;

 L.append( CFFactor(F,1) );

 int n = F.level();
 int *vrs = new int[n+1];
 for ( int i = 0; i <= n; i++ ) vars[i] = 0;
 for ( CFIterator I = F; I.hasTerms(); ++I ) fillVarsRec( I.coeff(), vrs );

 N.append( CFFactor(F,1) ); 

 int i = n+1;

 while( i >= 0 )
 {
  b = 0; 

  if( i == 0 ){  v = mvar(F); b=1 ;}
  else
  if( vrs[i] != 0 ){ b=1; v= Variable(i);} 
  if( vrs[i] == 0 )  i--; 

  if( b )
  { 
   for( CFFListIterator J = L; J.hasItem(); J++ )
   {
    M = Mysqrfree_local(  J.getItem().factor() , v );

    for( CFFListIterator K = M; K.hasItem(); K++ )
    {
     if( K.getItem().factor().level() > 0 )
      {
    N.append( CFFactor( K.getItem().factor(), K.getItem().exp()+J.getItem().exp()-1  )); }
    }
    N.removeFirst();
   }
   if( N.length() == L.length() ) i -= 1;
   L=N; 
  }
 }

  return L;

}
#endif
