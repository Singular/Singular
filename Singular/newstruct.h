#ifndef NEWTYPES_H
#define NEWTYPES_H

typedef struct newstruct_desc_s *newstruct_desc;

void newstruct_setup(const char * name, newstruct_desc d);
newstruct_desc newstructFromString(const char *s);

#endif
