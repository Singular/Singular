 /*  File:     prefix.c
  *   Author:   Paul S. Wang (based on code by Simon Gray)
  *   Date:     9/16/96
  */

#include "MP.h"
#include "node.h"
#include "label.h"
#include "leaf.h"
#include "prefix.h"

static void display_op_node(/* note_t dnp */);

/****************************************************************** 
 *   FUNCTION: display_node_prefix
 *   ARGUMENT: dnp - a pointer to the root of a subtree.
 *   RETURN:   none
 *   PURPOSE:  Output the node dnp, including annotations,
 *             to stdout in a lisp-ish prefix format.
 ******************************************************************/
void display_node_prefix(dnp)
node_t  *dnp;
{   node_t **p;
    int i;
 /* display simple data node */
    if ( display_data_node(dnp) ) return;

 /* display operator */
    printf("(");     /* open ( */
    display_op_node(dnp);

 /* display operands */
    if ((dnp->num_children > 0) && (dnp->type != MP_RawType)) 
    { p = dnp->child_list;
      for (i = 0; i < dnp->num_children; i++, p++)
      {    printf(" ");
           display_node_prefix(*p);
      }
    }
    printf(")");     /* close ) */
}

static void display_op_node(dnp)
node_t  *dnp;
{ /* treat an operator node */
  switch(dnp->type)
  {   case MP_OperatorType: 
            printf("(%s ", dnp->data);
            break;
      case MP_CommonOperatorType: 
            printf("(MPop%d ",*((MP_Common_t *)dnp->data));
            break;
      default : fprintf(stderr, "Unknown type %d\n", dnp->type);
            exit(1);
  }
  /*  printing operator annotation as an op-list */
  if (dnp->annot_list != NULL)
           print_annot(dnp->annot_list, stdout);
  printf(") ");
}
