#include <coeffs/bigintmat.h>
#include "nforder.h"
#include <reporter/reporter.h>
#include<coeffs/numbers.h>
#include <coeffs/coeffs.h>
#include "Singular/ipid.h"




////////////////////////////////////
//// Konstruktoren/Destruktoren ////
////////////////////////////////////

/*________________0_______________*/
void nforder::init() {
  rc = 1;
  // Gibt es eine Multtable, so gibt es keine Baseorder
  baseorder = NULL;
  basis = NULL;
  // Discriminante wird erst berechnet, wenn sie benötigt wird
  discriminant = NULL;
  divisor = NULL;
  flags = 0;
  multtable = NULL;
  m_coeffs = NULL;
  setOneIsOne();
}

nforder::nforder(int dim, bigintmat **m,const coeffs q) {
  init();
  m_coeffs = q;
  dimension = dim;
  multtable = (bigintmat**)(omAlloc(dim*sizeof(bigintmat*)));
  for (int i=0; i<dim; i++) {
    multtable[i] = new bigintmat(m[i]);
  }
  basis = NULL;
  inv_basis = NULL;
}

nforder::nforder(nforder *o, bigintmat *base, number div, const coeffs q) {
   init();
   m_coeffs = q;
   basis = new bigintmat(base);
   //neue Ordnung erzeugen und übergebene Daten kopieren
   baseorder = o;
   o->ref_count_incref();
   //Gibt es eine Baseorder, brauchen wir keine Multtable. Könnte aber evtl. generiert werden
   multtable = NULL;
   divisor = n_Copy(div,basecoeffs());
   basis->simplifyContentDen(&divisor);
   dimension = o->getDim();
   discriminant = NULL;

   inv_basis = new bigintmat(base->rows(), base->rows(), q);
   inv_divisor = basis->pseudoinv(inv_basis);
   inv_basis->skalmult(divisor, q);
   inv_basis->simplifyContentDen(&inv_divisor);
}

nforder::nforder(nforder *o, int) {
  init();
  m_coeffs = o->basecoeffs();
  ::Print("copy called: %lx\n", (long unsigned int) m_coeffs);
  // Kopiert die Daten der übergebenen Ordnung auf die erzeugte
  if (o->discriminant)
    discriminant = n_Copy(o->discriminant,  basecoeffs());
  // get-Funktionen liefern immer nur Kopien der Attribute zurück
  dimension = o->getDim();
  multtable = (bigintmat **)omAlloc(dimension*sizeof(bigintmat*));
  if (!o->getMult(multtable)) {
    omFree(multtable);
    multtable = NULL;
  }
  baseorder = o->getBase();
  if (baseorder) baseorder->ref_count_incref();
  basis = o->getBasis();
  if (o->divisor)
    divisor = n_Copy(o->divisor,  basecoeffs());
  if (o->inv_basis) {
    inv_basis = new bigintmat(o->inv_basis);
    inv_divisor = n_Copy(o->inv_divisor, basecoeffs());
  }
}

void nforder::Write() {
  StringAppend("Order:\nof dimension %d and rc: %d\n", dimension, ref_count());
  if (discriminant && !n_IsZero(discriminant, m_coeffs)) {
    StringAppend("and discriminant: ");
    n_Write(discriminant, m_coeffs);
    StringAppend("\n");
  }
//  coeffs
  if (multtable) {
    StringAppend("Multiplication table:\n");
    for(int i=0; i<dimension; i++) {
      StringAppend("%d: ", i);
      multtable[i]->Write();
      StringAppendS("\n");
    }
  }

  if (baseorder) {
    StringAppendS("as extension of:");
    baseorder->Write();
    StringAppendS("with basis:\n");
    basis->Write();
    StringAppendS("and denominator: ");
    n_Write(divisor, m_coeffs);
    StringAppendS("\nwith inv_basis:\n");
    inv_basis->Write();
    StringAppendS("and inv_denominator: ");
    n_Write(inv_divisor, m_coeffs);
    StringAppendS("\n");
  }

  StringAppend("Flags: %lx\n", flags);
}

