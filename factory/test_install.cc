/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: test_install.cc,v 1.2 1997-10-23 13:21:55 schmidt Exp $ */

//{{{ docu
//
// test_install.cc - test factory installation.
//
//}}}

#include <factory.h>

int
main ()
{
    // this will try to locate the GF(q) tables
    setCharacteristic( 2, 3, 'Z' );
    cout << factoryVersion << endl;
    cout << factoryConfiguration << endl;
    cout << "Factory lives!!" << endl;
    return 0;
}
