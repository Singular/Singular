/* 
   node.c -  Code to support building a linked version of an MP tree
	     in the server.  Label and Reference annotations are 
	     supported, but the Label must occur before the Reference.

   CHANGE LOG:
   May 21, 1995 - SG fixed m_get_node() to properly handle dummy
		  nodes when a reference annotation is found.
		  Also discovered that free_tree() will croak 
		  trying to access a shared substructure that
		  was previously freed.
   May 27, 1995 - SG added routines to copy a node and its descendants,
                  including annotations.  This solves, for the time being,
		  the problem of freeing memory for shared substructures. 
		  Whenever a reference annotation is found, we copy the
		  shared substructure.  The alternative is to do our own
		  memory management, maintaining a reference count for each
		  node, and allow multiple pointers to the same node.
*/
#include "MP.h"
#include "gmp.h"
#include "node.h"
#include "label.h"

extern char * annot_to_str();   /* from util.c */

/*
   We maintain a table a labels and a pointer to the root of the 
   subtree to which the label refers.  This TABLE is freed after
   processing each tree to ensure that we don't get labels 
   confused across trees.  The TABLE is a linear linked list of
   label entries.  In the future we might want to make it a 
   hashed table(?).
*/
static label_entry_t  *TABLE = NULL;

void process_label();
label_entry_t  * find_label();
static void make_entry();
node_t *copy_node();

IMP_ApIntCopy(dest, src)
MP_INT *dest;
MP_INT *src;
{
  int size;
  
  dest->_mp_alloc = src->_mp_alloc;
  dest->_mp_size  = src->_mp_size;
  size = (src->_mp_size < 0) ? -1 * src->_mp_size : src->_mp_size;
  dest->_mp_d     = (unsigned long *)malloc(sizeof(long) * size);
  bcopy((char *)src->_mp_d, (char *)dest->_mp_d, sizeof(long) * size);
}

IMP_ApRealCopy(dest, src)
mpf_t dest;
mpf_t src;
{
  int size;

  dest->_mp_prec = src->_mp_prec;
  dest->_mp_size = src->_mp_size;
  dest->_mp_exp  = src->_mp_exp;
  size = (src->_mp_size < 0) ? -1 * src->_mp_size : src->_mp_size;
  dest->_mp_d     = (unsigned long *)malloc(sizeof(long) * size);
  bcopy((char *)src->_mp_d, (char *)dest->_mp_d, sizeof(long) * size);
}

annot_t *copy_annotation(original)
annot_t *original;
{
  annot_t *cpy = (annot_t *) malloc(sizeof(annot_t));
  
  cpy->type  = original->type;
  cpy->dtag  = original->dtag;
  cpy->flags = original->flags;
  cpy->node = NULL;
  cpy->next = NULL;
  if (original->node != NULL)
    cpy->node = copy_node(original->node);
  if (original->next != NULL)
    cpy->next = copy_annotation(original->next);

  return cpy;
}


