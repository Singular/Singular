 /*  File:     infix.c
  *   Author:   Paul S. Wang (based on code by Simon Gray)
  *   Date:     9/16/96
  */

#include "MP.h"
#include "node.h"
#include "label.h"
#include "leaf.h"
#include "infix.h"

static void get_op_string(/* node_t dnp, char buf[] */);

/****************************************************************** 
 *   FUNCTION: display_node_infix
 *   ARGUMENT: dnp - a pointer to the root of a subtree.
 *   RETURN:   none
 *   PURPOSE:  Output the node dnp, not including annotations,
 *             to stdout in an infix format.
 ******************************************************************/

void display_node_infix(dnp, p_op)
node_t  *dnp;
char p_op;
{   node_t **p;
    int i;
    char op[16];
 /* display simple data node */
    if ( display_data_node(dnp) ) return;

 /* get operator */
    get_op_string(dnp, op);  /* sets op to string */
     

 /* display infix exprssion */
    if ((dnp->num_children > 0) && (dnp->type != MP_RawType))
           p = dnp->child_list;
    else return;

    /* arithemetic ops with more than 1 oprands */
    if  ( (dnp->num_children > 1)  
         &&  (*(op) == '+' || *(op) == '-' 
               || *(op) == '*' || *(op) == '/' || *(op) == '^')
        )
    {   switch(*op)
        {  case '+':
              if ( p_op != '+' ) printf("(");
              for (i = 0; i < dnp->num_children; i++, p++)
              {  if ( i > 0 )  
                 { if ( is_neg(*p) )
                   {   printf(" - "); 
                       display_node_infix(*((*p)->child_list), *op);
                   }
                   else
                   {   printf(" + ");
                       display_node_infix(*p, *op);
                   }
                 }
                 else
                      display_node_infix(*p, *op);
              }
              if ( p_op != '+' ) printf(")");
              break;
           case '-': 
              if ( p_op != '+' ) printf("(");
              display_node_infix(p[0], '+');
              printf(" - ");
              display_node_infix(p[1], '-');
              if ( p_op != '+' ) printf(")");
              break;
           case '*': 
              if ( p_op == '/' || p_op == '^') printf("(");
              for (i = 0; i < dnp->num_children-1; i++, p++)
              {  display_node_infix(*p, '*');
                    printf(" ", op);
              }
              display_node_infix(*p, '*');
              if ( p_op == '/' || p_op == '^') printf(")");
              break;
           case '/':
              if ( p_op == '^' || p_op == '/') printf("(");
              display_node_infix(p[0], '*');
              printf(" / ");
              display_node_infix(p[1], '/');
              if ( p_op == '^' || p_op == '/') printf(")");
              break;
           case '^': 
              if ( p_op == '^') printf("(");
              display_node_infix(p[0], '^');
              printf("^");
              display_node_infix(p[1], '^');
              if ( p_op == '^') printf(")");
              break;
        }
    }
    /* basically the -x case */
    else if  ( (dnp->num_children == 1)  
               &&  (*(op) == '-' || *(op) == '+')
               &&  is_data_node(p[0])
             )
    {   printf(op);
        display_data_node(p[0]); 
    }
    /* arithemetic ops with 1 oprand, or other oprators */
    else
    {   printf(op); printf("(");
        for (i = 0; i < dnp->num_children; i++, p++)
        {   display_node_infix(*p, '+');
            if ( i < dnp->num_children - 1) printf(", ");
        }
        printf(")");
    }
}


static void get_op_string(dnp, buf)
node_t  *dnp;
char buf[];
{ switch(dnp->type)
  {   case MP_OperatorType: 
            strcpy(buf, dnp->data);
            return;
      case MP_CommonOperatorType: 
            sprintf(buf, "MPop%d ",*((MP_Common_t *)dnp->data));
            return;
      default : fprintf(stderr, "Unknown type %d\n", dnp->type);
            exit(1);
  }
  /*  printing operator annotation as an op-list */
  /*  hard to handle for infix
      if (dnp->annot_list != NULL)
           print_annot(dnp->annot_list, stdout);
   */
}
