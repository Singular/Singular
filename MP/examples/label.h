/*
   label.h - definitions for supporting Label and Reference annotations.
*/

#ifndef label_h
#define label_h

typedef struct ref_node       ref_node_t;
typedef struct label_entry   label_entry_t;

struct ref_node{
  node_t	**node;
  ref_node_t	*next;
};

struct label_entry {
  char		*label;
  ref_node_t	*ref_list;
  node_t	*lnode;
  label_entry_t  *next;
};

#endif
