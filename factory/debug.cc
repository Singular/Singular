/* emacs edit mode for this file is -*- C++ -*- */
#include "factory/globaldefs.h"

THREAD_VAR static int deb_level = -1;
THREAD_VAR char * deb_level_msg = (char *)"";

void deb_inc_level()
{
    int i;

    // deb_level == -1 iff we enter this function for the first time
    if ( deb_level == -1 )
        deb_level = 0;
    else
        delete [] deb_level_msg;

    deb_level++;
    deb_level_msg = new char[3*deb_level+1];
    for ( i = 0; i < 3*deb_level; i++ )
        deb_level_msg[i] = ' ';
    deb_level_msg[3*deb_level] = '\0';
}

void deb_dec_level()
{
    if ( deb_level > 0 ) {
        int i;
        deb_level--;
        delete [] deb_level_msg;
        deb_level_msg = new char[3*deb_level+1];
            for ( i = 0; i < 3*deb_level; i++ )
                deb_level_msg[i] = ' ';
        deb_level_msg[3*deb_level] = '\0';
    }
}
