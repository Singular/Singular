/// testdata.h

/// This file provides an interface for generating test data.

#ifndef TESTDATA_H
#define TESTDATA_H

#include "globals.h"

extern int random_matrix(const short& rows, const short& columns,
                         const Integer& lower_bound,
                         const Integer& upper_bound,
                         ofstream& MATRIX);

/// Creates a random matrix of size rows x columns with entries between
/// lower_bound and upper_bound. This matrix is written in the MATRIX file
/// in the format explained in IP_algorithms.h, together with a randomly
/// generated (integer) cost vector with entries between 0 and upper_bound.

/// The positive row space vector needed by the algorithms of Hosten/Sturmfels
/// and Bigatti/LaScala/Robbiano has to be entered by hand. It can be computed
/// (or its existence can be verified) with the Simplex algorithm that is not
/// included in this package.

extern int transportation_problem(const short& sources, const short& targets,
                                  const Integer& upper_bound,
                                  ofstream& MATRIX);

/// Creates a transportation problem with the given number of sources and
/// targets. This involves creating the (fixed) constraint matrix of size
/// (sources + targets) x (sources * targets) and a randomly chosen cost vector
/// (the "transportation matrix") of size targets*sources with entries between
/// 0 and upper_bound.

/// In the case of the transportation problem, a positive row space vector
/// is given e.g. by the vector where all entries are 1, so it is printed
/// automatically.

extern int random_problems (const short& vector_dimension,
                            const long& number_of_instances,
                            const Integer& lower_bound,
                            const Integer& upper_bound,
                            ofstream& PROBLEM);

/// Creates number_of_instances random vectors of size vector_dimension
/// with entries beteewn lower_bound and upper_bound. These vectors are
/// written in the PROBLEM file in the format explained in IP_algorithms.h.

/// For the (Positive) Conti-Traverso algorithm it might be more useful to
/// generate some problems by hand (randomly created instances show to be
/// unsolvable in many cases).

#endif  /// TESTDATA_H
