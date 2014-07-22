//////////////////////////////////////////
//////////////////////////////////////////
////     Einfache Ordnungs-Klasse     ////
//////////////////////////////////////////
// Kira Kraft, Jan Albert, Marco Sieben //
/////// Praktikum bei Herrn Fieker ///////
//////////////////////////////////////////
//////////////////////////////////////////
/*
 * - Einer Ordnung liegt entweder eine Multiplikationstabelle zu Grunde, oder sie wird durch eine O-Basis erzeugt, wobei O eine andere Ordnung ist
 * - Ordnungselemente werden als Koeffizientenvektoren (Klasse matrix, Zeilenvektoren!) dargestellt und können addiert/subtrahiert/multipliziert werden
 * - Die Diskriminante kann von selbst berechnet werden
*/
#ifndef NFORDER_HPP
#define NFORDER_HPP

enum order_flags_log {
  one_is_one, //if 1 is the first basis element
  is_maximal_known,
  is_maximal
};

class nforder
{
private:
  ////////////////////////////////////
  ////////// Membervariablen /////////
  ////////////////////////////////////
  int rc;
  number discriminant;
  int dimension;
  coeffs m_coeffs;
  bigintmat **multtable; // Multiplikationstabelle als Array von Matrizen ... 
  nforder *baseorder; // ... oder zu Grunde liegende Ordnung  
  bigintmat *basis; // Lin.Komb. der Basiselemente von baseorder zu Basiselementen der Ordnung (Eine Zeile ^= ein Basiselement)  
  number divisor; // Hauptnenner der Linearkombination der Basiselemente  
    // Entweder multtable oder baseorder zeigt auf NULL - je nachdem, wie die Ordnung konstruiert wurde  
  bigintmat *inv_basis; // (inv_basis/inv_divisor) = (basis/divisor)^-1
  number inv_divisor; // 
  int flags;
  
  ////////////////////////////////////
  /////// -1 Memberfunktionen ////////
  ////////////////////////////////////
  // Genauere Beschreibung aller Funktionen in der Funktionen.odt
  
  void init(); //basic initialisation
public:
  void calcdisc(); // Berechnet Diskriminante
  inline int ref_count_incref(){return rc++;};
  inline int ref_count_decref(){return rc--;};
  inline int ref_count(){return rc;};
  
  
  ////////////////////////////////////
  /// 0 Konstruktoren/Destruktoren ///
  ////////////////////////////////////
   //Lädt entweder Multiplikationstabelle von location, oder legt Ordnung o zu Grunde (mit Basis base und Hauptnenner div)  
  nforder(int dim, bigintmat **m, const coeffs q); // (keine Übergabe von const char *, sondern von bigintmat *, diese wird kopiert und als multtable verwendet)
  nforder(nforder *o, bigintmat *base, number div, const coeffs q);
  nforder(nforder *o, int);
  
  ~nforder();
  void Write();
  char* String();
  void Print();
  nforder *simplify(); //collapses a tower: multipy all bases together

  ////////////////////////////////////
  // +1 Zugriff auf Membervariablen //
  ////////////////////////////////////
  
  number getDisc();
  inline number viewDisc(){return discriminant;};
  int getDim();
  inline coeffs basecoeffs() const { return m_coeffs; }
  number getDiv();
  // Liefert Zeiger auf Kopier der Objekte zurück  
  bool getMult(bigintmat **m);
  nforder *getBase();
  bigintmat *getBasis();
  bigintmat *viewBasis();

  inline bool oneIsOne() {return (flags & (1<<one_is_one)) != 0;}
  inline void setOneIsOne() {flags |= (1<<one_is_one);}

  inline bool isMaximalKnown() {return (flags & (1<<is_maximal_known)) != 0;};
  inline bool isMaximal() {return isMaximalKnown() && (flags & (1<<is_maximal_known));};
  inline void setIsMaximal(bool is) {flags = (flags & (~((1<<is_maximal_known) + (1<<is_maximal)))) | (1<<is_maximal_known) | (is*(1<<is_maximal));};
  
  
  
  ////////////////////////////////////
  ////// +2 Elementoperationen ///////
  ////////////////////////////////////
  // Addiert/Subtrahiert/Multipliziert zu a das Element b hinzu  
  void elAdd(bigintmat *a, bigintmat *b);
  void elSub(bigintmat *a, bigintmat *b);
  void elMult(bigintmat *a, bigintmat *b);
  number elTrace(bigintmat *a);
  number elNorm(bigintmat *a);
  bigintmat * elRepMat(bigintmat *a);
  
  ////////////////////////////////////
  //// +3 Funktionen für Round 2 /////
  ////////////////////////////////////
  // long long int getsmallestsqprime();
  /* Liefert kleinste Primzahl >= p, die die Diskriminante quadratisch teilt  */
  void multmap(bigintmat *a, bigintmat *m);
  bigintmat *traceMatrix();

  void createmulttable(bigintmat **a);
  
};

////////////////////////////////////
////// 1 Komfortfunktionen /////////
////////////////////////////////////
/* Setzt Vektor m auf (0,...,0,1,0,...,0) (i-ten Basisvektor)  */
void basis_elt(bigintmat *m, int i);

////////////////////////////////////
//////////// 2 Round 2 /////////////
////////////////////////////////////
/* Liefert bzgl. Primzahl p um eines größere Ordnung von o zurück */ 
nforder *onestep(nforder *o, number p, coeffs c);
/* Macht liefert p-maximale Ordnung von o zurück  */
nforder *pmaximal(nforder *o, number p);
/* Liefert Maximalordnung, ausgehend von o, zurück  */
nforder *round2(nforder *o); // Benötigt Faktorisierung der Diskriminanten
/* Liefert Basis von I_p(O)/pI_p(O)  */
bigintmat *radicalmodpbase(nforder *o, number p, coeffs c);
/* Berechnet die Basis mit Hilfe der langen Matrix  */
number multring(bigintmat* nbase, nforder *o, number p);
void nforder_delete(nforder *o);

#endif
