
#ifndef NTL_fileio__H
#define NTL_fileio__H

#include <NTL/tools.h>

#if 0
namespace foo_bar {

class ofstream;
class ifstream;

}
#endif

NTL_OPEN_NNS


// opens file for reading

char *FileName(const char* stem, const char *ext);

// builds the name "stem.ext"

char *FileName(const char* stem, const char *ext, long d);

// builds the name stem.ext.d

NTL_CLOSE_NNS

#endif