char * nforder::String() {
  StringSetS("");
  Write();
  return StringEndS();
}
void nforder::Print() {
  char * s = String();
  PrintS(s);
  PrintS("\n");
  omFree(s);
}
void nforder_delete(nforder* o) {
  if (o->ref_count_decref()>0) {
    return;
  }
  delete o;
}

nforder::~nforder() {
  if (multtable != NULL) {
    for (int i=0; i<dimension; i++)
      delete multtable[i];
    omFree(multtable);
  }
  else
  {
    // andernfalls werden baseorder und basis gelöscht
    nforder_delete (baseorder);
    delete basis;
    if (divisor) n_Delete(&divisor, basecoeffs());
    if (inv_basis) delete inv_basis;
    if (inv_divisor) n_Delete(&inv_divisor, basecoeffs());
  }
  if (discriminant) n_Delete(&discriminant, basecoeffs());
}

/*
////////////////////////////////////
//////// Private Funktionen ////////
///////////////////////////////////*/
/*_______________-1_______________ */
void nforder::calcdisc() {
  // Determinante von Spurmatrix ist die Discriminante
  if (discriminant) return;
  if (baseorder == NULL) {
    bigintmat *m = traceMatrix();
    discriminant = m->det();
    assume(discriminant);
    delete m;
  }
  else
  {
    number prod = n_Init(1, basecoeffs());
    number tmp, tmp2; //assumes that the basis is triangular!
    for (int i=1; i<=dimension; i++) {
      tmp2 = basis->view(i, i);
      tmp = n_Mult(prod, tmp2, basecoeffs());
      n_Delete(&prod, basecoeffs());
      prod = tmp;
    }
    baseorder->calcdisc();
    number disc = baseorder->viewDisc();
    assume(disc);
    number detquad = n_Mult(prod, prod, basis->basecoeffs());
    discriminant = n_Mult(disc, detquad, basecoeffs());

    for (int i=1; i<=2*dimension; i++) {
      tmp = n_Div(discriminant, divisor, basecoeffs());
      n_Delete(&discriminant, basecoeffs());
      discriminant = tmp;
    }
    n_Delete(&detquad, basis->basecoeffs());
  }
}

bigintmat *nforder::traceMatrix() {
  bigintmat *m = new bigintmat(dimension, dimension, basecoeffs());
  bigintmat *base1 = new bigintmat(dimension, 1, basecoeffs());
  bigintmat *base2 = new bigintmat(dimension, 1, basecoeffs());
  bigintmat *mm = new bigintmat(dimension, dimension, basecoeffs());
  number sum;

  for (int i=1; i<=dimension; i++) {
    for (int j=i; j<=dimension; j++) {
      // Berechnet Produkt von Basiselementen i und j und speichert es in base1
      basis_elt(base1, i);
      basis_elt(base2, j);
      elMult(base1, base2);
      // Schreibt Abbildungsmatrix der Multiplikation mit base1 in mm
      sum = elTrace(base1);
      m->set(i, j, sum, basecoeffs());
      if (i!=j)
        m->set(j, i, sum, basecoeffs());
      n_Delete(&sum, basecoeffs());
    }
  }
  delete base1;
  delete base2;
  delete mm;
  return m;
}
////////////////////////////////////
////// Öffentliche Funktionen //////
////////////////////////////////////

/*_____________+1_______________ */
number nforder::getDisc() {
  // Falls Discriminante bisher noch nicht berechnet wurde, berechne diese
  if (!discriminant || n_IsZero(discriminant, basecoeffs())) {
    calcdisc();
  }
  return n_Copy(discriminant, basecoeffs());
}

int nforder::getDim() {
  return dimension;
}

