/***************************************************************************//**
 * @file test.h
 *
 * Let's try to put parameters where they should be in F2...
 *
 * Issue reported: https://bugzilla.gnome.org/show_bug.cgi?id=316311
 *
 ******************************************************************************/

  /** F1. 
   *
   * @param [in] a1 some int
   * @param [out] r1 integer result
   *
   * The end!
   */
  void F1(
     int a1,
     int & r1
         );

  /** F2. 
   *
   * @params <- We would prefere all parameters to be here...
   *
   * @param []  
   *
   * The end!
   */
  void F2(
     int a2,  ///< [in] some int
     int & r2 ///< [out] integer result
         );
