/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: test_install.cc,v 1.1 1997-10-21 11:24:44 schmidt Exp $ */

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
    cout << "Factory lives!!" << endl;
    return 0;
}