bigintmat *nforder::getBasis() {
  // Falls basis ein NULL-Pointer ist, liefere NULL zurück, andernfalls liefere eine Kopie von basis
  if (basis == NULL)
    return NULL;
  bigintmat *m = new bigintmat(basis); //wenn Fehler dann hier
  return m;
}
bigintmat *nforder::viewBasis() {
  if (basis == NULL)
    return NULL;
  return basis;
}
bool nforder::getMult(bigintmat **m) {
  // Falls multtable ein NULL-Pointer ist, liefere NULL zurück, andernfalls erzeuge neues Array of Matrix, kopiere die Matrizen aus multtable dort hinein, und gib es zurück
  if (multtable == NULL) {
    return false;
  }
  for (int i=0; i<dimension; i++)
  {
    m[i] = new bigintmat(multtable[i]);
  }
  return true;
}


number nforder::getDiv() {
  return n_Copy(divisor, basecoeffs());
}

nforder *nforder::getBase() {
  // returns the baseorder, if present. Does not incref the ref count.
  if (baseorder == NULL)
    return NULL;
  return baseorder;
}

nforder *nforder::simplify() {
  coeffs c = basecoeffs();
  if (!baseorder || !baseorder->baseorder) {
    ref_count_incref();
    return this;
  }
  nforder * O = baseorder;
  number den = n_Copy(divisor, c);
  bigintmat *bas = getBasis();
  while (O->baseorder) {
    bigintmat * b = bimMult(bas, O->viewBasis());
    n_InpMult(den, O->divisor, c);
    O =  O->baseorder;
    delete bas;
    bas = b;
  }
  nforder * res = new nforder(O, bas, den, c);
  if (discriminant)
    res->discriminant = n_Copy(discriminant, c);

  //TODO: copy multtable if we have it
  delete bas;
  n_Delete(&den, c);
  return res;
}

void nforder::elAdd(bigintmat *a, bigintmat *b) {
  if ((a->cols() != 1) || (a->rows() != dimension) || (b->cols() != 1) || (b->rows() != dimension)) {
    Werror("Error in elSub");
  }
  else {
    a->add(b);
  }
}


void nforder::elSub(bigintmat *a, bigintmat *b) {
  if ((a->cols() != 1) || (a->rows() != dimension) || (b->cols() != 1) || (b->rows() != dimension)) {
    // Kein Zeilenvektor der korrekten Größe
    Werror("Error in elSub");
  }
  else {
    a->sub(b);
  }
}

void nforder::elMult(bigintmat *a, bigintmat *b) {
  if ((a->cols() != 1) || (a->rows() != dimension) || (b->cols() != 1) || (b->rows() != dimension)) {
    // Kein Zeilenvektor der korrekten Größe
    Werror("Error in elMult");
  }

  coeffs C = a->basecoeffs();
  assume(C == b->basecoeffs());
  assume(C == this->basecoeffs());

  if (multtable != NULL) {
    // Multiplikation mit Hilfe von Multiplikationstabelle
    // Zu Grunde liegende Formel: Basis w_i; Für alpha = sum a_i*w_i und beta = sum b_i*w_i gilt:
    // alpha*beta = sum sum a_i*b_j*w_i*w_j
    bigintmat *sum = new bigintmat(dimension, 1, C);
    bigintmat *tmp = new bigintmat(dimension, 1, C);
    number ntmp;

    for (int i=1; i<=dimension; i++) {
      // Laufe mit i durch Basiselemente
      for (int j=1; j<=dimension; j++) {
        // Laufe mit j durch Basiselemente
        // Speichere Produkt von Basiselem. i mit Basiselem. j als Koeff.vektor in tmp

        multtable[i-1]->getcol(j, tmp);
        // Multipliziere ihn mit a[i] und b[j]
        ntmp = n_Mult(a->view(i, 1), b->view(j, 1), C);
        tmp->skalmult(ntmp, C);

        n_Delete(&ntmp, C);
        // und addiere alles auf
        sum->add(tmp);
      }
    }
    delete tmp;
    // Am Ende überschreibe a mit dem Ergebnis
    for (int i=0; i<dimension; i++)
      a->set(i+1, 1, sum->get(i+1, 1));
    delete sum;
  } else {
  // Multiplikation mit hilfe von baseorder:
    bigintmat *sumb = new bigintmat(dimension, 1, C);
  // Produkt von a (b) mit basis liefert Koeff-Vektor von a*divisor (b*divisor) in baseorder
    bimMult(basis, a, a);
    bimMult(basis, b, sumb);
    // Multipliziere Elemente in baseorder (und speichere in suma)
    baseorder->elMult(a, sumb);
    delete sumb;
    a->skaldiv(divisor);
    bimMult(inv_basis, a, a);
    a->skaldiv(inv_divisor);
    a->skaldiv(divisor);
  }
}


