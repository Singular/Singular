/**
 * @file cfSubResGcd.h
 *
 * subresultant pseudo remainder sequence GCD over finite fields and Z
**/
#ifndef CF_SUB_RES_GCD_H
#define CF_SUB_RES_GCD_H

/// subresultant GCD over finite fields.
/// In case things become too dense we switch to a modular algorithm
CanonicalForm
subResGCD_p( const CanonicalForm & f, const CanonicalForm & g );

/// subresultant GCD over Z.
/// @note in contrast to subResGCD_p we do not switch to a modular algorithm
///       in case things become too dense
CanonicalForm
subResGCD_0( const CanonicalForm & f, const CanonicalForm & g );
#endif
