/****************************************************************** 
 *   FUNCTION: display_node_latex
 *   ARGUMENT: dnp - a pointer to the root of a subtree.
 *   RETURN:   int indicating if there are more trees to retrieve.
 *             This is indicated by the absence of the MP_QUIT node.
 *             (MP_MpOperatorType MP_QUIT).
 *             1 = there are more trees to retrieve.
 *             0 = this is the MP_QUIT node, we are done. 
 *   PURPOSE:  Output the node dnp, not including annotations,
 *             to stdout in latex format.
 *
 *   Author:   Paul S. Wang
 *   Date:     9/19/96
 ******************************************************************/
#ifndef __node__latex
#define __node__latex

void display_node_latex(/* node_t  dnp, char op */);

#endif
