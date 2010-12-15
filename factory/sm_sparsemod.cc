/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// sm_sparsemod.cc - zippel's sparse modular gcd.
//
// Contributed by Marion Bruder <bruder@math.uni-sb.de>.
//
// Used by: cf_gcf.cc
//
//}}}

#include <config.h>

#include "assert.h"
#include "debug.h"

#include "canonicalform.h"
#include "cf_map.h"
#include "cf_reval.h"
#include "cf_irred.h"
#include "cf_iter.h"
#include "cf_primes.h"
#include "cf_random.h"
#include "fac_util.h"
#include "cf_algorithm.h"
#include "sm_util.h"
#include "sm_sparsemod.h"
#include "templates/ftmpl_array.h"
#include "templates/ftmpl_functions.h"

static CanonicalForm
smodgcd( const CanonicalForm & u, const CanonicalForm & v, const CanonicalForm & lcggt, const REvaluation & alpha, CFRandom & gen, int CHAR, const Variable & extension )
{
  DEBOUTLN( cerr, " ALPHA = " << alpha );

  DEBOUTLN( cerr, " u = " << u );
  DEBOUTLN( cerr, " v = " << v );

  //    diverse Fallunterscheidungen  //

  if ( u.isZero() )
    return v;
  else if ( v.isZero() )
    return u;
  else if ( u.inBaseDomain() )
    if ( v.inBaseDomain() )
      return gcd( u, v );
    else
      return gcd( u, icontent( u ) );
  else if ( v.inBaseDomain() )
    return gcd( icontent( u ), v);
  else if ( u.isUnivariate() )
    if ( v.isUnivariate() )
      return gcd( u, v );
    else
      return gcd( v, u );

  //   u und v Polynome in levU - Variablen x1, ..., xlevU
  //   mit den gleichen Variablen, welches in sparsemod gesichert wurde

  int levU = level( u );
  CFArray G( 1, levU );
  Variable x( 1 );
  CanonicalForm alphau = alpha( u, 2, levU );
  CanonicalForm alphav = alpha( v, 2, levU );

  G[1] = gcd( alphau, alphav );       // univariater gcd berechnen in x1

  DEBOUTLN( cerr, " G[1] ohne lc = " << G[1] );
  //  Leitkoeffizienten auf univariaten gcd aufsetzen   //

  if( alpha(lcggt) != 0 && degree(G[1]) != 0 )
  {
    //G[1] =( alpha( lcggt ))*( ( 1/lc( G[1] ) ) * G[1] );
    G[1] =( alpha( lcggt ))*( ( 1/Leitkoeffizient( G[1] ) ) * G[1] );
  }

  if ( degree( G[1]) < 0 )
  {
   return 1 ;
  }


  if ( degree( G[1] ) == 0 )
  {
    // zwei Auswertungen um sicherzugehen, dass gcd Eins ist!
    return 1;
  }

  CFIterator J = G[ 1 ];
  CFArray g( 1, degree( G[ 1 ]) + 1 );
  int * emma = new int[ degree( G[1] ) + 2  ];
  int s, m, i = 1;

  //  g[i] enthaelt die momentan berechneten Monome bzgl x^emma[i] //

  while ( J.hasTerms() )
  {
    g[ i ] = J.coeff() ;
    emma[ i ] = J.exp();
    J++;
    i++;
  }

  m = i-1;           /// Anzahl der Terme bzgl x1
  int * n = new int[ m+1  ];
  CFArray U( 2, levU ), V( 2, levU );

  int  N, d   ;
  int k, j, zip ;

  // fuer jede Variabel interpolieren, um g[s] zu erhalten  //

  DEBOUTLN( cerr, " unigcd mit alpha mit lc = " << G[1] );

  for( s = 2 ;s <= levU ; s++ )
  {
    U[ s ] = alpha( u, s+1, levU );  // s Variablen stehen lassen, fuer
    V[ s ] = alpha( v, s+1, levU );  // den Rest alpha einsetzen
    d = tmin( degree( U[ s ], s ), degree( V[ s ], s ));

    DEBOUTLN( cerr, " U["<< s << "] = " << U[s] );
    DEBOUTLN( cerr, " V["<< s << "] = " << V[s] );
    DEBOUTLN( cerr, " d = " << d );

    for ( i = 1; i <= m ; i++ )
    {
      // Anzahl der Monome berechnen pro gi liefert das Skeletonpolynom //
      n[ i ] = countmonome( g[ i ] );
      //cout << "n["<<i<<"] = "<< n[i] << endl;
    }

    for ( i = 1; i <= m    ; i++ )
    {
      if ( i ==1 )
        N = n[i];
      else
      {
        if ( n[i]> N )
          N = n[i];
      }
    }

    //      int tau[d+1][m+1][N+1];               /// keine Integers !!!
    typedef CanonicalForm** CF_ptr_ptr;
    typedef CanonicalForm* CF_ptr;

    CanonicalForm ***tau = new CF_ptr_ptr[m+1];
    for ( i = 1; i <= m; i++ )
    {
      tau[i] = new CF_ptr[d+1];
      for ( j = 1; j <= d; j++ )
        tau[i][j] = new CanonicalForm[N+1];
    }

    CFArray beta( 1, d );

    for ( i = 1; i <= d ; i++ )
    {
      beta[ i ] =  gen.generate();   // verschiedene Elemente aus Zp
      //  cout << "  beta["<<i << "] = " << beta[i];
    }

    Array<REvaluation> xi( 1, N );
    REvaluation a( 2, s-1, gen  );

    for ( i = 1 ; i <= N ; i++ )
    {
      a.nextpoint();
      xi[ i ] = a;//REvaluation( 2, s-1, gen );
      // cout << "  xi["<<i<<"] = "<< xi[i];
    }

    //CFArray T(1, d*N );

    if ( d == 0 )
    {
      ASSERT( 0, "alpha bad point! try some other gcd algorithm" );
      return gcd(u, v);
    }

    CFMatrix T( d, N ) ;  // diese Moeglichkeit laeuft!!!
                    //help = Koeff( T( j, k ), emma[ i ] );
                    //tau[i][j][k] = help; //.intval();

    for ( j = 1 ; j <= d ; j++ ) // jedesmal andere REvaluation??
    {
      for ( k = 1 ; k <= N ; k++ )
      {
        CanonicalForm zwischenu, zwischenv, help, nfa ;

        zwischenu = U[ s ]( beta[ j ], s );
        zwischenv = V[ s ]( beta[ j ], s );

        T( j, k) = gcd ( xi[k]( zwischenu, 2, s-1 ), xi[k]( zwischenv, 2, s-1 ));

        nfa = lcggt( beta[j], s );
        nfa =  alpha( nfa, s+1, levU );

        //T(j, k ) = (xi[k]( nfa, 2, s-1 ))*((1/lc(T( j, k ))) * T( j, k ));
        T(j, k ) = (xi[k]( nfa, 2, s-1 ))*((1/Leitkoeffizient(T( j, k ))) * T( j, k ));

        //cout <<"T("<<j<<", "<< k <<") = " << T(j, k) << endl;

        for ( i = 1 ; i <= m ; i++ )
        {
          // diese Moeglichkeit laeuft!!!
          //help = Koeff( T( j, k ), emma[ i ] );
          //tau[i][j][k] = help; //.intval();
          if ( T( j, k).inBaseDomain() )
          {
            if ( emma[i] == 0 )
              tau[i][j][k] = T( j, k );
            else
              tau[i][j][k] =  0 ;
          }
          else
          {
            tau[i][j][k] = T(j, k)[emma[i]];
          }
        }
      }

      CFMatrix h( m, d );

      for ( i = 1; i <= m ; i++ )
      {
        for ( j = 1 ; j <= d ; j++ )
        {
          zip = n[i] +1;
          CanonicalForm * zwischen = new CanonicalForm[ zip ];//n[i]+1 ];

          for ( k = 1 ; k <= n[i] ; k++ )
          {
            zwischen[ k ] = tau[i][j][k];
          }

          //cout <<" werte fuer sinterpol : " << endl;
          //cout <<" g["<<i<<"] = " << g[i] << " xi = " << xi << endl;
          //cout << " zwischen = " << zwischen << " ni = " << n[i]<<endl;
          h[ i ][ j ] = sinterpol( g[i], xi, zwischen, n[i] );
          DEBOUTLN( cerr, " Ergebnis von sinterpol h["<<i<<"]["<<j<< "] = " << h[i][j] );
          delete [] zwischen;
        }
      }
      for ( i = 1 ; i <= m ; i++ )
      {
        CFArray zwitscher( 1, d );
        for ( j = 1 ; j <= d ; j++ )
        {
          zwitscher[ j ] = h[ i ][ j ];
        }

        DEBOUTLN( cerr, "Werte fuer dinterpol : " );
        DEBOUTLN( cerr, " d = " << d << " g["<<i<<"] = "<< g[i] );
        DEBOUTLN( cerr, " zwitscher = " << zwitscher );
        DEBOUTLN( cerr, " alpha["<<s<< "] = "<< alpha[s] << " beta = "<<beta << " n["<<i<<"] = " << n[i] );

        g[ i ] = dinterpol( d, g[i], zwitscher, alpha, s, beta, n[ i ], CHAR );
        DEBOUTLN( cerr, " Ergebnis von dinterpol g["<<i<<"] = " << g[i] );

      }

      for ( i = 1 ; i <= m ; i++ )
      {
        G[ s ] += g[ i ] * ( power( x, emma[i] ) );
      }
      DEBOUTLN( cerr, "G["<<s<<"] = " << G[s] );
      for ( i = 1; i <= m; i++ )
      {
        for ( j = 1; j <= d; j++ )
          delete [] tau[i][j];
        delete [] tau[i];
      }
      delete [] tau;
    }
  }
  delete [] emma;

  return G[ levU ];
}

