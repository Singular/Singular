/*   File:     latex.c 
 *   Author:   Paul S. Wang
 *   Date:     9/19/96
 */

/* Can do the following now from the 4 .bin files in data/
        $$
        \sum_{x=1}^{10}\left(3~x + 2\right)
        $$
        $$
        factor\left(x^{4} + x^{3} - 13~x^{2} - x + 12\right)
        $$
        $$
        expand\left(\left(x + 1\right)~\left(x + 2\right)\right)
        $$
        $$
        \int~{\cos{x}^{2}}\,d\,x
        $$
*/

#include "MP.h"
#include "node.h"
#include "label.h"
#include "leaf.h"
#include "latex.h"

static void get_op_string(/* node_t dnp, char buf[] */);
static int latex_tag(/* char* op, node_t **args */);
static int latex_diff(/* char* op, node_t **args */);
static int latex_int(/* char* op, node_t **args */);
static int latex_sum(/* char* op, node_t **args */);
static int latex_cir(/* char* op, node_t **args */);

/****************************************************************** 
 *   FUNCTION: display_node_latex
 *   ARGUMENT: dnp - a pointer to the root of a subtree.
 *   RETURN:   none
 *   PURPOSE:  Output the node dnp, not including annotations,
 *             to stdout in latex format.
 ******************************************************************/

void display_node_latex(dnp, p_op)
node_t  *dnp;
char p_op;   /* op of parent node */
{   node_t **p;
    int i;
    char op[16];
 /* display simple data node */
    if ( display_data_node(dnp) ) return;

 /* get operator */
    get_op_string(dnp, op);  /* sets op to string */

 /* display latex exprssion */
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
              if ( p_op != '+' ) printf("\\left(");
              for (i = 0; i < dnp->num_children; i++, p++)
              {  if ( i > 0 )  
                 { if ( is_neg(*p) )
                   {   printf(" - "); 
                       display_node_latex(*((*p)->child_list), *op);
                   }
                   else
                   {   printf(" + ");
                       display_node_latex(*p, *op);
                   }
                 }
                 else
                      display_node_latex(*p, *op);
              }
              if ( p_op != '+' ) printf("\\right)");
              break;
           case '-': 
              if ( p_op != '+' ) printf("\\left(");
              display_node_latex(p[0], '+');
              printf(" - ");
              display_node_latex(p[1], '-');
              if ( p_op != '+' ) printf("\\right)");
              break;
           case '*': 
              if ( p_op == '/' || p_op == '^') printf("\\left(");
              for (i = 0; i < dnp->num_children-1; i++, p++)
              {  display_node_latex(*p, '*');
                    printf("~", op);
              }
              display_node_latex(*p, '*');
              if ( p_op == '/' || p_op == '^') printf("\\right)");
              break;
           case '/':
              /* if ( p_op == '^' || p_op == '/') printf("\\left("); */
              printf("\\frac{");
              display_node_latex(p[0], '*');
              printf("}{");
              display_node_latex(p[1], '/');
              printf("}");
              /* if ( p_op == '^' || p_op == '/') printf("\\right)"); */
              break;
           case '^': 
              /* if ( p_op == '^') printf("\\left("); */
              display_node_latex(p[0], '^');
              printf("^{");
              display_node_latex(p[1], '^');
              printf("}");
              /* if ( p_op == '^') printf("\\right)"); */
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
    /* arithemetic ops with 1 expressin oprand, or other oprators */
    else
    {   if ( latex_tag(op, p, dnp->num_children)) return;
        printf(op); printf("\\left(");
        for (i = 0; i < dnp->num_children; i++, p++)
        {   display_node_latex(*p, '+');
            if ( i < dnp->num_children - 1) printf(", ");
        }
        printf("\\right)");
    }
}


static int latex_tag(op, args, n)
char* op; node_t **args; int n;
{   if (latex_cir(op, args, n) ||
        latex_diff(op, args, n) ||
        latex_sum(op, args, n) ||
        latex_int(op, args, n)  /* ... */
       ) return 1;
    return 0;
}

static int latex_cir(op, args, n)
char* op; node_t **args; int n;
{   if ( strcmp(op,"sin")==0 || strcmp(op,"cos")==0 || strcmp(op,"tan")==0 ||
         strcmp(op,"cot")==0 || strcmp(op,"sec")==0 || strcmp(op,"csc")==0 ||
         strcmp(op,"arcsin")==0 || strcmp(op,"arccos")==0 ||
         strcmp(op,"arctan")==0 || strcmp(op,"sinh")==0 ||
         strcmp(op,"cosh")==0 ||strcmp(op,"tanh")==0 || strcmp(op,"coth")==0 ||
         strcmp(op,"log")==0 ||strcmp(op,"lg")==0 ||strcmp(op,"ln")==0
       )
    {  printf("\\%s{", op);
       display_node_latex(args[0], '+');
       printf("}");
       return 1;
    }
    return 0;
}


static int latex_diff(op, args, n)
char* op; node_t **args; int n;
{   if (strcmp(op,"diff")==0 || strcmp(op, "derivative")==0)
    {  if (n == 2) /* first derivative */
       {  printf("\\frac{d\\,");
          display_node_latex(args[0], '+');
          printf("}{d\\,");
          display_node_latex(args[1], '+');
          printf("}");
          return 1;
       }
       if (n == 3) /* higher derivative */
       {  printf("\\frac{d^{");
          display_node_latex(args[2], '+');
          printf("}");
          display_node_latex(args[0], '+');
          printf("}{d\\,");
          display_node_latex(args[1], '+');
          printf("}^{");
          display_node_latex(args[2], '+');
          printf("}");
          return 1;
       }
    }
    return(0);
}

static int latex_sum(op, args, n)
char* op; node_t **args; int n;
{   if (strcmp(op,"sum")==0 && n == 4)
    {  printf("\\sum_{");
       display_node_latex(args[1], '+');
       printf("=");
       display_node_latex(args[2], '+');
       printf("}^{");
       display_node_latex(args[3], '+');
       printf("}\\left(");
       display_node_latex(args[0], '+');
       printf("\\right)");
       return 1;
    }
    return 0;
}

static int latex_int(op, args, n)
char* op; node_t **args; int n;
{   if (strcmp(op,"integral")==0 || strcmp(op, "integrate")==0)
    {  if (n == 2) /* indefinite integral */
       {  printf("\\int~{");
          display_node_latex(args[0], '+');
          printf("}\\,d\\,");
          display_node_latex(args[1], '+');
          return 1;
       }
       if (n == 4) /* definite integral */
       {  printf("\\int_{");
          display_node_latex(args[2], '+');
          printf("}^{");
          display_node_latex(args[3], '+');
          printf("}");
          display_node_latex(args[0], '+');
          printf("\\,d\\,");
          display_node_latex(args[1], '+');
          return 1;
       }
    }
    return(0);
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
  /*  hard to handle for latex
      if (dnp->annot_list != NULL)
           print_annot(dnp->annot_list, stdout);
   */
}
