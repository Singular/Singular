/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file ExtensionInfo.h
 *
 * This file provides a class to store information about finite fields and
 * extensions thereof.
 *
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef EXTENSION_INFO_H
#define EXTENSION_INFO_H

#include <config.h>

#include "canonicalform.h"

/** @class ExtensionInfo ExtensionInfo.h "factory/ExtensionInfo.h"
 *  ExtensionInfo contains information about extension.
 *  If @a m_extension is true we are in an extension of some initial field.
 *  If the initial field is \f$ F_p \f$ and we pass to \f$ F_p (\alpha) \f$
 *  then @a m_alpha is an algebraic variable, @a m_beta= Variable(1),
 *  @a m_gamma= @a m_delta= 1, @a m_GFDegree= 0, @a m_GFName= 'Z'. If we pass
 *  to some GF (p^k) then @a m_alpha= Variable (1), @a m_beta= Variable(1),
 *  @a m_gamma= @a m_delta= 1, @a m_GFDegree= 1, @a m_GFName= 'Z'.
 *  @n If the initial field is \f$ F_p (\epsilon) \f$, then @a m_beta=
 *  \f$ \epsilon \f$, @a m_alpha an algebraic variable defining an extension of
 *  \f$ F_p (\epsilon) \f$, @a m_gamma is a primitive element of
 *  \f$ F_p (\alpha) \f$, @a m_delta is a primitive element of
 *  \f$ F_p (\beta) \f$, @a m_GFDegree= 0, @a m_GFName= 'Z'.
 *  @n If the initial field is GF(p^k), then @a m_alpha= Variable (1),
 *  @a m_beta= Variable (1), @a m_gamma= 1, @a m_delta= 1, @a m_GFDegree()= k,
 *  @a m_GFName= gf_name of the initial field.
 *  @n If @a m_extension is false and the current field is \f$ F_p \f$ then
 *  @a m_alpha= Variable (1), @a m_beta= Variable (1), @a m_gamma= 1,
 *  @a m_delta= 1, @a m_GFDegree= 1, @a m_GFName= 'Z'.
 *  @n If the current field is \f$ F_p (\alpha) \f$ then
 *  @a m_alpha is some algebraic variable, @a m_beta= Variable (1),
 *  @a m_gamma= 1, @a m_delta= 1, @a m_GFDegree= 0, @a m_GFName= 'Z'.
 *  @n If the current field is GF then @a m_alpha= Variable (1),
 *  @a m_beta= Variable (1),  @a m_gamma= 1, @a m_delta= 1,
 *  @a m_GFDegree= getGFDegree(), @a m_GFName= gf_name.
 *
 *  @sa facFqBivar.h, facFqFactorize.h
 */
class ExtensionInfo
{
private:
  /// an algebraic variable or Variable (1)
  Variable m_alpha;
  /// an algebraic variable or Variable (1)
  Variable m_beta;
  /// a primitive element of \f$ F_p (\alpha) \f$ or 1
  CanonicalForm m_gamma;
  /// a primitive element of \f$ F_p (\beta) \f$ or 1
  CanonicalForm m_delta;
  /// GF degree or 1
  int m_GFDegree;
  /// name of GF variable
  char m_GFName;
  /// indicates if we are in an extension of some initial field
  bool m_extension;
public:
  /// \f$ F_p \f$ as initial field, if @a extension is true we are in some GF
  ExtensionInfo (const bool extension  ///< [in] some bool
                );
  /// Construct an @a ExtensionInfo
  ExtensionInfo (const Variable& alpha,      ///< [in] some algebraic variable
                 const Variable& beta,       ///< [in] some algebraic variable
                 const CanonicalForm& gamma, ///< [in] some primitive element
                                             ///< of \f$ F_p (\alpha) \f$
                 const CanonicalForm& delta, ///< [in] some primitive element
                                             ///< of \f$ F_p (\beta) \f$
                 const int nGFDegree,        ///< [in] GFDegree of initial field
                 const char cGFName,         ///< [in] name of GF variable of
                                             ///< initial field
                 const bool extension        ///< [in] some bool
                );
  /// \f$ F_p (\beta) \f$ as initial field and switch to an extension given by
  /// @a alpha, needs primitive elements @a gamma and @a delta for maps
  /// between \f$ F_p (\alpha) \subset F_p (\beta) \f$
  ExtensionInfo (const Variable& alpha,      ///< [in] some algebraic variable
                 const Variable& beta,       ///< [in] some algebraic variable
                 const CanonicalForm& gamma, ///< [in] some primitive element
                                             ///< of \f$ F_p (\alpha) \f$
                 const CanonicalForm& delta  ///< [in] some primitive element
                                             ///< of \f$ F_p (\beta) \f$
                );
  /// \f$ F_p (\alpha) \f$ as initial field, if @a extension is false.
  /// Else initial field is \f$ F_p \f$
  ExtensionInfo (const Variable& alpha, ///< [in] some algebraic variable
                 const bool extension   ///< [in] some bool
                );

  ExtensionInfo (const Variable& alpha ///< [in] some algebraic variable
                );

  /// GF as initial field
  ExtensionInfo (const int nGFDegree,   ///< [in] GF degree of initial field
                 const char cGFName,    ///< [in] name of GF variable
                 const bool extension   ///< [in] some bool
                );

  /// getter
  ///
  /// @return @a getAlpha() returns @a m_alpha
  Variable getAlpha () const
  {
    return m_alpha;
  }
  /// getter
  ///
  /// @return @a getBeta() returns @a m_beta
  Variable getBeta () const
  {
    return m_beta;
  }
  /// getter
  ///
  /// @return @a getGamma() returns @a m_gamma
  CanonicalForm getGamma() const
  {
    return m_gamma;
  }
  /// getter
  ///
  /// @return @a getDelta() returns @a m_delta
  CanonicalForm getDelta() const
  {
    return m_delta;
  }
  /// getter
  ///
  /// @return @a getGFDegree() returns @a m_GFDegree
  int getGFDegree() const
  {
    return m_GFDegree;
  }
  /// getter
  ///
  /// @return @a getGFName() returns @a m_GFName
  char getGFName() const
  {
    return m_GFName;
  }
  /// getter
  ///
  /// @return @a isInextension() returns @a m_extension
  bool isInExtension() const
  {
    return m_extension;
  }
};

#endif
/* EXTENSION_INFO_H */

