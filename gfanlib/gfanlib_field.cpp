/*
 * gfanlib_field.cpp
 *
 *  Created on: Sep 2, 2015
 *      Author: anders
 */

#include "gfanlib_field.h"

FieldElement::FieldElement(Field const &f):
  //  theField(f)
implementingObject(f.zHomomorphismImplementation(0))
{
//  assert(f.implementingObject);
//  implementingObject=f.zHomomorphismImplementation(0);//create object with refCount 1
  //  f.implementingObject->refCount++;
//  flog2 fprintf(Stderr,"FieldElement - constructing1\n");
}