node_t* copy_node(original)
node_t *original;
{
  node_t **ochild, **child, *cpy = (node_t *) malloc(sizeof(node_t));
  int    i, datasize;
 
  cpy->annot_list   = NULL;
  cpy->child_list   = NULL;
  cpy->num_children = original->num_children;
  cpy->num_annots   = original->num_annots;
  cpy->type         = original->type;
  cpy->dtag         = original->dtag;
 
  switch (cpy->type) {
  case MP_Sint8Type:
  case MP_Uint8Type:
  case MP_BooleanType:             datasize = 1; break;      
  case MP_Sint32Type:
  case MP_Uint32Type:
  case MP_Real32Type:              datasize = 4; break;
  case MP_Real64Type:              datasize = 8; break;
  case MP_IdentifierType:
  case MP_ConstantType:
  case MP_StringType:
  case MP_MetaType:
  case MP_OperatorType:		   datasize = strlen(original->data) + 1; 
                                   break;
  case MP_RawType:                 datasize = original->num_children; break;
  case MP_CommonConstantType:
  case MP_CommonOperatorType:
  case MP_CommonLatinIdentifierType:
  case MP_CommonGreekIdentifierType: datasize = 1; break;
  case MP_ApIntType:                 datasize = sizeof(MP_INT); break;
  case MP_ApRealType:                datasize = sizeof(__mpf_struct);
  default:
    fprintf(stderr,"copy_node: unknown type %d\n", cpy->type);
    free(cpy);
    return NULL;
  }
  cpy->data = (char *) malloc(datasize);
  if (cpy->type == MP_ApIntType) {
    mpz_init((MP_INT *)cpy->data);
    IMP_ApIntCopy(cpy->data, original->data);
    } 
  else if (cpy->type == MP_ApRealType) {
    mpf_init((__mpf_struct *)cpy->data);
    IMP_ApRealCopy(cpy->data, original->data);
  }
  else bcopy(original->data, cpy->data, datasize);

  if ((original->num_children > 0) && (original->type != MP_RawType)) {
    cpy->child_list = (node_t **)malloc(cpy->num_children * sizeof(node_t *));
    for (i = 0, child = cpy->child_list, ochild = original->child_list;
         i < cpy->num_children; i++, child++, ochild++) {
       *child = copy_node(*ochild);
       }
    }

  if (original->annot_list != NULL) 
     cpy->annot_list = copy_annotation(original->annot_list);

  return cpy;
}
  
     
  
/****************************************************************** 
 *   FUNCTION: m_get_annot
 *   ARGUMENT: link - pointer to a stream structure
 *   RETURN:   pointer to an annotation structure
 *   PURPOSE:  Retrieve a single annotation from the data stream
 *	     associated with link.  If the annotation is valuated,
 *	     fetch the MP tree associated with the annotation.
 ******************************************************************/
annot_t *
m_get_annot(link)
MP_Link_t  *link;
{
       char  *s = NULL;
       annot_t  *anp = (annot_t *)malloc(sizeof(annot_t));
       node_t   *dnp;

       anp->type =  anp->flags = 0;
       anp->node = NULL;
       anp->next = NULL;

       MP_GetAnnotationPacket(link, &(anp->dtag), &(anp->type), &(anp->flags));
       if (anp->flags & MP_AnnotValuated) 
	 anp->node = m_get_node(link);
            	
       return anp;
}

/****************************************************************** 
 *   FUNCTION: m_get_node
 *   ARGUMENT: link - pointer to a stream structure
 *   RETURN:   pointer to a node structure
 *   PURPOSE:  Retrieve a node from the data stream associated with 
 *	       link.  If the node has annotations, fetch them.  
 *	       Then if the node has children, fetch them too. 
 *	       
 *	       This routine handles Label and Reference annotations.
 *	       Label annotations are placed in the label TABLE along
 *	       with a pointer to the labelled node.  Finding a
 *	       Reference annotation causes a lookup in the label
 *	       TABLE.  An error message is printed to stderr if
 *	       the Referenced label is not found in the TABLE and
 *	       the Referencing node is left unchanged.  If the label
 *	       is found, the Referencing node is freed and a pointer
 *	       to the labelled node is returned in its place.  
 *	       Further annotations of the Referencing node are read,
 *	       but ignored.  It is assumed that the Referencing node
 *	       has no children.  No check is made for this!
 ******************************************************************/
node_t *
m_get_node(link)
MP_Link_t *link;
{
  MP_NodeType_t    node_type  = 0;
  MP_AnnotType_t   annot_type = 0;
  MP_NumAnnot_t    num_annots = 0;
  MP_NumChild_t    num_child  = 0;
  MP_NodeHeader_t  hdr        = 0;
  MP_Common_t      cval       = 0;
  MP_ApInt_t       apint;
  MP_ApReal_t      apreal;
  int  i, more = 1, referenced_node = 0;
  node_t   *dnp = (node_t *)malloc(sizeof(node_t));
  annot_t  *annotp;
  node_t   **child, *ref_nodep = NULL;
  label_entry_t *labelp = NULL;
  char c, *cp;
  unsigned char uc;
  u_long ui;

  dnp->type = 0;
  dnp->data = NULL;
  dnp->dtag = 0;
  dnp->annot_list = NULL;
  dnp->child_list = NULL;
  dnp->num_children = dnp->num_annots = 0;

  IMP_GetNodeHeader(link, &(dnp->type), &(dnp->dtag), &cval, 
		    &(dnp->num_annots), &(dnp->num_children));
  switch(dnp->type) {
      case MP_Sint32Type: 
	    dnp->data = (char *)malloc(sizeof(MP_Sint32_t));
	    IMP_GetSint32(link, (long*) dnp->data); 
	    break;
      case MP_Uint32Type: 
	    dnp->data = (char *)malloc(sizeof(MP_Uint32_t));
	    IMP_GetUint32(link, (unsigned long *)dnp->data);
	    break;
      case MP_Sint8Type:
	    dnp->data = (char *)malloc(sizeof(MP_Sint8_t));
	    *(dnp->data) = (MP_Sint8_t) cval;
	    break;
      case MP_Uint8Type:
	    dnp->data = (char *)malloc(sizeof(MP_Uint8_t));
	    *(dnp->data) = (MP_Uint8_t) cval;
	    /*  bcopy (&ui, dnp->data, 4);*/
	    break;
      case MP_BooleanType:
	    dnp->data = (char *)malloc(sizeof(MP_Boolean_t));
	    *(dnp->data) = (MP_Boolean_t) cval;
	    /* bcopy (&ui, dnp->data, 4);*/
	    break;
      case MP_Real32Type: 
	    dnp->data = (char *)malloc(sizeof(float));
	    IMP_GetReal32(link, (float *)dnp->data);
	    break;
      case MP_Real64Type: 
	    dnp->data = (char *)malloc(sizeof(double));
	    IMP_GetReal64(link, (double *)dnp->data);
	    break;
      case MP_IdentifierType: 
	    IMP_GetIdentifier(link, &(dnp->data));
	    break;
      case MP_CommonLatinIdentifierType: 
      case MP_CommonGreekIdentifierType: 
      case MP_CommonConstantType:
      case MP_CommonOperatorType:	
	    dnp->data = (char *)malloc(1);
	    *(dnp->data) = cval; 
	    break;
      case MP_ConstantType: 
	    IMP_GetString(link, &(dnp->data));
            break;
      case MP_StringType: 
	    IMP_GetString(link, &(dnp->data));
	    break;
      case MP_MetaType: 
	    IMP_GetMetaType(link, &(dnp->data));
	    break;
      case MP_RawType: 
	    IMP_GetRaw(link, &(dnp->data), &(dnp->num_children));
	    break;
      case MP_ApIntType: 
	    dnp->data = (char *)malloc(sizeof(__mpz_struct));
	    mpz_init((mpz_ptr)dnp->data);
	    IMP_GetApInt(link, (MP_ApInt_t)&(dnp->data));
	    break;
      case MP_ApRealType: 
	    dnp->data = (char *)malloc(sizeof(__mpf_struct));
	    mpf_init((mpf_ptr)dnp->data); 
	    IMP_GetApReal(link, (MP_ApReal_t)&(dnp->data));
	    break;
      case MP_OperatorType: 
	    IMP_GetOperator(link, &(dnp->data));
	    break;
      default : fprintf(stderr, "server - m_get_node: unknown type %d\n", node_type);
		exit(1);
      }

      /* first we get the annotations, if there are any */
      if (dnp->num_annots > 0)  {
         annotp = dnp->annot_list = m_get_annot(link);
	 /* look for Label and Reference annotations */
	 if (annotp->type == MP_AnnotMpLabel) 
	    process_label(annotp->node->data, dnp);
	 else if (annotp->type == MP_AnnotMpRef) {
	   if ((labelp = find_label(annotp->node->data)) != NULL) {
	     referenced_node = MP_TRUE;
	     ref_nodep= labelp->lnode;
	     }
	   else 
	      fprintf(stderr, "unknown label reference: %s\n",
		  annotp->node->data);
           } 
         for (i = 0; i < dnp->num_annots - 1; i++) {
	   annotp->next = m_get_annot(link);
	   annotp = annotp->next;
	   if (annotp->type == MP_AnnotMpLabel) 
	     process_label(annotp->node->data, dnp);
	   else if (annotp->type == MP_AnnotMpRef) {
	     if ((labelp = find_label(annotp->node->data)) != NULL) {
	        referenced_node = MP_TRUE;
	        ref_nodep = labelp->lnode;
	        }
	     else 
	        fprintf(stderr, "unknown reference: %s\n",
		  annotp->node->data);
           }
	  } 
	 }

      /* For now assume that a node with a referenced label is   */
      /* a dummy node and that the child field is 0.  We _don't_ */
      /* check this.  Return a pointer to the _labelled_ node.   */
      /* Also, we are abandoning all remaining annotations.      */
      /* What does the specification have to say about this?     */

      if (referenced_node == MP_TRUE) {
	free(dnp->data); free(dnp);
	return ref_nodep;
	}

      /* now get the children : do special check for raw type since */
      /* we have a double use of the num_children field.            */
      if ((dnp->num_children > 0) && (dnp->type != MP_RawType)) {
	dnp->child_list = (node_t **)malloc(dnp->num_children * sizeof(node_t *));
	for (i = 0,child = dnp->child_list; i < dnp->num_children; i++,child++)
	   *child = m_get_node(link);
	}
      return dnp;
}


