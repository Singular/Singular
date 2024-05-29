// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file test.h
 *
 * Let's try to put parameters where they should be, compare F1 and F2.
 *
 * ABSTRACT: Issue reported: https://bugzilla.gnome.org/show_bug.cgi?id=316311
 *
 * @author Oleksandr Motsak
 *
 *
 **/
/*****************************************************************************/

  /** F1.
   *
   * This is function 1.
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
   * This is function 2.
   *
   * @params <- We would prefer all parameters to be here...
   *
   * @param []
   *
   * The end!
   */
  void F2(
     int a2,  ///< [in] some int
     int & r2 ///< [out] integer result
         );


// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab
