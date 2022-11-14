/***********************************************************************************
 * Author: Sebastian Jambor, 2011                                                  *
 * (C) GPL (e-mail from June 6, 2012, 17:00:31 MESZ)                               *
 * sebastian@momo.math.rwth-aachen.de                                              *
 *                                                                                 *
 * Implementation of an algorithm to compute the minimal polynomial of a           *
 * square matrix A \in \F_p^{n \times n}.                                          *
 *                                                                                 *
 * Let V_1, \dotsc, V_k \in \F_p^{1 \times n} be vectors such that                 *
 * V_1, V_1*A, V_1*A^2, \dotsc, V_2, V_2*A, V_2*A^2, \dotsc                        *
 * generate \F_p^{1 \times n}.                                                     *
 * Let mpV_i be the monic polynomial of smallest degree such that                  *
 * V_i*mpV_i(A) = 0.                                                               *
 * Then the minimal polynomial of A is the least common multiple of the mpV_i.     *
 *                                                                                 *
 *                                                                                 *
 * The algorithm uses two classes:                                                 *
 *                                                                                 *
 * 1. LinearDependencyMatrix                                                       *
 * This is used to find a linear dependency between the vectors V, V*A, \ldotsc.   *
 * To to this, it has an internal n \times (2n + 1) matrix.                        *
 * Every time a new row VA^i is inserted, it is reduced via Gauss' Algorithm,      *
 * using right hand sides. If VA^i is reduced to zero, then the vectors are        *
 * linearly dependent, and the dependency can be read of at the right hand sides.  *
 *                                                                                 *
 * Example: Compute the minimal polynomial of A = [[0,1],[1,1]] with V = [1,0]     *
 * over F_5.                                                                       *
 * Then LinearDependencyMatrix will be:                                            *
 * After the first step (i.e., after inserting V = [1,0]):                         *
 *       ( 1 0 | 1 0 0 )                                                           *
 * After the second step (i.e., after inserting VA = [0,1]):                       *
 *       ( 1 0 | 1 0 0 )                                                           *
 *       ( 0 1 | 0 1 0 )                                                           *
 * In the third step, where VA^2 = [1,1] is inserted, the row                      *
 *       ( 1 1 | 0 0 1 )                                                           *
 * is reduced to                                                                   *
 *       ( 0 0 | 4 4 1)                                                            *
 * Thus VA^2 + 4*VA + 4*V = 0, so mpV = t^2 + 4*t + 4.                             *
 *                                                                                 *
 *                                                                                 *
 *                                                                                 *
 * 2. NewVectorMatrix                                                              *
 * If one vector V_1 is not enough to compute the minimal polynomial, i.e. the     *
 * vectors V_1, V_1*A, V_1*A^2, \dotsc don't generate \F_p^{1 \times n}, then      *
 * we have to find a vector V_2 which is not in the span of the V_1*A^i.           *
 * This is done with NewVectorMatrix, which simply holds a reduced n \times n      *
 * matrix, where the rows generate the span of the V_jA^i.                         *
 * To find a vector which is not in the span, simply take the k-th standard        *
 * vector, where k is not a pivot element of A.                                    *
 *                                                                                 *
 *                                                                                 *
 * For efficiency reasons, the matrix entries in LinearDependencyMatrix            *
 * and NewVectorMatrix are not initialized to zero. Instead, a variable rows       *
 * is used to indicate the number of rows which are nonzero (all further           *
 * rows are regarded as zero rows). Furthermore, the array pivots stores the       *
 * pivot entries of the rows, i.e., pivots[i] indicates the position of the        *
 * first non-zero entry in the i-th row, which is normalized to 1.                 *
 ***********************************************************************************/




#ifndef MINPOLY_H
#define MINPOLY_H

class NewVectorMatrix;

class LinearDependencyMatrix {
    friend class NewVectorMatrix;
private:
    unsigned p;
    unsigned long n;
    unsigned long **matrix;
    unsigned long *tmprow;
    unsigned *pivots;
    unsigned rows;

public:
    LinearDependencyMatrix(unsigned n, unsigned long p);
    ~LinearDependencyMatrix();

    // reset the matrix, so that we can use it to find another linear dependence
    // Note: there is no need to reinitialize the matrix and vectors!
    void resetMatrix();


    // return the first nonzero entry in row (only the first n entries are checked,
    // regardless of the size, since we will also apply this for rows with
    // right hand sides).
    // If the first n entries are all zero, return -1 (so this gives a check if row is the zero vector)
    int firstNonzeroEntry(unsigned long *row);

