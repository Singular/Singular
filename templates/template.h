// -*- c++ -*-
/***************************************************************************//**
 * @file template.h
 * 
 * This file defines the class TEMPLATE. AAA. BBB.
 * CCC.
 *
 * DDDD.
 *
 * @author Oleksandr Motsak
 *
 * @par Copyright:
 *   (C) by The SINGULAR Team, see COPYING file
 * EEE.
 *
 * FFFFFFFF.
 * 
 * FFFFFFFFFFFFFFFFFFFFFFFFF.
 * @internal 
 * @version \$Id$
 *
 ******************************************************************************/

// include basic definitions
#include "singularxx_defs.h"

#ifndef SINGULAR_TEMPLATE_CLASS_h_
#define SINGULAR_TEMPLATE_CLASS_h_

BEGIN_NAMESPACE_SINGULARXX

/** @class TEMPLATE template.h somRootDirectory/template.h
 * 
 * [brief description of class TEMPLATE].
 *
 * [detailed description of class TEMPLATE]. 
 * AAAA. 
 * @par blah, blah, ..., blah.
 *
 * @sa see alsos
 */
class TEMPLATE: public TEMPLATE_BASE_CLASS  // doxygen will document the inheritance automatically
{

public:
  /// Name type of *this. 
  typedef TEMPLATE self;

  /// My enum. Not yours!
  enum My 
  {
    int EVal1, ///< Enum value 1
    int EVal2  ///< Enum value 2
  };

  /// Default constructor. And now goes the detailed description....
  /// 
  TEMPLATE();

  /// Default destructor. 
  /// And now goes its detailed description....
  ~TEMPLATE();
  

  /** Brief: [method description].
   *
   * Long: [detailed method description]
   *
   * @param
   * @return [information about return value]
   * @sa [see also section]
   * @note This will be hilighted in the documentation as a special note.
   * @warning [any warning if necessary]
   * @exception std::exception Some explanation of what causes this exception.
   */
  void MethodTemplate(
         const int a, ///< [in] some int
         const int b, ///< [in] another int
         int & result ///< [out] integer result
                     );

  ///
  /// Brief: Example of Copy a string method.
  ///
  /// Long: The strcpy function copies @a strSource, including
  /// the terminating null character, to the location
  /// specified by @a strDestination. No overflow checking
  /// is performed when strings are copied or appended.
  /// The behavior of @a strcpy is undefined if the source
  /// and destination strings overlap.
  ///
  /// @param
  /// @return @a strcpy returns the destination string.
  /// No return value is reserved to indicate an error.
  /// @sa wcscpy(), _mbscpy(), strncpy()
  ///
  char* strcpy2(
            char* strDestination,  ///< [out] pointer to source null terminated 
                                   ///< string ...
                                   ///< continue ...
            const char* strSource, ///< [in] pointer to destination memory ....
                                   ///< continue ...
            const int a            ///< [in] counter
               );


protected:

  /// Short description
  bool m_bAnotherFlag;

  /// Short description of some flag. Long description: use this format to document a variable or
  /// method
  bool m_bSomeFlag;

};

END_NAMESPACE_SINGULARXX

#endif 
/* #ifndef SINGULAR_TEMPLATE_CLASS_h_ */
