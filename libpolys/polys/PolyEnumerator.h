// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file PolyEnumerator.h
 * 
 * Concrete implementation of enumerators over polynomials
 *
 * @author Oleksandr Motsak
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef POLYENUMERATOR_H
#define POLYENUMERATOR_H

// include basic definitions
#include <coeffs/Enumerator.h>
#include <polys/monomials/monomials.h>

/** @class CBasePolyEnumerator
 * 
 * Base polynomial enumerator for simple iteration over terms of
 * (non-zero) polynomials.
 *
 * Note that the first element must exist directly after Reset() call.
 * Moreover, it doesn't inherit from IAccessor and thus doesn't
 * override Current().
 *
 * @sa IBaseEnumerator, @sa CPolyCoeffsEnumerator
 */
class CBasePolyEnumerator: public virtual IBaseEnumerator
{
  private:
    const poly m_poly;

  protected:
    poly m_position;
    
    inline void Iterate()
    {
      if( m_position != NULL )
        pIter( m_position );
    }    
  public:
    CBasePolyEnumerator(poly p): m_poly(p), m_position(p) { assume(p != NULL); }
    
    /// Sets the position marker to the leading term.
    virtual void Reset() { assume(m_poly!= NULL); m_position = m_poly; }

    /// Advances the position to the next term of the polynomial.
    /// returns true if the position marker was successfully advanced to the
    /// next term;
    /// false if the position marker has passed the end of the
    /// polynomial.
    virtual bool MoveNext()
    {
      assume( m_position != NULL );
      Iterate();
      return (m_position != NULL);
    }
};


/// This is the interface we use in coeffs.h for ClearDenominators and
/// ClearContent.
typedef IEnumerator<number> IPolyCoeffsEnumerator;

/** @class CPolyCoeffsEnumerator
 * 
 * This is a polynomial enumerator for simple iteration over
 * coefficients of (non-zero) polynomials.
 * 
 * It is required to inherit this class from IEnumerator<number> for
 * its use in coeffs and implement IAccessor<number> interface.
 *
 * Note also the virtual multiple inheritance due to the diamond
 * problem of inheriting both CBasePolyEnumerator and IEnumerator<T>
 * from IBaseEnumerator.
 *
 * @sa CBasePolyEnumerator, @sa IEnumerator
 */
class CPolyCoeffsEnumerator: public CBasePolyEnumerator, public virtual IPolyCoeffsEnumerator
{
  public:
    CPolyCoeffsEnumerator(poly p): CBasePolyEnumerator(p) { assume(p != NULL); }
    
    /// Gets the current element in the collection (read and write).
    virtual IPolyCoeffsEnumerator::reference Current()
    {
      assume( m_position != NULL );
      return pGetCoeff(m_position);      
    }

    /// Gets the current element in the collection (read only).
    virtual IPolyCoeffsEnumerator::const_reference Current() const
    {
      assume( m_position != NULL );
      return pGetCoeff(m_position);
    }
};

#endif 
/* #ifndef POLYENUMERATOR_H */

// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab

