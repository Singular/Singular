/* Copyright 1997 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// static char * rcsid = "$Id: debug.cc,v 1.2 1997-09-12 07:19:43 Singular Exp $ ";
////////////////////////////////////////////////////////////
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

/*
$Log: not supported by cvs2svn $
Revision 1.1  1997/01/17 05:14:54  michael
Initial revision

*/
