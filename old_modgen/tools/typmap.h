/*
 *
 */

#ifndef _TYPMAP_H
#define _TYPMAP_H
VAR char type_conv[MAX_TOK][32];

void init_type_conv()
{
  strcpy(type_conv[NONE], "none");
  strcpy(type_conv[NONE], "void");
  strcpy(type_conv[INT_CMD], "int");
  strcpy(type_conv[RING_CMD], "ring");
  strcpy(type_conv[QRING_CMD], "ring");
  strcpy(type_conv[POLY_CMD], "poly");
  strcpy(type_conv[NUMBER_CMD], "number");
  strcpy(type_conv[MODUL_CMD], "ideal");
  strcpy(type_conv[VECTOR_CMD], "ideal");
  strcpy(type_conv[IDEAL_CMD], "ideal");
  strcpy(type_conv[MAP_CMD], "map");
  strcpy(type_conv[MATRIX_CMD], "matrix");
  strcpy(type_conv[STRING_CMD], "char *");
  strcpy(type_conv[INTMAT_CMD], "intvec *");
  strcpy(type_conv[INTVEC_CMD], "intvec *");
  strcpy(type_conv[LIST_CMD], "lists");
  strcpy(type_conv[LINK_CMD], "si_link");
  strcpy(type_conv[PACKAGE_CMD], "package");
  strcpy(type_conv[PROC_CMD], "procinfo *");
/*
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
*/
  printf("[%d] %s\n", INT_CMD, type_conv[INT_CMD]);
  printf("[%d] %s\n", MODUL_CMD, type_conv[MODUL_CMD]);
  printf("[%d] %s\n", STRING_CMD, type_conv[STRING_CMD]);
  printf("[%d] %s\n", LINK_CMD, type_conv[LINK_CMD]);
  printf("[%d] %s\n", PACKAGE_CMD, type_conv[PACKAGE_CMD]);
  /**/
}


#endif /* _TYPMAP_H */