    void reduceTmpRow();

    void normalizeTmp(unsigned i);

    bool findLinearDependency(unsigned long* newRow, unsigned long* dep);

    //friend std::ostream& operator<<(std::ostream& out, const LinearDependencyMatrix& mat);
};


// This class is used to find a new vector for the next step in the
// minimal polynomial algorithm.
class NewVectorMatrix {
private:
    unsigned p;
    unsigned long n;
    unsigned long **matrix;
    unsigned *pivots;
    unsigned *nonPivots;
    unsigned rows;

public:
    NewVectorMatrix(unsigned n, unsigned long p);
    ~NewVectorMatrix();

    // return the first nonzero entry in row (only the first n entries are checked,
    // regardless of the size, since we will also apply this for rows with
    // right hand sides).
    // If the first n entries are all zero, return -1 (so this gives a check if row is the zero vector)
    int firstNonzeroEntry(unsigned long *row);

//    // let piv be the pivot position of row i. then this method eliminates entry piv of row
//    void subtractIthRow(unsigned long *row, unsigned i);

    void normalizeRow(unsigned long *row, unsigned i);

    void insertRow(unsigned long* row);

    // insert each row of the matrix
    void insertMatrix(LinearDependencyMatrix& mat);

    // Finds the smallest integer between 0 and n-1, which is not a pivot position.
    // If no such number exists, return -1.
    int findSmallestNonpivot();

    int findLargestNonpivot();
};


// compute the minimal polynomial of matrix \in \F_p^{n \times n}.
// The result is an array of length n + 1, where the i-th entry represents the i-th coefficient
// of the minimal polynomial.
//
// result should be deleted with delete[]
unsigned long* computeMinimalPolynomial(unsigned long** matrix, unsigned n, unsigned long p);



/////////////////////////////////
// auxiliary methods
/////////////////////////////////


// compute x^(-1) mod p
//
// NOTE: this uses long long instead of unsigned long, for the XEA to work.
// This shouldn't be a problem, since p has to be < 2^31 for the multiplication to work anyway.
//
// There is no need to distinguish between 32bit and 64bit architectures: On 64bit, long long
// is the same as long, and on 32bit, we need long long so that the variables can hold negative values.
unsigned long modularInverse(long long x, long long p);

void vectorMatrixMult(unsigned long* vec, unsigned long **mat, unsigned **nonzeroIndices, unsigned *nonzeroCounts, unsigned long* result, unsigned n, unsigned long p);

// a is a vector of length at least dega + 1, and q is a vector of length at least degq + 1,
// representing polynomials \sum_i a[i]t^i \in \F_p[t].
// After this method, a will be a mod q.
// Method will change dega accordingly.
void rem(unsigned long* a, unsigned long* q, unsigned long p, int & dega, int degq);

// a is a vector of length at least dega + 1, and q is a vector of length at least degq + 1,
// representing polynomials \sum_i a[i]t^i \in \F_p[t].
// After this method, a will be a / q.
// Method will change dega accordingly.
void quo(unsigned long* a, unsigned long* q, unsigned long p, int & dega, int degq);


// NOTE: since we don't know the size of result (the list can be longer than the degree of the polynomial),
// every entry has to be preinitialized to zero!
void mult(unsigned long* result, unsigned long* a, unsigned long* b, unsigned long p, int dega, int degb);


// g = gcd(a,b).
// returns deg(g)
//
// NOTE: since we don't know the size of g, every entry has to be preinitialized to zero!
int gcd(unsigned long* g, unsigned long* a, unsigned long* b, unsigned long p, int dega, int degb);

// l = lcm(a,b).
// returns deg(l)
//
// has side effects for a
//
// NOTE: since we don't know the size of l, every entry has to be preinitialized to zero!
int lcm(unsigned long* l, unsigned long* a, unsigned long* b, unsigned long p, int dega, int degb);


// method suggested by Hans Schoenemann to multiply elements in finite fields
// on 32bit and 64bit machines
static inline unsigned long multMod(unsigned long a, unsigned long b, unsigned long p)
{
#if SIZEOF_LONG == 4
#define ULONG64 (unsigned long long)
#else
#define ULONG64 (unsigned long)
#endif
  return (unsigned long)((ULONG64 a)*(ULONG64 b) % (ULONG64 p));
}

#endif // MINPOLY_H
