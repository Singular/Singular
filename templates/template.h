//*****************************************************************************
// Computer Algebra System SINGULAR    
// Copyright: See COPYING file that comes with this distribution
//*****************************************************************************
/** @file template.h
 * 
 * This file defines the class TEMPLATE. 
 *
 * ABSTRACT: We need this for something...
 *
 * @author Oleksandr Motsak
 *
 * @internal @version \$Id$
 *
 **/
//*****************************************************************************

#ifndef SINGULAR_TEMPLATE_CLASS_h_
#define SINGULAR_TEMPLATE_CLASS_h_

// include basic definitions
#include "singularxx_defs.h"

BEGIN_NAMESPACE_SINGULARXX

/** @class TEMPLATE template.h "someRootDirectory/template.h"
 * 
 * Brief description of class TEMPLATE.
 *
 * Detailed description of class TEMPLATE. 
 *
 * @sa some other classes
 */
class TEMPLATE: public TEMPLATE_BASE_CLASS
{
public:
  /// Name type of *this.  
  typedef TEMPLATE self;

  /// My enum. Or yours, if you want!
  enum My 
  {
    int EVal1, ///< Enum value 1
    int EVal2  ///< Enum value 2
  };

  /// Default constructor. 
  /// And now goes the detailed description....
  TEMPLATE();

  /// Default destructor. 
  /// And now goes its detailed description....
  ~TEMPLATE();
  

  /** Brief method description.
   *
   * Detailed method description.
   *
   * @param [in] a some int
   * @param [out] o integer result
   * @return information about return value
   * @sa strcpy2
   */
  int Method( int a, int &o );

  /// Example of Copy a string method.
  ///
  /// The strcpy function copies @a strSource, including
  /// the terminating null character, to the location
  /// specified by @a strDestination. No overflow checking
  /// is performed when strings are copied or appended.
  /// The behavior of @a strcpy is undefined if the source
  /// and destination strings overlap.
  ///
  /// @return @a strcpy returns the destination string.
  /// No return value is reserved to indicate an error.
  /// @sa Method(), wcscpy(), _mbscpy(), strncpy()
  char* strcpy2(
            char* strDestination,  ///< [out] pointer to source null terminated 
                                   ///< string ...
                                   ///< continue ...
            const char* strSource  ///< [in] pointer to destination memory ....
                                   ///< continue ...
               );


protected:
  /// Short description of some flag. Long description: use this format to document a variable or
  /// method
  bool m_bSomeFlag;
};

END_NAMESPACE_SINGULARXX

#endif 
/* #ifndef SINGULAR_TEMPLATE_CLASS_h_ */