//TODO: compute the trace from the mult-table without
//      the explicit rep_mat
number nforder::elTrace(bigintmat *a)
{
  bigintmat * rep_mat = elRepMat(a);
  number t = rep_mat->trace();
  delete rep_mat;
  return t;
}

number nforder::elNorm(bigintmat *a)
{
  bigintmat * rep_mat = elRepMat(a);
  number n = rep_mat->det();
  delete rep_mat;
  return n;
}

bigintmat * nforder::elRepMat(bigintmat *a)
{
  bigintmat *b=new bigintmat(dimension, dimension, basecoeffs());
  multmap(a, b);
  return b;
}

//CF: TODO if multtable, then use lin. comb. of multtable
//rather than poducts. reduces complexity by a magnitude.
void nforder::multmap(bigintmat *a, bigintmat *m) {
  if ((m->cols() != dimension) || (m->rows() != dimension)) {
    Werror("Error in multmap");
    return;
  }
  bigintmat *bas = new bigintmat(dimension, 1, basecoeffs());
  for (int i=1; i<=dimension; i++) {
    // Durchläuft alle Basiselemente
    // Multipliziert i-tes Basiselement mit a
    basis_elt(bas, i);
    elMult(bas, a);
    // Schreibt Ergebnis in i-te Zeile der Matrix m. Am Ende ist m dann die Abbildungsmatrix der Multiplikation mit a
    m->setcol(i, bas);
  }
  delete bas;
}

/*________________1_______________ */
void basis_elt(bigintmat *m, int i) {
  if (((m->rows() == 1) && (i <= m->cols())) || ((m->cols() == 1) && (i <= m->rows()))) {
    // Falls m Zeilen- oder Spaltenvektor ist, setze alle Einträge auf 0 und Eintrag i auf 1 (Koeff-Vektor des i-ten Basiselements)
    number t1 = n_Init(0,m->basecoeffs());
    for (int j=0; ((j<m->rows()) || (j<m->cols())); j++) {
      m->set(j, t1);

    }
    n_Delete(&t1,m->basecoeffs());
    number t2 = n_Init(1,m->basecoeffs());
    m->set(i-1, t2);
    n_Delete(&t2,m->basecoeffs());
  }
  else
    Werror("Error in basis_elt. Not a vector.");
}

////////////////////////////////////
//////////// 2 Round 2 /////////////
////////////////////////////////////
//TODO: make the radical a proper ideal rather than a matrix
//  or at least, provide an ideal based interface
//  similar, expand the multring to deal with ideals