// In sparsemod testen, ob G[levU] | u und G[levU] | v ///
static CanonicalForm
internalSparsemod( const CanonicalForm & F, const CanonicalForm & G )
{
  // On(SW_USE_EZGCD );
  On( SW_SYMMETRIC_FF );
  //cout << " in sparse " << endl;
  if( F.inCoeffDomain() &&  G.inCoeffDomain() )
  {
    return gcd( F, G );
  }

  CanonicalForm f, g, ff, gg, ggt, res, fmodp, gmodp ;
  int i, count = 10;

  //   COMPRESS    ///////////////////

  CFArray A(1, 2);
  A[1] = F;
  A[2] = G;
  CFMap M, N;
  compress(A, M, N);
  f = M(A[1]);
  g = M(A[2]);

  // POLYNOME PRIMITIV BZGL DER ERSTEN VARIABLE  /////

  CanonicalForm primif, primig, lcf, lcg, lcggt, lcggtmodp, result ;
  ff = content( f, Variable(1) );//contentsparse( f, 1  );
  gg = content( g, Variable(1) );//contentsparse( g, 1  );

  primif = f/ff;
  primig = g/gg;
  ggt = gcd( ff, gg );

  if( primif.inCoeffDomain() &&  primig.inCoeffDomain() )
  {
    return N( gcd( primif, primig ) ) * N( ggt );
  }

  // Variablen, die in beiden Polynomen auftreten /////

  int levis, m, levelprimif, levelprimig;
  int  ABFRAGE = 1 ;

  levelprimif = level( primif );
  levelprimig = level( primig );

  if ( levelprimif > levelprimig )
    levis = levelprimif;
  else
    levis = levelprimig;

  if ( levis < 0 )
    return N( gcd( primif, primig ) );

  int * varf = new int[levis];
  int * varg = new int[levis];
  int * schnitt = new int[levis];

  while ( ABFRAGE == 1 )
  {
    levelprimif = level(primif);
    levelprimig = level(primig);

    if ( levelprimif > levelprimig )
      levis = levelprimif;
    else
      levis = levelprimig;

    if ( levis < 0 )
      return N( gcd(primif, primig ));

    for( i = 1; i <= levis ; i++ )
    {
      if ( degree( primif, i ) != 0 )
        varf[i] = 1;
      else
        varf[i] = 0;
      if ( degree( primig, i ) != 0 )
        varg[i] = 1;
      else
        varg[i] = 0;
      if ( varg[i] == 1 && varf[i] == 1 )
        schnitt[i] = 1;
      else
        schnitt[i] = 0;
    }

    levelprimif = level(primif);
    levelprimig = level(primig);

    for ( m  = 1; m <= levis ; m++)
    {
      if ( schnitt[m] == 0 )
      {
        if ( varf[m] == 1)
        {
          primif = content( primif, m ); //contentsparse( primif, m  );
        }
        else
        {
          primig = content( primig, m ); //contentsparse( primig, m  );
        }
      }
    }

    if ( level( primif ) == level( primig ) )
      ABFRAGE = 0 ;

  }

  delete [] varf; delete [] varg; delete [] schnitt;

  //  Nochmal compress fuer den Fall, dass eine Variable rausfliegt //

  CFArray C(1, 2);
  C[1] = primif;
  C[2] = primig;
  CFMap MM, NN ;
  compress(C, MM, NN);
  primif = MM(C[1]);
  primig = MM(C[2]);

  if ( level( primif) != level( primig ) )
    ASSERT( 0, "this should n e v e r happen !!!!" );

  //cout << " primif = " << primif << endl;
  //cout << " primig = " << primig << endl;

  if( primif.inCoeffDomain() &&  primig.inCoeffDomain() )
  {
    return N( NN( gcd( primif, primig ) ) ) * N( ggt );
  }

  // erst hier Leitkoeffizienten updaten  /////////

  //if( primif.inCoeffDomain() || primif.isUnivariate

  lcf = LC(primif, 1 );
  lcg = LC(primig, 1 );
  lcggt = gcd ( lcf, lcg );

  //   BOUND BESTIMMEN fuer Charakteristik Null   /////////

  int CHAR = getCharacteristic(), deg  ;
  if ( CHAR < 10 )
    deg = 3; // Default Erweiterung bei kleiner Primzahl;
  else
    deg = 1;
  CanonicalForm B, L, Bound, lcF = Leitkoeffizient( primif); //lc( primif ) ;
  B =  2 * maxNorm( primif ) * maxNorm( g ) ;
  L = lcF ;
  Bound = abs( 2 * B * L + 1 );
  int length = 1;

  CFArray p( 1, 10 ) ;

  if( CHAR == 0 )
  {
    p[ 1 ]  = cf_getBigPrime( count );
    CanonicalForm q = p[ 1 ];

    while ( q < Bound )
    {
      count++;
      length++;
      p[ length ] = cf_getBigPrime( count );
      q     *= p[ length ];  //G[levU] = ( 1/lc(G[levU] ))* G[levU];// sinnig?
      //cout << " lcggt = " << lcggt << endl;
      //bool ja;
      //ja = fdivides( lcggt, lc( G[levU] ) );
      //cout << " ja = " << ja << endl;
      //cout << " vor Verlassen " << endl;
    }
  }
  else
  {
    //int q  = CHAR;
    //setCharacteristic( 0 );
    //CanonicalForm Bound2 = mapinto( Bound ) ;
    //cout << " Bound2 " << Bound2 << endl;
    //cout << " CHAR =  " << q << endl;

    // erstmal ohne Erweiterung !!!
    //deg = 3; // Default Erweiterung
    //q *= ( q * q ) ;cout << "q = " << q << endl;

    //Bestimme Grad der Koerpererweiterung voellig unnuetz!!!
    //while ( q < abs( Bound2 ) )
    //        {
    //  q *= CHAR;cout << " q = " << q << endl;
    //deg++;cout << " degchar = " << deg << endl;
    //cout << " in Graderhoehung? " << endl;

    //}
    //setCharacteristic( CHAR );
    //cerr << " JUHU " << endl;
  }

  //        ENDE BOUNDBESTIMMUNG       /////////////////

  FFRandom gen ;
  levelprimif = level( primif );
  REvaluation am( 2, levelprimif, FFRandom ( ));
  int k, help ;
  CFArray resultat( 1, length );//cout << " nach Resultat " << endl;
  CanonicalForm zwischen;
  Variable alpha1( levelprimif + 1 );
  ABFRAGE = 0;


  for ( i = 1 ; i <= 10; i++ )               // 10 Versuche mit sparsemod
  {
    if ( CHAR == 0 )
    {
      for( k = 1; k <= length ; k++)
      {
        help = p[ k ].intval();
        setCharacteristic( help );
        FFRandom mache;
        am = REvaluation( 2, levelprimif, mache  );
        am.nextpoint();
        fmodp  = mapinto( primif );
        gmodp = mapinto( primig );
        lcggtmodp = mapinto ( lcggt );

        zwischen = smodgcd( fmodp, gmodp, lcggtmodp, am, mache, CHAR, Variable( 1 ));
        // Variable ( 1 ) Interface fuer endliche Koerper

        // Char auf 0 wegen Chinese Remainder ////////

        setCharacteristic( 0 );

        resultat[ k ] = mapinto( zwischen );
      }

      ////////////////////////////////////////////////////////////
      // result[k] mod p[k] via Chinese Remainder zu Resultat //////
      // ueber Z hochziehen                                //////
      ////////////////////////////////////////////////////////////

      if( length != 1 )
        ChinesePoly( length, resultat, p, result );
      else
        result = resultat[1];

      CanonicalForm contentresult = content( result, 1 );

      if ( contentresult != 0 )
        res = result/contentresult;
      else
        res = result;

      if ( fdivides( res, primif ) && fdivides( res, primig ) )
        return  N(NN(res))*N(ggt) ;     /// compress rueckgaengig machen!
      else
      {

        // Start all over again ///

        count++;
        for ( k = 1; k <= length ; k++)
        {
          p[k] = cf_getBigPrime( count );
          count++;
        }
      }
    }
    else
    {
      // Fall Char != 0 ///
      // in algebraische Erweiterung vom Grad deg gehen //
      //cout << " IN CHAR != 0 " << endl;
      //cout << " degree = " << deg << endl;
      CanonicalForm minimalpoly;
      //cout << " vor mini " << endl;
      minimalpoly = find_irreducible( deg, gen, alpha1 );
      //cout << " nach mini " << endl;
      Variable alpha2 = rootOf( minimalpoly, 'a' ) ;
      AlgExtRandomF hallo( alpha2 );
      //cout << " vor am " << endl;
      REvaluation am (  2, levelprimif, hallo );
      //cout << " nach ma " << endl;
      am.nextpoint();
      //cout << "vor smodgcd " << endl;
      result = smodgcd( primif, primig, lcggt, am, hallo, CHAR, alpha2  );
      if ( result == 1 && ABFRAGE == 0)
      {
        // zwei Auswertungen fuer gcd Eins
        am.nextpoint();
        ABFRAGE = 1;
      }
      //CanonicalForm contentresult = contentsparse(  result, 1 );
      //zuerst mal auf Nummer sicher gehen ...
      else
      {
        CanonicalForm contentresult = content(  result, 1 );
        //cout << "RESULT = " << result << endl;
        if ( contentresult != 0 )
          res = result/contentresult;
        else
          res = result;

        if ( ( fdivides( res, primif )) && ( fdivides ( res, primig ) ))
        {
          // make monic ////
          res = (1/lc(res)) * res;
          // eventuell auch hier Leitkoeffizient anstatt lc ?

          return  N( NN( res ) ) * N( ggt ) ;
        }
        else
        {
          // Grad der Erweiterung sukzessive um eins erhoehen
          deg++;
          //cout << " deg = " << deg << endl;
          am.nextpoint();
          // nextpoint() unnoetig?
        }
      }
    }
  }


  //Fuer den Fall der unwahrscheinlichen Faelle, dass die Versuche
  //nicht ausreichen :

  ASSERT( 0, "sparsemod failed! try some other gcd algorithm" );
  if( CHAR == 0 )
    setCharacteristic( 0 );

  return 0;
}

CanonicalForm
sparsemod( const CanonicalForm & F, const CanonicalForm & G )
{
    Off( SW_USE_SPARSEMOD );
    CanonicalForm result = internalSparsemod( F, G );
    On( SW_USE_SPARSEMOD );
    return result;
}
