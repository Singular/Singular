// the following initialization is needed for linkage on Mac OS X,
// since initialized variables will be "D" (uninit. go to "C") segments
// also, initialization must not be 0, as that goes to "B" (ubuntu)
#include "misc/options.h"
THREAD_VAR unsigned si_opt_1 = Sy_bit(OPT_FASTHC);
THREAD_VAR unsigned si_opt_2 = Sy_bit(V_QUIET)
                   //| Sy_bit(V_QRING) // not default, as speed drops by 10 %
                   | Sy_bit(V_REDEFINE)
                   | Sy_bit(V_LOAD_LIB)
                   | Sy_bit(V_SHOW_USE)
                   | Sy_bit(V_PROMPT)
                   ;

THREAD_VAR BOOLEAN siCntrlc = FALSE;
