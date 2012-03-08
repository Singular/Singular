#ifndef NEWTYPES_H
#define NEWTYPES_H

typedef struct newstruct_desc_s *newstruct_desc;

void newstruct_setup(const char * name, newstruct_desc d);
newstruct_desc newstructFromString(const char *s);
newstruct_desc newstructChildFromString(const char *p, const char *s);
BOOLEAN newstruct_set_proc(const char *name,const char *func,int args, procinfov p);

#endif
