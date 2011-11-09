/* slightly changed for use by omalloc.h */
/*
 * defines to get the return-address for non-dmalloc_lp malloc calls.
 *
 * Copyright 2000 by Gray Watson
 *
 * This file is part of the dmalloc package.
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies, and that the name of Gray Watson not be used in advertising
 * or publicity pertaining to distribution of the document or software
 * without specific, written prior permission.
 *
 * Gray Watson makes no representations about the suitability of the
 * software described herein for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * The author may be contacted via http://dmalloc.com/
 *
 * $Id$
 */

/*
 * This file contains the definition of the GET_RET_ADDR macro which
 * is designed to contain the archecture/compiler specific hacks to
 * determine the return-address from inside the malloc library.  With
 * this information, the library can display caller information from
 * calls that do not use the malloc_lp functions.
 *
 * Most of the archectures here have been contributed by other
 * individuals and may need to be toggled a bit to remove local
 * configuration differences.
 *
 * PLEASE send all submissions, comments, problems to the author.
 *
 * NOTE: examining the assembly code for x =
 * __builtin_return_address(0); with gcc version 2+ should give you a
 * good start on building a hack for your box.
 *
 * NOTE: the hacks mentioned above were removed in favor of the GCC macro
 * __builtin_return_address(). Assumptions made in these hacks break for
 * recent GCC versions.
 */

#ifndef __OM_RETURN_H__
#define __OM_RETURN_H__


/********************************** default **********************************/
#ifndef GET_RET_ADDR

#if __GNUC__ > 1
#define GET_RET_ADDR(file) (file = __builtin_return_address(0))
#else
#define GET_RET_ADDR(file)	(file = 0)
#endif
#endif /* __GNUC__ > 1 */

#endif /* ! __OM_RETURN_H__ */
