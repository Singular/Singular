/* 
    util.c - some utility routines to support the examples.
*/

#include "MP.h"

/* we assume here that annot_type is in the correct range! */
char * annot_to_str(MP_DictTag_t dtag, MP_AnnotType_t annot_type)
{
    switch(dtag) {
    case MP_MpDict    : return MpAnnotations[annot_type];
    case MP_ProtoDict : return ProtoAnnotations[annot_type];
    default : return "Unknown Annot";
    }
}
