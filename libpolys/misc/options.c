// the following initialization is needed for linkage on Mac OS X, 
// since initialized variables will be "D" (uninit. go to "C") segments
#include <misc/options.h>
unsigned test = 0;
unsigned verbose = Sy_bit(V_QUIET)
                   //| Sy_bit(V_QRING) // not default, as speed drops by 10 %
		   | Sy_bit(V_REDEFINE)
		   | Sy_bit(V_LOAD_LIB)
		   | Sy_bit(V_SHOW_USE)
		   | Sy_bit(V_PROMPT)
		   ;

