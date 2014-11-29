//////////////////////////////////////////
//////////////////////////////////////////
////     ideals in oforder    ////////////
//////////////////////////////////////////
//////////////////////////////////////////
#ifndef NFORDER_IDEAL_HPP
#define NFORDER_IDEAL_HPP

#include <coeffs/bigintmat.h>

class nforder_ideal
{
private:
  ////////////////////////////////////
  ////////// Membervariablen /////////
  ////////////////////////////////////
  number norm, norm_den, min, min_den;
  coeffs ord;  // but of dynamic type order! (as cring)
  bigintmat *basis;
  number den;  // fractional ideals....
public:

  ////////////////////////////////////
  /// 0 Konstruktoren/Destruktoren ///
  ////////////////////////////////////
  nforder_ideal();
  void init();
  nforder_ideal(bigintmat *basis,
                   number * min, number * min_den,
                   number * norm, number * norm_den,
                   const coeffs ord);
  nforder_ideal(bigintmat *basis, const coeffs q);
  nforder_ideal(nforder_ideal *i, int);

  ~nforder_ideal();
  void Write();
  char * String();
  void Print();

  ////////////////////////////////////
  // +1 Zugriff auf Membervariablen //
  ////////////////////////////////////

  number getNorm(), getNormDen(), getMin(), getMinDen();
  inline coeffs order() const { return ord; }
  inline bigintmat * viewBasis() {return basis;};
  inline bigintmat * getBasis() {return new bigintmat(basis);};
  inline number viewBasisDen() {return den;};
  inline int isFractional() {return den!=NULL;};
  inline void setMinTransfer(number a, number b){min = a; min_den = b;}
  inline void setNormTransfer(number a, number b){norm = a; norm_den = b;}
  inline number viewNorm() { return norm;};
  inline number viewMin() { return norm;};
  inline void setBasisDenTransfer(number a){den = a;}

  ////////////////////////////////////
  ////// +2 Elementoperationen ///////
  ////////////////////////////////////
  // Addiert/Subtrahiert/Multipliziert zu a das Element b hinzu
};

nforder_ideal* nf_idAdd(nforder_ideal *a, nforder_ideal *b);
nforder_ideal* nf_idMult(nforder_ideal *a, nforder_ideal *b);
nforder_ideal* nf_idMult(nforder_ideal *a, number b);
nforder_ideal* nf_idMult(nforder_ideal *a, int b);
nforder_ideal* nf_idPower(nforder_ideal *a, int b);
nforder_ideal* nf_idInit(int, coeffs);
nforder_ideal* nf_idInit(number, coeffs);
nforder_ideal* nf_idDiv(nforder_ideal *a, nforder_ideal *b);
nforder_ideal* nf_idMeet(nforder_ideal *a, nforder_ideal *b);
#endif