bigintmat *radicalmodpbase(nforder *o, number p, coeffs c) {

  number dimen = n_Init(o->getDim(), o->basecoeffs());
  int n = o->getDim();

  bigintmat *m, *bas;
  // Berechnet F_p-Basis von I_p/pI_p (Radical mod p)
  // Dazu:
  if (n_Greater(p, dimen, c)) {
    // Falls Primzahl größer gleich Dimension der Ordnung, so berechne Kern der Spurmatrix modulo p.
    // also works it p is no prime.
    m = o->traceMatrix();
    bas = new bigintmat(n, 1, o->basecoeffs());
  } else {
    // Sonst: Berechne Kern der Abbildung x -> x^(p^j) mod p, wobei j>0 mit p^j >= dimension
    int j = 1;
    // ex als number, oder reicht long long int?
    // Finde j von oben und berechne p^j
    number ex = n_Init(1, o->basecoeffs());
    number temp;
    while (n_Greater(dimen, ex, o->basecoeffs())) {
      temp = n_Mult(ex, p, o->basecoeffs());
      n_Delete(&ex, o->basecoeffs());
      ex = temp;
      j++;
    }

    // Berechne Abbildungsmatrix der oben genannten Abbildung und speichere diese in m (genauere Erklärung dazu: Siehe multmap())
    m = new bigintmat(n, n, o->basecoeffs());
    bas = new bigintmat(n, 1, o->basecoeffs());
    bigintmat *prod = new bigintmat(n, 1, o->basecoeffs());

    number klauf;
    number eins = n_Init(1, o->basecoeffs());

    for (int i=1; i<=n; i++) {
      basis_elt(bas, i);
      prod->copy(bas);
      klauf = n_Init(1, o->basecoeffs());
      for (; n_Greater(ex, klauf, o->basecoeffs());) {
        o->elMult(prod, bas);
        prod->mod(p, c);
        temp = n_Add(klauf, eins, o->basecoeffs());
        n_Delete(&klauf, o->basecoeffs());
        klauf = temp;
      }
      n_Delete(&klauf, o->basecoeffs());
      m->setcol(i, prod);
    }

    delete prod;
    n_Delete(&ex, o->basecoeffs());
    n_Delete(&eins, o->basecoeffs());

  }

  bigintmat *kbase = new bigintmat(n, n, o->basecoeffs());

  // Speichere Basiselemente von Kern der Matrix m (Spurmatrix oder Abbildungsmatrix, je nach if-else-Fall) (von Z/pZ -> Z/pZ) in kbase (ersten kdim Spalten bilden Basis)
  int kdim = kernbase(m, kbase, p, c);
  // Schreibe für jedes i=1,, .., dimension p*(i-tes Basiselement) als Spalten in Matrix gen, dahinter die oben errechnete Basis vom Kern
  // Wir erhalten (als Spalten) ein Erzeugendensystem vom Kern von Z->Z/pZ: x->x^(p^j)
  bigintmat *gen = new bigintmat(n, n+kdim, o->basecoeffs());

  for (int i=1; i<=n; i++) {
    basis_elt(bas, i);
    bas->skalmult(p, c);
    gen->setcol(i, bas);
  }
  for (int i=1; i<=kdim; i++) {
    kbase->getcol(i, bas);
    gen->setcol(i+n, bas);
  }

  // HNF auf EZS anwenden liefert (als letzten dimension Spalten) eine Basis des Kerns
  gen->hnf();
  bigintmat *tmp = new bigintmat(n, 1, o->basecoeffs());
  bigintmat *nbase = new bigintmat(n, n, o->basecoeffs());
  // Schreibe diese als Spalten in nbase und gib nbase zurück
  for (int i=1; i<=n; i++) {
    gen->getcol(gen->cols()-n+i, tmp);
    nbase->setcol(i, tmp);
  }

  n_Delete(&dimen, o->basecoeffs());
  delete m;
  delete bas;
  delete kbase;
  delete gen;
  delete tmp;
  return nbase;

}

void rowhnf(bigintmat * b) {
  bigintmat * n = b->transpose(), *m;
//  for(int i=1; i<= n->rows() /2; i++)
//    n->swaprow(i, n->rows()-i+1);
// TODO: needs probable more row&column swapping.
  n->hnf();
  m =  n->transpose();
  b->copy(m);
  delete n;
  delete m;
}

