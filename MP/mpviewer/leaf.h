#ifndef __leaf__
#define __leaf__
/*******    File: leaf.h    *******/

/****************************************************************** 
 *   FUNCTION: display_data_node
 *   ARGUMENT: dnp - a pointer to the root of a subtree.
 *   RETURN:   int indicating if dnp is a data node
 *             1 = data node ouput done
 *             0 = not a know data node
 *   PURPOSE:  Output the data node dnp to stdout
 ******************************************************************/

int display_data_node(/* node_t  *dnp */);

/****************************************************************** 
 *   FUNCTION: is_data_node
 *   ARGUMENT: dnp - a pointer to the root of a subtree.
 *   RETURN:   0 or 1
 *   PURPOSE:  predicate function
 ******************************************************************/

int is_data_node(/* node_t  *dnp */);

/* checks for a negative term, such as -x or -cos(x) ***/
int is_neg(/* node_t  *dnp */);

#endif