/****************************************************************** 
 *   FUNCTION: print_node
 *   ARGUMENT: dnp - a pointer to the root of a subtree.
 *	       fd  - file descriptor where we will print the tree.
 *   RETURN:   int indicating if there are more trees to retrieve.
 *	       This is indicated by the absence of the MP_QUIT node.
 *	       1 = there are more trees to retrieve.
 *	       0 = this is the MP_QUIT node, we are done. 
 *   PURPOSE:  Print a node in the data stream.  This includes any
 *	       annotations the node may have as well as its 
 *	       children.  The data is sent to standard out and to
 *	       the file associated with file descriptor fd.
 ******************************************************************/
int
print_node(dnp, fd)
node_t  *dnp;
FILE    *fd;
{
  char   *str = NULL, c;
  node_t **p;
  MP_INT apint;
  short flags;
  int  i, j, more = 1;
  MP_Uint8_t t;

  switch(dnp->type){
      case MP_Sint32Type: 
	    printf("%-20s       %-30d %d\n", "MP_Sint32" , 
		(long) *((long *)dnp->data), dnp->num_annots);
	    fprintf(fd,"MP_Sint32Type %d %d\n", 
		(long) *((long *)dnp->data), dnp->num_annots);
	    break;
      case MP_Uint32Type: 
	    printf("%-20s       %-30u %d\n", "MP_Uint32", 
		(long) *((long *)dnp->data), dnp->num_annots);
	    fprintf(fd, "MP_Uint32Type %d %u\n", 
		(long) *((long *)dnp->data), dnp->num_annots);
	    break;
      case MP_Sint8Type:
	    printf("%-20s       %-30d %d\n", "MP_Sint8",  
		 *(dnp->data), dnp->num_annots);
	    fprintf(fd,"MP_Sint8Type %d %u\n", *(dnp->data) ,dnp->num_annots);
	    break;
      case MP_Uint8Type:
            t = (MP_Uint8_t)*((MP_Uint8_t *)dnp->data);
	    printf("%-20s       %-30d %d\n", "MP_Uint8", t, dnp->num_annots);
	    fprintf(fd,"MP_Uint8Type %u %u\n", t, dnp->num_annots);
	    break;
      case MP_BooleanType:
	    c = (*((MP_Boolean_t *)dnp->data) == 1) ? 'T' : 'F';
	    printf("%-20s       %-30c %d\n", "MP_Boolean", c, dnp->num_annots);
	    fprintf(fd,"MP_BooleanType %c %d\n", c, dnp->num_annots);
	    break;
       case MP_Real32Type: 
	    printf("%-20s       %-30.10G %d\n", "MP_Real32", 
		   (float) *((float *)dnp->data), dnp->num_annots);
	    fprintf(fd, "MP_Real32Type %d %20.10G\n", 
		   (float) *((float *)dnp->data), dnp->num_annots);
	    break;
      case MP_Real64Type: 
	    printf("%-20s       %-30.15lG %d\n", "MP_Real64", 
		   (double) *((double *)dnp->data), dnp->num_annots);
	    fprintf(fd, "MP_Real64Type %d %20.15lG\n",
		   (double) *((double *)dnp->data), dnp->num_annots);
	    break;
      case MP_CommonLatinIdentifierType: 
	    printf("%-20s %-5d %-30c %-7d\n", "MP_LatinIdentifier",
		   dnp->dtag, *(dnp->data), dnp->num_annots);
	    fprintf(fd, "MP_CommonLatinIdentifierType %d %c %d\n", 
		   dnp->dtag, *(dnp->data), dnp->num_annots);
	    break;
      case MP_CommonGreekIdentifierType: 
	    printf("%-20s %-5d %-30c %-7d\n", "MP_GreekIdentifier",
		   dnp->dtag, *(dnp->data), dnp->num_annots);
	    fprintf(fd, "MP_CommonGreekIdentifierType %d %c %d\n", 
		   dnp->dtag, *(dnp->data), dnp->num_annots);
	    break;
      case MP_IdentifierType: 
	    printf("%-20s %-5d %-30s %-7d\n", "MP_Identifier",
                    dnp->dtag, dnp->data, dnp->num_annots);
	    fprintf(fd, "MP_IdentifierType %d %s %d\n", 
                    dnp->dtag, dnp->data, dnp->num_annots);
	    break;
      case MP_ConstantType: 
	    printf("%-20s %-5d %-30s %-7d\n", "MP_Constant", 
                    dnp->dtag, dnp->data, dnp->num_annots);
	    fprintf(fd, "MP_ConstantType %d %s %d\n",
                    dnp->dtag, dnp->data, dnp->num_annots);
	    break;
      case MP_CommonConstantType: 
	    printf("%-20s %-5d %-30d %-7d\n", "MP_CommonCnst", 
                   dnp->dtag, *((MP_Common_t *)dnp->data), dnp->num_annots);
	    fprintf(fd, "MP_CommonConstantType %d %d %d\n", 
                   dnp->dtag, *((MP_Common_t *)dnp->data), dnp->num_annots);
	    break;
      case MP_StringType: 
	    printf("%-20s       %-20s %d\n", "MP_String", 
                   dnp->data, dnp->num_annots);
	    fprintf(fd, "MP_StringType %s %d\n",  
                   dnp->data, dnp->num_annots); 
	    break;
      case MP_MetaType: 
	    printf("%-20s %-5d %-30s %-7d\n", "MP_Meta", 
                    dnp->dtag, dnp->data, dnp->num_annots); 
	    fprintf(fd, "MP_MetaType %d %s %d\n", 
                   dnp->dtag, dnp->data, dnp->num_annots); 
	    break;
      case MP_RawType: 
	    str = malloc(dnp->num_children + 1);
	    memcpy(str, dnp->data, dnp->num_children);
	    str[dnp->num_children] = '\0';   /* make the output nice */
	    printf("%-20s       %-30s %d\n", "MP_Raw", str, dnp->num_annots);
	    fprintf(fd, "MP_RawType %s %d\n", str, dnp->num_annots);
	    free(str);  
	    break;
      case MP_ApIntType: 
	    printf("%-20s       ", "MP_ApInt");
	    fprintf(fd, "MP_ApIntType ");
	    printf(" %d",  dnp->num_annots);
	    fprintf(fd, " %d", dnp->num_annots);
	    mpz_out_str(stdout,10, (mpz_ptr) dnp->data);
	    mpz_out_str(fd, 10, (mpz_ptr) dnp->data); 
	    printf(" %d\n",  dnp->num_annots);
	    fprintf(fd, " %d\n", dnp->num_annots);
	    fflush(stdout);fflush(fd);
	    break;
      case MP_ApRealType: 
	    printf("%-20s       ", "MP_ApReal");
	    fprintf(fd, "MP_ApRealType "); 
	    mpf_out_str(stdout, 10, 0, (mpf_ptr) dnp->data);
	    mpf_out_str(fd, 10, 0, (mpf_ptr) dnp->data); 
	    printf(" %d\n",dnp->num_annots);
	    fprintf(fd, " %d\n", dnp->num_annots);
	    fflush(stdout);fflush(fd);
	    break;
      case MP_OperatorType: 
	    printf("%-20s %-5d %-30s %-7d %-9d\n", "MP_Op", 
               dnp->dtag, dnp->data, dnp->num_annots, dnp->num_children);
	    fprintf(fd, "MP_OperatorType %d %s %d %d\n",
               dnp->dtag, dnp->data, dnp->num_annots, dnp->num_children);
	    break;
      case MP_CommonOperatorType: 
	    printf(" %-20s %-5d %-30d %-7d% -5d\n", "MP_CommonOp", 
		   dnp->dtag, *((MP_Common_t *)dnp->data), dnp->num_annots, 
		   dnp->num_children);
	    fprintf(fd, "MP_CommonOperatorType %d %d %d %d\n",
		   dnp->dtag, *((MP_Common_t *)dnp->data), dnp->num_annots, 
		   dnp->num_children);
	    if (!(more = !(dnp->dtag == MP_MpDict && 
                           *((MP_Common_t *)dnp->data) == MP_CopMpEndSession)))
		return 0;
	    break;
      default : fprintf(stderr, "server: unknown type %d\n", dnp->type);
      }
  if (dnp->annot_list != NULL) print_annot(dnp->annot_list, fd);
 
  if ((dnp->num_children > 0) && (dnp->type != MP_RawType)) {
    p = dnp->child_list;
    for (i = 0; i < dnp->num_children; i++, p++){
      print_node(*p, fd);
      }
    }
  return more;
}