number multring(bigintmat *nbase, nforder *o, number p) {
  coeffs R = o->basecoeffs();
  number divi;
  int n = o->getDim();

  bigintmat *inv = new bigintmat(n, n, R);
  divi = nbase->pseudoinv(inv);

  // Zusammenbau der "langen" Matrix
  bigintmat *lon = new bigintmat(n, 0, R);
  bigintmat *oldlon;
  bigintmat *mm = new bigintmat(n, n, R);
  bigintmat *temp = new bigintmat(n, 1, R);
  bigintmat *nochnetemp = new bigintmat(n, n, R);

  for (int i=1; i<=n; i++) {
    nbase->getcol(i, temp);
    o->multmap(temp, mm);
    bimMult(inv, mm, nochnetemp);
    mm->copy(nochnetemp);
    mm->inpTranspose();
    oldlon = lon;
    lon = new bigintmat(n, (i)*n, o->basecoeffs());
    lon->concatcol(oldlon, mm);
    delete oldlon;
  }

  lon->skaldiv(divi);

  bigintmat * red;
  if (1) {
    bigintmat * cmp = lon->modhnf(p, o->basecoeffs());
    red = cmp;
  } else {
    lon->hnf();
    red = new bigintmat(n, n, o->basecoeffs());
    lon->getColRange((n-1)*n+1, n, red);
  }
  delete lon;
  red->inpTranspose();

  number divisor = red->pseudoinv(nbase);
  nbase->hnf();

  delete inv;
  delete mm;
  delete temp;
  delete red;
  delete nochnetemp;
  n_Delete(&divi, o->basecoeffs());
  return divisor;
}

nforder *onestep(nforder *o, number p, coeffs c) {
  // Berechne F_p-Basis von I_p/pI_p
  bigintmat *basis;
  basis = radicalmodpbase(o, p, c);


  // Bestimme Basis vom Ring der Multiplikatoren (speicher diese in basis), und Nenner davon (in divisor)
  number divisor = multring(basis, o, p);
  // Erzeuge neue Ordnung, der o zu Grunde liegt, mit Basis basis und Nenner divisor
  if (basis->isOne() && n_IsOne(divisor, c)) {
    delete basis;
    n_Delete(&divisor, c);
    return o;
  }

  nforder *no = new nforder(o, basis, divisor, c);

  delete basis;
  n_Delete(&divisor, c);
  return no;
}

nforder *pmaximal(nforder *o, number p) {
  coeffs c = o->basecoeffs();
  nforder *no = o;
  nforder *otemp;
  // TODO: check if p^2 still divides disc (maybe in onestep)
  // simplify the tower
  do {
    otemp = no;
    no = onestep(otemp, p, c);
    if (no==otemp)
      break;
    nforder_delete (otemp);
    otemp = no->simplify();
    nforder_delete (no);
    no = otemp;
  } while (1);
  return no;
}

/*
// Zum Round2 fehlt noch die Faktorisierung der Diskriminante. Daher auch noch nicht getestet
nforder *round2(nforder *o) {
  nforder *otemp = new nforder(o,basecoeffs());
  number p = otemp->getsmallestsqprime(); // Benötigt kleinste Primzahl, die die Disc. quadratisch teilt
  nforder *no;
  number eins = n_Init(1, basecoeffs());
  number tmp;
  while (n_GreaterZero(p,basecoeffs())) {
    // Laufe durch Primzahlen p, die die Ordnung quadratisch teilen, und erzeuge p-maximale Ordnung
    no = pmaximal(otemp, p);
    delete otemp;
    otemp = no;
    // Nimm nächstgrößere Primzahl, welche die Ordnung quadratisch teilt
    tmp = n_Add(p,eins, basecoeffs());
    p = otemp->getsmallestsqprime(tmp); // Benötigt kleinste Primzahl größer tmp, die die Disc. quad. teilt
    n_Delete(&tmp, basecoeffs());
  }
  n_Delete(&p, basecoeffs());
  n_Delete(&eins, basecoeffs());
  return otemp;
}
*/


void nforder::createmulttable(bigintmat **a) {
  // Falls es eine Multtable gibt, liefere eine Kopie davon zurück
  if (multtable != NULL) {
    for (int i=0; i<dimension; i++) {
      a[i] = new bigintmat(multtable[i]);
    }
  }
  else {
    // Sonst berechne sie auf kanonische Art und Weise
    bigintmat *bas = new bigintmat(1, dimension, basecoeffs());
    for (int i=0; i<dimension; i++) {
      basis_elt(bas, i+1);
      a[i] = new bigintmat(dimension, dimension, basecoeffs());
      multmap(bas, a[i]);
    }
  }
}
