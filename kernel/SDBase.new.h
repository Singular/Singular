/*! @file
 *
 * @brief Function declarations in ShiftDVec namespace
 *
 * This file is part of the Letterplace ShiftDVec Project
 *
 * @author Grischa Studzinski
 * @author Benjamin Schnitzler benjaminschnitzler@googlemail.com
 *
 * @copyright see main copyright notice for Singular
 */

#ifndef SDBASE_H
#define SDBASE_H

/*! @brief The namespace containing the major part of functions
 *         for the LPDV project.
 */
namespace ShiftDVec
{
  namespace SD = ShiftDVec;

  LObject* enterOnePair
    ( poly p1, int atR1,
      int isFromQ1, int ecart1,
      poly p2, uint shift, int atR2,
      int isFromQ2, int ecart2, SD::kStrategy strat );
}

#endif

// vim: set foldmethod=syntax :
// vim: set textwidth=65 :
// vim: set colorcolumn=+1 :
