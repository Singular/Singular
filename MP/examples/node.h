/*
   node.h - all the declarations we need to create, put, get, and print 
	    the nodes of an annotated parse tree.  The tree is stored
	    as a collection of nodes, with each node containing 
	    pointers to a list of annotations associated with the
	    node and a list of children (for operator nodes).
*/

#ifndef node_h
#define node_h

typedef struct annot  annot_t;
typedef struct node   node_t;

/* this is the implementation of an annotation to a data node */
struct annot {
  MP_AnnotType_t  type;	    /* type of the annotation            */
  MP_DictTag_t    dtag;     /* dictionary in which it is defined */
  MP_AnnotFlags_t flags;    /* flags modifying its meaning       */
  node_t    	  *node;    /* ptr to MP tree if valuated        */
  annot_t   	  *next;    /* the next annotation in the list   */
};


/* this is the implementation of a data node */
struct node {
  MP_NodeType_t  type;		/* type of the data field: MP basic type */
  MP_NumAnnot_t  num_annots;	
  MP_NumChild_t  num_children;	/* type == operator : number of children */
				/* type == raw      : # bytes of data    */
  MP_DictTag_t dtag;
  char           *data;          /* generic pointer to the node's "value" */
  char           *aux;           /* generic pointer for "data objects"    */
				/* assumed to be an MP-defined object    */
  annot_t  	 *annot_list;	/* the head of the annotation list       */
  node_t   	 **child_list;   /* dynamic array of ptrs to child nodes  */
};

/* The routines that are available externally. */
void    print_annot();
int     print_node();
annot_t *m_get_annot();
node_t  *m_get_node();
void    free_tree();
void    free_label_table();

#endif
