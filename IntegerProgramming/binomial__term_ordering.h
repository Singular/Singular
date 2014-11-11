// binomial__term_ordering.h

// The class binomial needs to know the class term_ordering and vice versa.
// But for a  beautiful design, I did not want to write this declarations
// into the same file. The file binter.h simply puts the two corresponding
// header files together.

#ifndef BINOMIAL__TERM_ORDERING_H
#define BINOMIAL__TERM_ORDERING_H

#include "term_ordering.h"
#include "binomial.h"

#endif  // BINOMIAL__TERM_ORDERING_H
