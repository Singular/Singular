#ifndef LATTICE_HPP
#define LATTICE_HPP

// Algorithms based on chapter 2.6 of
// "A course in computational algebraic numbertheory" by Henry Cohen
class lattice {

    private:

        //array of basisvectors
        bigintmat ** basis;

        //gram matrix of the lattice
        bigintmat * gram_matrix;

        //size of basis
        int n;

        //length of basisvectors
        int m;

        coeffs coef;

        //constant for LLL
        number c;

        //array of basisvectors of LLL-basis
        bigintmat ** b;

        //for LLL, see Cohen 2.6.3
        bigintmat ** b_star;

        //array of B_i, see Cohen 2.6.3
        number * B;

        //for LLL, see Cohen 2.6.3
        bigintmat * H;

        //for LLL, see Cohen 2.6.3
        bigintmat * my;

        //for integral LLL, see Cohen 2.6.7
        number * d;

        //rank of the lattice
        int rank;

        //if true transformation matrix H will be computed
        bool trans_matrix;

        //true if basisvectors should be considered independent
        bool independentVectors;

        //true if gram matrix is integral
        bool integral;

        //triangular matrix for enumeration
        bigintmat * Q;

        //testing element for enumeration
        bigintmat * x;

        //bond for x_i in enumeration
        number * bound;

        //coef for output
        coeffs out_coef;


//         int precision;



        //for LLL, see Cohen 2.6.3
        inline void RED(int k, int l);

        //for LLL, see Cohen 2.6.3
        inline void SWAP(int k, int k_max);

        //for MLLL, see Cohen 2.6.8
        inline void SWAPG(int k, int k_max);

        //for LLL, see Cohen 2.6.3
        inline bool gram_schmidt(int k);

        //for MLLL, see Cohen 2.6.8
        inline void gram_schmidt_MLLL(int k);

//         inline void INSERT(int k, int i);
//         inline bool gram_matrix(int k);

        inline void compute_gram_matrix();

        inline number enumerate_get_next();

        inline bool quadratic_supplement();

        inline void increase_x(int index);
        inline number check_bound(int index);

    public:

        //constructor creates new lattice spanned by columns of b
        lattice(bigintmat* basis);

        //destructor
        ~lattice();

        //LLL with c=3/4
        bool LLL();

        bool LLL(number& c);

        bigintmat * get_basis();

        bigintmat * get_reduced_basis();

        bigintmat * get_transformation_matrix();

        bigintmat * enumerate_all(number a);

        bigintmat * enumerate_next(number a, bigintmat * x);

        bigintmat * enumerate_next(number a);

        bigintmat * enumerate_next(bigintmat * x);

        bigintmat * enumerate_next();

//         void set_precision(int a);

};


//NOTE: could be moved to bigintmat
inline number scalarproduct(bigintmat * a, bigintmat * b);

//minkowski
bigintmat * minkowksi(bigintmat ** elementarray,int size_elementarray, number * poly,int deg, coeffs coef, int precision);
bool IsReal(number a, coeffs coef);
bool ImagGreaterZero(number a, coeffs coef);
number squareroot(number a, coeffs coef, int iteration);// iteration in Heron algorithm

#endif