/****************************************************************** 
 *   FUNCTION: print_annot
 *   ARGUMENT: anp - a pointer to an annotation structure.
 *	       fd  - file descriptor where we will print the tree.
 *   RETURN:   none.
 *   PURPOSE:  Print an annotation node to standard output and to
 *	       the file associated with file descriptor fd.  This 
 *	       includes any arguments the annotation has (if it
 *	       is valuated).  The next annotation in the list, if
 *	       it exists, is recursively printed.
 ******************************************************************/
void
print_annot(anp, fd)
annot_t  *anp;
FILE *fd;
{  char *annotstr = NULL;

    annotstr = annot_to_str(anp->dtag, anp->type);
    printf("%-20s %-5d flags: 0x%X\n", annotstr, anp->dtag,
	  anp->flags);
   fprintf(fd,"%s  %d  %X\n", annotstr, anp->dtag, anp->flags);
   if (anp->node != NULL)  
     print_node(anp->node, fd);
   if (anp->next != NULL)  print_annot(anp->next, fd);
}


/****************************************************************** 
 *   FUNCTION: find_label
 *   ARGUMENT: label - a char string containing the label.
 *   RETURN:   Success - a pointer to the entry in the label TABLE
 *	                 containing label.
 *	       Failure - NULL.
 *   PURPOSE:  Look for label among the collection of label entries
 *	       maintained by TABLE.  This TABLE is cleaned out (freed)
 *	       after each tree is processed.
 ******************************************************************/
