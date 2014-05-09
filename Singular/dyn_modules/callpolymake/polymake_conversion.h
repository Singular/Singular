#ifndef POLYMAKE_CONVERSION_H
#define POLYMAKE_CONVERSION_H

#include <kernel/mod2.h>

#include <gmpxx.h>

#include <polymake/Main.h>
#include <polymake/Matrix.h>
#include <polymake/Rational.h>
#include <polymake/Integer.h>
#include <polymake/Set.h>
#include <polymake/common/lattice_tools.h>
#include <polymake/IncidenceMatrix.h>

#include <gfanlib/gfanlib.h>
#include <gfanlib/gfanlib_q.h>

#include <coeffs/numbers.h>
#include <coeffs/bigintmat.h>
#include <misc/intvec.h>
#include <Singular/lists.h>

/* Functions for converting Integers, Rationals and their Matrices
   in between C++, gfan, polymake and singular */

/* gfan -> polymake */

polymake::Integer GfInteger2PmInteger (const gfan::Integer& gi);
polymake::Rational GfRational2PmRational (const gfan::Rational& gr);
polymake::Vector<polymake::Integer> Intvec2PmVectorInteger (const intvec* iv);
polymake::Matrix<polymake::Integer> GfZMatrix2PmMatrixInteger (const gfan::ZMatrix* zm);
polymake::Matrix<polymake::Rational> GfQMatrix2PmMatrixRational (const gfan::QMatrix* qm);

/* gfan <- polymake */

gfan::Integer PmInteger2GfInteger (const polymake::Integer& pi);
gfan::Rational PmRational2GfRational (const polymake::Rational& pr);
gfan::ZMatrix PmMatrixInteger2GfZMatrix (const polymake::Matrix<polymake::Integer>* mi);
gfan::QMatrix PmMatrixRational2GfQMatrix (const polymake::Matrix<polymake::Rational>* mr);

/* polymake -> singular */

int PmInteger2Int(const polymake::Integer& pi, bool &ok);
intvec* PmVectorInteger2Intvec (const polymake::Vector<polymake::Integer>* vi, bool &ok);
intvec* PmMatrixInteger2Intvec (polymake::Matrix<polymake::Integer>* mi, bool &ok);
lists PmIncidenceMatrix2ListOfIntvecs (polymake::IncidenceMatrix<polymake::NonSymmetric>* icmat);
lists PmAdjacencyMatrix2ListOfEdges (polymake::IncidenceMatrix<polymake::NonSymmetric>* icmat);
intvec* PmSetInteger2Intvec (polymake::Set<polymake::Integer>* si, bool &b);
number PmInteger2Number (const polymake::Integer& pi);
bigintmat* PmMatrixInteger2Bigintmat (polymake::Matrix<polymake::Integer>* mi);

/* polymake <- singular */

polymake::Matrix<polymake::Integer> Intvec2PmMatrixInteger (const intvec* im);


/* Functions for converting cones and fans in between gfan and polymake,
   Singular shares the same cones and fans with gfan */

gfan::ZCone* PmCone2ZCone (polymake::perl::Object* pc);
gfan::ZCone* PmPolytope2ZPolytope (polymake::perl::Object* pp);
gfan::ZFan* PmFan2ZFan (polymake::perl::Object* pf);
polymake::perl::Object* ZCone2PmCone (gfan::ZCone* zc);
polymake::perl::Object* ZPolytope2PmPolytope (gfan::ZCone* zc);
polymake::perl::Object* ZFan2PmFan (gfan::ZFan* zf);

#endif
