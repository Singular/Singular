/****************************************************************** 
 *   FUNCTION: display_node_prefix
 *   ARGUMENT: dnp - a pointer to the root of a subtree.
 *   RETURN:   int indicating if there are more trees to retrieve.
 *             This is indicated by the absence of the MP_QUIT node.
 *             (MP_MpOperatorType MP_QUIT).
 *             1 = there are more trees to retrieve.
 *             0 = this is the MP_QUIT node, we are done. 
 *   PURPOSE:  Output the node dnp, including annotations,
 *             to stdout in a lisp-ish prefix format.
 *
 *   Author:   Paul S. Wang (based on code by Simon Gray)
 *   Date:     9/16/96
 ******************************************************************/

#ifndef __node__prefix__
#define __node__prefix__

void display_node_prefix(/* node_t  dnp */);

#endif