label_entry_t  *
find_label(label)
char *label;
{
  label_entry_t *cp, *p = TABLE;
  
  while (p != NULL) {
     if (strcmp(p->label, label) == 0)  break;
     p = p->next;
     }
  if (p != NULL) {
   cp = (label_entry_t *) malloc(sizeof(label_entry_t));
   cp->lnode = copy_node(p->lnode);
   return cp;
  }                                        
  else return p;  
}


node_t *
process_ref(label, node)
char  *label;
node_t **node;
{
   ref_node_t *r;
   label_entry_t *p;

   if ((p = find_label(label)) == NULL) {
      /* p = make_entry(label, *node); */
      r = (ref_node_t *)malloc (sizeof(ref_node_t));
      r->next = p->ref_list;
      p->ref_list = r;
      }
    return (p->lnode);
}



/****************************************************************** 
 *   FUNCTION: process_label
 *   ARGUMENT: label - a char string containing the label.
 *	       node  - a pointer to the node structure associated
 *	               with this label.
 *   RETURN:   None.
 *   PURPOSE:  Determine if the label is already in the label TABLE.
 *	       If yes, print an error announcing the duplicate and 
 *	          return (don't do anything else).
 *	       If no, place the label in the TABLE and associated
 *	          node with it.
 *
 *   COMMENT:  Consider having the routine return an error value
 *	       to identify duplicate labels so the caller can 
 *	       decide what should be done.
 ******************************************************************/
