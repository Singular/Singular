#ifndef NFORDER_ELT_HPP
#define NFORDER_ELT_HPP

THREAD_VAR extern n_coeffType nforder_type;

number EltCreateMat(nforder *a, bigintmat *b);

BOOLEAN n_nfOrderInit(coeffs r,  void * parameter);

#endif
