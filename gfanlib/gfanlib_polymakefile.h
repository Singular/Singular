/*
 * gfanlib_polymakefile.h
 *
 *  Created on: Nov 17, 2010
 *      Author: anders
 */

#ifndef GFANLIB_POLYMAKEFILE_H_INCLUDED
#define GFANLIB_POLYMAKEFILE_H_INCLUDED


#include <list>
#include <vector>
#include <string>
#include <iostream>

#include "gfanlib_matrix.h"

namespace gfan
{
class PolymakeProperty
{
 public:
  std::string value;
  std::string name;
  PolymakeProperty(const std::string &name_, const std::string &value_);
};

class PolymakeFile
{
  std::string application,type;
  std::string fileName;
  std::list<PolymakeProperty> properties;
  std::list<PolymakeProperty>::iterator findProperty(const char *p);
  void writeProperty(const char *p, const std::string &data);
  bool isXml;
 public:
//   void open(const char *fileName_);
   void open(std::istream &f);
  void create(const char *fileName_, const char *application_, const char *type_, bool isXml_=false);
  void writeStream(std::ostream &file);
  void close();
  bool hasProperty(const char *p, bool doAssert=false);

  // The following could be part of a subclass to avoid dependencies on gfan
  Integer readCardinalProperty(const char *p);
  void writeCardinalProperty(const char *p, Integer n);

  bool readBooleanProperty(const char *p);
  void writeBooleanProperty(const char *p, bool n);

  ZMatrix readMatrixProperty(const char *p, int height, int width);
  void writeMatrixProperty(const char *p, const ZMatrix &m, bool indexed=false, const std::vector<std::string> *comments=0);

  std::vector<std::list<int> > readMatrixIncidenceProperty(const char *p);
  void writeIncidenceMatrixProperty(const char *p, const std::vector<std::list<int> > &m);

  ZVector readCardinalVectorProperty(const char *p);
  void writeCardinalVectorProperty(const char *p, ZVector const &v);

  void writeStringProperty(const char *p, const std::string &s);
};
}

#endif

