/*******    File: leaf.c    *******/

#include "gmp.h"
#include "MP.h"
#include "node.h"
#include "label.h"
#include "leaf.h"

/****************************************************************** 
 *   FUNCTION: display_data_node
 *   ARGUMENT: dnp - a pointer to the root of a subtree.
 *   RETURN:   int indicating if dnp is a data node
 *             1 = data node ouput done
 *             0 = not a know data node
 *   PURPOSE:  Output the data node dnp to stdout
 ******************************************************************/

int display_data_node(dnp)
node_t  *dnp;
{ char   c;
  /* Treat a data node */
  switch(dnp->type)
  {   case MP_Sint32Type: 
            printf("%d", (long) *((long *)dnp->data));
            return 1;
      case MP_Uint32Type: 
            printf("%u", (long) *((long *)dnp->data));
            return 1;
      case MP_Sint8Type:
            printf("%d", *(dnp->data));
            return 1;
      case MP_Uint8Type:
            printf("%u", *((MP_Uint8_t *)dnp->data));
            return 1;
      case MP_BooleanType:
            c = (*((MP_Boolean_t *)dnp->data) == 1) ? 'T' : 'F';
            printf("%c", c);
            return 1;
      case MP_Real32Type: 
            printf("%20.10G", (float) *((float *)dnp->data));
            return 1;
      case MP_Real64Type: 
            printf("%20.15lG",   (double) *((double *)dnp->data));
            return 1;
      case MP_CommonLatinIdentifierType: 
            printf("%c", *(dnp->data));
            return 1;
      case MP_CommonGreekIdentifierType: 
            printf("%c", *(dnp->data));
            return 1;
      case MP_IdentifierType: 
            printf("%s", dnp->data);
            return 1;
      case MP_ConstantType: 
            printf("%s", dnp->data);
            return 1;
      case MP_CommonConstantType: 
            printf("%d", *((MP_Common_t *)dnp->data));
            return 1;
      case MP_StringType: 
            printf("%s", dnp->data);
            return 1;
      case MP_MetaType: 
            printf("%s", dnp->data);
            return 1;
      case MP_RawType: 
            printf("%s\n", dnp->data);
            return 1;
      case MP_ApIntType: 
            mpz_out_str(stdout,10, (mpz_ptr)dnp->data); 
            return 1;
      case MP_ApRealType: 
            mpf_out_str(stdout, 10, 0, (mpf_ptr)dnp->data);
            return 1;
  }
  return(0);
  /*  consider printing annotation on data with (note data annotation) */
}

/****************************************************************** 
 *   FUNCTION: is_data_node
 *   ARGUMENT: dnp - a pointer to the root of a subtree.
 *   RETURN:   0 or 1
 *   PURPOSE:  predicate function
 ******************************************************************/

int is_data_node(dnp)
node_t  *dnp;
{ switch(dnp->type)
  {   case MP_Sint32Type: 
      case MP_Uint32Type: 
      case MP_Sint8Type:
      case MP_Uint8Type:
      case MP_BooleanType:
      case MP_Real32Type: 
      case MP_Real64Type: 
      case MP_CommonLatinIdentifierType: 
      case MP_CommonGreekIdentifierType: 
      case MP_IdentifierType: 
      case MP_ConstantType: 
      case MP_CommonConstantType: 
      case MP_StringType: 
      case MP_MetaType: 
      case MP_RawType: 
      case MP_ApIntType: 
      case MP_ApRealType: 
            return 1;
  }
  return(0);
}

/* checks for a negative term */
int is_neg(dnp)
node_t  *dnp;
{ return (dnp->type == MP_OperatorType &&
          *(dnp->data) == '-' &&
          dnp->num_children == 1);
}
