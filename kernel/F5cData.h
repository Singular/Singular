/*!
 * \file F5cData.h
 * \author Christian Eder
 * \date 07/07/2009
 * \version 0.01
 * \brief Declarations of the classes CPair and Rule.
 * \details This file contains the declarations of the class \c CPair, representing
 * critical pairs, and the class \c Label, representing the labels, in the F5C
 * Algorithm. All methods are inlined.
 */

#ifndef F5CDATA_HEADER
#define F5CDATA_HEADER
#ifdef HAVE_F5C

/*!
 * class definitions of the needed datatypes in the F5C algorithm:
 * (a) critical pairs
 * (b) labels
 */
class CPair;
class Label;
class PolyAndShort;

/*!
 * \class CPair
 * \author Christian Eder
 * \brief This is the data structure of a critical pair in the F5C implementation.
 * \details
 */
class CPair {
  private:
  
  public:

}; // end CPair


/*!
 * \class Label
 * \author Christian Eder
 * \brief This is the data structure of a rule, i.e. a label in the F5C
 * Algorithm.
 * \details
 */
class Label {
  private:
    unsigned int* m_pExpVec;
    unsigned long m_nShortExpVec;
  public:
    /*!
     * \fn Label(int* expVec);
     * \param[in] expVec The exponent vector of some monomial defining a label
     * \brief Constructor of an object of class \c Label
     */
    Label(unsigned int* expVec);
    /*!
     * \fn ~Label();
     * \brief Destructor of an object of class \c Label
     */
    ~Label();
    /*!
     * \fn static inline int getExpVec();
     * \return The first entry of the \c integer vector at the address \c m_pExpVec
     * \brief Getter of the \c integer vector at the address \c m_pExpVec
     */
    inline unsigned int* getExpVec(); 
    /*!
     * \fn static inline long getShortExpVec();
     * \return The short exponent vector \c m_nShortExpVec of the label
     * \brief Getter of the \code unsigned long m_nShortExpVec \endcode
     * 
     */
    inline unsigned long getShortExpVec(); 
    /*!
     * \fn static inline unsigned long computeShortExpVec(int* expVec);
     * \param[in] expVec The exponent vector of some monomial defining a label
     * \return Short exponent vector of type \code unsigned long \endcode
     * \brief Computation of the short exponent vector of type \code unsigned 
     * long \endcode of the exponent vector \c expVec
     * \note This method is originally in polys-impl.cc, in this version here it
     * is optimized for the case of working with exponent vectors and not with
     * polys as input data.
     */
    inline unsigned long computeShortExpVec(unsigned int* expVec);
    /*!
     * \fn static inline unsigned long getBitFields(int e, unsigned int s,
     * unsigned int n);
     * \param[in] e Entry of the exponent vector
     * \param[in] s Integer smaller than the bitsize of \c long
     * \param[in] n Integer representing the number of bits per entry of the
     * exponent vector \c expVec
     * \return \code unsigned long \endcode the bitfield for the input data
     * \brief Computation of the bitfield of type \code unsigned long \endcode
     * for an entry \c e of the given exponent vector \c expVec at an iteration 
     * step where \c s iterates over all \c integers smaller than the bitsize of 
     * \c long, and \c n iterates over the number of bits per entry of the 
     * exponent vector \c expVec
     * \note This method is originally in polys-impl.cc, in this version here it
     * is optimized for the case of working with exponent vectors and not with
     * polys as input data.
     */
    inline unsigned long getBitFields(unsigned int e, unsigned int s, unsigned int n);
}; // end Label

/*!
 * \class PolyAndShort
 * \author Christian Eder
 * \brief This is the data structure of a polynomial together with the short
 * exponent vector of its leading monomial in F5C
 * \details
 */
class PolyAndShort {
  private:
    poly          m_poly;
    unsigned long m_nShortExpVec;
  public:
    /*!
     * \fn PolyAndShort(poly p);
     * \param[in] p The polynomial to be stored
     * \brief Constructor of an object of class \c PolyAndShort
     * \details Constructor of an object of class \c PolyAndShort used when
     * inserting an element generating the input ideal \c I of the F5C Algorithm
     */
    PolyAndShort(poly p);
    /*!
     * \fn ~Label();
     * \brief Destructor of an object of class \c Label
     */
    ~PolyAndShort();
    /*!
     * \fn static inline int getExpVec();
     * \return The first entry of the \c integer vector at the address \c m_pExpVec
     * \brief Getter of the \c integer vector at the address \c m_pExpVec
     */
    inline unsigned int* getExpVec(); 
    /*!
     * \fn static inline long getShortExpVec();
     * \return The short exponent vector \c m_nShortExpVec of the label
     * \brief Getter of the \code unsigned long m_nShortExpVec \endcode
     * 
     */

}; // end LPoly
#endif
// HAVE_F5C
#endif
// F5CDATA_HEADER