void 
process_label(label, node)
char    *label;
node_t  *node;
{
  label_entry_t  *p;

  if ((p = find_label(label)) != NULL)  /* check for a duplicate */
    fprintf(stderr,"duplicate label found: %s\n", label);
  else    /* not a duplicate, so put it in the table */
    make_entry(label, node);
    
}


/****************************************************************** 
 *   FUNCTION: make_entry
 *   ARGUMENT: label - a char string containing the label.
 *	       node  - a pointer to the node structure associated
 *	               with this label.
 *   RETURN:   None.
 *   PURPOSE:  Make an entry in TABLE for the (label, node) pair.
 *	       It is assumed at this point that label is not 
 *	       already in the TABLE.
 ******************************************************************/
static void 
make_entry (label, node)
char *label;
node_t *node;
{
  label_entry_t  *p = (label_entry_t *)malloc(sizeof(label_entry_t));

  p->label = (char *)malloc(strlen(label) + 1);
  strcpy(p->label, label);
  p->ref_list = NULL;
  p->lnode = node;
  p->next = TABLE;
  TABLE = p;
}


/****************************************************************** 
 *   FUNCTION: free_annotation
 *   ARGUMENT: annotp - a pointer to an annotation structure. 
 *   RETURN:   None.
 *   PURPOSE:  Free the memory allocated to the annotation structure.
 *	       If the annotation is valuated, free its argument first.
 ******************************************************************/
static void
free_annotation(annotp)
annot_t *annotp;
{
  if (annotp->node != NULL) free_tree(annotp->node);
  free(annotp);
}


/****************************************************************** 
 *   FUNCTION: free_tree
 *   ARGUMENT: dnp - a pointer to a node structure. 
 *   RETURN:   None.
 *   PURPOSE:  Free up the memory of the nodes and annotations 
 *	       rooted at the subtree pointed to by dnp.
 ******************************************************************/
void
free_tree(dnp)
node_t *dnp;
{
  annot_t *annotp, *next_annotp;
  node_t **childp;
  int  i;

  /* first we free the annotations */
  for (i = 0, annotp = dnp->annot_list; i < dnp->num_annots; i++) {
    next_annotp = annotp->next;
    free_annotation(annotp);
    annotp = next_annotp;
    }

  /* free the children, if there are any */
  if ((dnp->num_children > 0) && (dnp->type != MP_RawType)) 
    for (i = 0, childp = dnp->child_list; i < dnp->num_children; i++, childp++) 
      free_tree(*childp);
      
  if (dnp->type == MP_ApIntType) mpz_clear((MP_INT *)dnp->data);
  else free(dnp->data);

  /* now free the pointers to the children and the node itself */
  free(dnp->child_list);
  free(dnp);

}

/****************************************************************** 
 *   FUNCTION: free_label_table
 *   ARGUMENT: None.  TABLE is accessed globally.
 *   RETURN:   None.
 *   PURPOSE:  Free up the memory of the label entries pointed 
 *	       to by TABLE.  Set TABLE to NULL on exit.
 ******************************************************************/
void
free_label_table()
{
  label_entry_t *lp = TABLE, *nlp;

  while (lp != NULL) {
    nlp = lp->next;
    free(lp->label);
    free(lp);
    lp = nlp;
    }
  TABLE = NULL;
}
    
/*
static void 
fix_refs(node, ref_list)
node_t  	*node;
ref_node_t  	**ref_list;
{
    ref_node_t  *p = *ref_list;

   while (p != NULL) {
       *(p->node) = node;
       p = p->next;
       free(*ref_list);
       *ref_list = p;
   }
}
*/
