// matrix.h


// This class is designed to handle the input of the integer programming
// problem. It offers facilities to convert a two-dimensional integer array
// into a matrix and to read a matrix from an input file.
// Furhermore, some special routines needed by the IP-algorithms are
// implemented:
// - the computation of a lattice basis for the integer kernel via the LLL
//   algorithm;
// - the computation of a kernel vector which has no zero components (if
//   such a vector exists) and is a basis vector for the kernel lattice;
// - the procedure of Hosten and Shapiro computing a small set of saturation
//   variables for the toric ideal given by the matrix kernel.



#ifndef MATRIX_H
#define MATRIX_H



#include "LLL.h"



class matrix
{



private:

  short rows;

  short columns;
  // Also used as error flag (values <0):
  // -1 indicates a "semantic" error (which occurs e.g. if some constructor
  //    argument is out of range)
  // -2 indicates an error occured when reading from a file

  Integer **coefficients;
  // the matrix entries

  BigInt **H;
  // This array is used to store the LLL-reduced lattice basis of the kernel.
  // Memory allocation is done in the LLL-routines, so the array is only
  // allocated if such a basis is really computed.

  short _kernel_dimension;
  // the number of vectors stored in H (the size of these vectors is columns)
  // If _kernel_dimension==-2, no kernel basis has been computed yet.
  // If _kernel_dimension==-1, an error has occured during the kernel basis
  // computation.



public:



// constructors and destructor

  matrix(const short& row_number, const short& column_number);
  // Creates a zero matrix of the specified size.

  matrix(const short& row_number, const short& column_number,
         Integer** entries);
  // Builds a matrix from its coefficient array.

  matrix(ifstream& input);
  // Reads a matrix from the given ifstream.
  // The input stream must have the following format:
  //
  //    m=number of rows
  //    n=number of columns
  //    coefficients 0..n-1 of row 1
  //    coefficients 0..n-1 of row 2
  //            ...
  //    coefficients 0..n-1 of row m

  matrix(const short& m, const short& n, ifstream& input);
  // Reads a (m x n)-matrix from the given ifstream.
  // The input stream must have the following format:
  //
  //    coefficients 0..n-1 of row 1;
  //    coefficients 0..n-1 of row 2;
  //            ...
  //    coefficients 0..n-1 of row m.

  matrix(const matrix&);
  // copy-constructor (also copies H)

  ~matrix();
  // destructor



// object properties

  BOOLEAN is_nonnegative() const;
  // Returns TRUE, if all entries of the matrix are >=0, else FALSE.

  short error_status() const;
  // Returns columns iff columns<0 (this is the "error flag"), else 0.

  short row_number() const;
  // Retuns the row number.

  short column_number() const;
  // Returns the column number.

  short kernel_dimension() const;
  // Returns the kernel dimension.



// special routines for the IP-algorithms

  short LLL_kernel_basis();
  // Computes a LLL-reduced integer basis of the matrix kernel and returns
  // the kernel dimension (-1 if an error has occurred).
  // This dimension is also stored in the member kernel_dimension.

  short compute_nonzero_kernel_vector();
  // Transforms the kernel lattice basis stored in H so that it contains
  // a vector whose components are all !=0;
  // returns 0 if no such vector exists, else 1.
  // If no such basis has been computed before, this is done now.

  short compute_flip_variables(short*&);
  // Computes a set of flip variables for the algorithm of DiBiase and
  // Urbanke from a kernel vector with no zero components.
  // Returns the size of that set if such a kernel vector exists, else -1.
  // If necessary, such a vector is computed.
  // The computed set is copied to the argument pointer (memory allocation
  // is done in the routine) to be accessible for the calling function.

  short hosten_shapiro(short*& sat_var);
  // Computes a set of saturation variables for the ideal defined by the
  // kernel lattice and returns the size of that set.
  // If no lattice basis has been computed before, this is done now.
  // The computed set is stored in the argument pointer (memory allocation
  // is done in the routine) to be accessible for the calling function.
  // This routine implements the most simple strategy for computing this set:
  // The kernel vectors are examined in their actual. A "greedy" strategy
  // choosing "clever" kernel vectors to begin with could give better results
  // in many cases, but this is not guaranteed...
  // (And what is a clever choice?)

// output

  void print() const;
  // Writes the matrix to the standard output medium.

  void print(FILE*) const;
  // Writes the matrix to the referenced file which has to be opened
  // for writing before.

  void print(ofstream&) const;
  // Writes the matrix to the given ofstream.

  friend class ideal;
  // Our toric ideals are all constructed from matrices, and the matrix class
  // is designed only for the needs of these constructors. So it would be
  // unnecessary overhead to hide the matrix members from these constructors.

};
#endif
