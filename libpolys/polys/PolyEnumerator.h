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
 *
 **/
/*****************************************************************************/

#ifndef POLYENUMERATOR_H
#define POLYENUMERATOR_H

// include basic definitions
#include <coeffs/Enumerator.h>
#include <polys/monomials/monomials.h>
#include <reporter/reporter.h> // for assume etc.

/** @class CBasePolyEnumerator
 * 
 * Base polynomial enumerator for simple iteration over terms of polynomials.
 *
 * Note that the first element desn't exist directly after Reset() call.
 * 
 * The class doesn't inherit from IAccessor and thus doesn't override Current().
 *
 * @sa IBaseEnumerator, @sa CPolyCoeffsEnumerator
 */
class CBasePolyEnumerator: public virtual IBaseEnumerator
{
  private:
    const poly m_poly; ///< essentially immutable original iterable object
    
    static const spolyrec m_prevposition_struct; ///< tag for "-1" position

  protected:
    poly m_position; ///< current position in the iterable object

    virtual bool IsValid() const
    {
      // not -1 or past the end position?
      return (m_position != NULL) && (m_position != &m_prevposition_struct);
    }
    
  public:
    CBasePolyEnumerator(poly p):
        IBaseEnumerator(), m_poly(p), m_position(const_cast<poly>(&m_prevposition_struct))
    {
      assume( !IsValid() ); 
    }
    
    /// Sets the position marker to the leading term.
    virtual void Reset()
    {
      m_position = const_cast<poly>(&m_prevposition_struct);
      assume( !IsValid() ); 
    }

    /// Advances the position to the next term of the polynomial.
    /// returns true if the position marker was successfully advanced to the
    /// next term which can be used;
    /// false if the position marker has passed the end of the
    /// polynomial.
    virtual bool MoveNext()
    {
      assume( m_position != NULL );

      {
        const poly p_next = pNext(m_position);

        if (p_next != NULL) // not the last term?
        {
          m_position = p_next;
          assume( IsValid() );
          return true;
        }
      }
      
      if (m_position == &m_prevposition_struct) // -1 position?
      {
        assume( !IsValid() );
        m_position = m_poly;
        return (m_position != NULL);
      }

      // else: past the end (or an empty polynomial)
      m_position = NULL;
      assume( !IsValid() );
      return false;
    }
};


/// This is the interface we use in coeffs.h for ClearDenominators and
/// ClearContent.
typedef IEnumerator<number> IPolyCoeffsEnumerator;

/** @class CPolyCoeffsEnumerator
 * 
 * This is a polynomial enumerator for simple iteration over
 * coefficients of polynomials.
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
      assume( IsValid() );
      return pGetCoeff(m_position);      
    }

    /// Gets the current element in the collection (read only).
    virtual IPolyCoeffsEnumerator::const_reference Current() const
    {
      assume( IsValid() );
      return pGetCoeff(m_position);
    }
};

#endif 
/* #ifndef POLYENUMERATOR_H */

// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab

