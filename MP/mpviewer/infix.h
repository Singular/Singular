/****************************************************************** 
 *   FUNCTION: display_node_infix
 *   ARGUMENT: dnp - a pointer to the root of a subtree.
 *   RETURN:   int indicating if there are more trees to retrieve.
 *             This is indicated by the absence of the MP_QUIT node.
 *             (MP_MpOperatorType MP_QUIT).
 *             1 = there are more trees to retrieve.
 *             0 = this is the MP_QUIT node, we are done. 
 *   PURPOSE:  Output the node dnp, not including annotations,
 *             to stdout in an infix format.
 *
 *   Author:   Paul S. Wang (based on code by Simon Gray)
 *   Date:     9/16/96
 ******************************************************************/
#ifndef __node__infix__
#define __node__infix__

void display_node_infix(/* node_t  dnp, char p_op */);

#endif
