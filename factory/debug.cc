/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: debug.cc,v 1.1 1996-12-04 13:59:26 schmidt Exp $ */

/*
$Log:
*/

static int deb_level;
char * deb_level_msg = 0;

void deb_inc_level()
{
    int i;
    deb_level++;
    if ( deb_level_msg != 0 )
	delete [] deb_level_msg;
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
