/*
 * gfanlib_polymakefile.cpp
 *
 *  Created on: Nov 17, 2010
 *      Author: anders
 */
#include <stddef.h>
#include "gfanlib_polymakefile.h"

#include <assert.h>
#include <stdio.h>
#include <sstream>
#include <istream>

using namespace std;

static void eatComment2(int &c, stringstream &s)
{
  if(c=='#')
    {
      do
        c=s.get();
      while(c!='\n' && !s.eof());
    }
}

static void eatComment(stringstream &s)
{
  int c=s.get();
  while(c==' '||c=='\t')c=s.get();
  eatComment2(c,s);
  s.unget();
}

static string readUntil(istream &f, int c)
{
  stringstream ret;
  int c2;
  c2=f.get();
//  c2=fgetc(f);
//  while(c2!=c && c2!=EOF)
  while(c2!=c && !f.eof())
  {
      ret<<char(c2);
      //c2=fgetc(f);
      c2=f.get();
  }
  return ret.str();
}

static list<int> readIntList(istream &s)
{
  list<int> ret;
  int c=s.peek();
  while(((c>='0') && (c<='9'))|| (c==' '))
    {
      //      fprintf(Stderr,"?\n");
      int r;
      s >> r;
      ret.push_back(r);
      c=s.peek();
    }
  return ret;
}

namespace gfan{
PolymakeProperty::PolymakeProperty(const std::string &name_, const std::string &value_):
  value(value_),
  name(name_)
{
}


void PolymakeFile::open(std::istream &f)
{
  isXml=false;
//  fileName=string(fileName_);

//  FILE *f=fopen(fileName.c_str(),"r");
//  if(!f)//fprintf(Stderr,"Could not open file:\"%s\"\n",fileName_);
//  assert(f);

  int c=f.get();//fgetc(f);
  while(!f.eof())
    {
      if(c=='_')
        {
          readUntil(f,'\n');
        }
      else if(c!='\n')
        {
          f.unget();
          //          ungetc(c,f);
          string name=readUntil(f,'\n');

//          fprintf(Stderr,"Reading:\"%s\"\n",name.c_str());
          stringstream value;
          while(1)
            {
              string l=readUntil(f,'\n');
              if(l.size()==0)break;
              value << l <<endl;
            }
          properties.push_back(PolymakeProperty(name.c_str(),value.str().c_str()));
        }
      c=f.get();//fgetc(f);
    }
}

/*void PolymakeFile::open(const char *fileName_)
{
  isXml=false;
  fileName=string(fileName_);

  FILE *f=fopen(fileName.c_str(),"r");
//  if(!f)//fprintf(Stderr,"Could not open file:\"%s\"\n",fileName_);
  assert(f);

  int c=fgetc(f);
  while(c!=EOF)
    {
      if(c=='_')
        {
          readUntil(f,'\n');
        }
      else if(c!='\n')
        {
          ungetc(c,f);
          string name=readUntil(f,'\n');

//          fprintf(Stderr,"Reading:\"%s\"\n",name.c_str());
          stringstream value;
          while(1)
            {
              string l=readUntil(f,'\n');
              if(l.size()==0)break;
              value << l <<endl;
            }
          properties.push_back(PolymakeProperty(name.c_str(),value.str().c_str()));
        }
      c=fgetc(f);
    }
}
*/
void PolymakeFile::create(const char *fileName_, const char *application_, const char *type_, bool isXml_)
{
  fileName=string(fileName_);
  application=string(application_);
  type=string(type_);
  isXml=isXml_;
}


void PolymakeFile::close()
{
  FILE *f=fopen(fileName.c_str(),"w");
  assert(f);

  if(isXml)
    {
      fprintf(f,"<properties>\n");

      for(list<PolymakeProperty>::const_iterator i=properties.begin();i!=properties.end();i++)
        {
          fprintf(f,"<property name=\"%s\">\n",i->name.c_str());
          fprintf(f,"%s",i->value.c_str());
          fprintf(f,"</property>\n");
        }
      fprintf(f,"</properties>\n");
    }
  else
    {
      fprintf(f,"_application %s\n",application.c_str());
      fprintf(f,"_version 2.2\n");
      fprintf(f,"_type %s\n",type.c_str());

      for(list<PolymakeProperty>::const_iterator i=properties.begin();i!=properties.end();i++)
        {
          fprintf(f,"\n%s\n",i->name.c_str());
          fprintf(f,"%s",i->value.c_str());
        }
    }
  fclose(f);
}


void PolymakeFile::writeStream(ostream &file)
{
  if(isXml)
    {
      file << "<properties>\n";

      for(list<PolymakeProperty>::const_iterator i=properties.begin();i!=properties.end();i++)
        {
          file << "<property name=\"" << i->name.c_str() << "\">\n";
          file << i->value.c_str();
          file << "</property>\n";
        }
      file << "</properties>\n";
    }
  else
    {
      file << "_application " << application << endl;
      file << "_version 2.2\n";
      file << "_type " << type << endl;

      for(list<PolymakeProperty>::const_iterator i=properties.begin();i!=properties.end();i++)
        {
          file << endl << i->name.c_str() << endl;
          file << i->value;
        }
    }
}


list<PolymakeProperty>::iterator PolymakeFile::findProperty(const char *p)
{
  string s(p);
  for(list<PolymakeProperty>::iterator i=properties.begin();i!=properties.end();i++)
    {
      if(s==i->name)return i;
    }

  return properties.end();
}


void PolymakeFile::writeProperty(const char *p, const string &data)
{
  if(hasProperty(p))
    {
      assert(0);
    }
  properties.push_back(PolymakeProperty(string(p),data));
}


bool PolymakeFile::hasProperty(const char *p, bool doAssert)
{
  if(doAssert)
    if(findProperty(p)==properties.end())
      {
        fprintf(stderr,"Property: \"%s\" not found in file.\n",p);
        assert(0);
      }

  return findProperty(p)!=properties.end();
}


Integer PolymakeFile::readCardinalProperty(const char *p)
{
  assert(hasProperty(p,true));

  list<PolymakeProperty>::iterator prop=findProperty(p);
  stringstream s(prop->value);

  int ret;
  s>>ret;

  return ret;
}


void PolymakeFile::writeCardinalProperty(const char *p, Integer n)
{
  stringstream t;
  t<<n<<endl;
  writeProperty(p,t.str());
}


bool PolymakeFile::readBooleanProperty(const char */*p*/)
{
  return false;
}


void PolymakeFile::writeBooleanProperty(const char */*p*/, bool /*n*/)
{
}


ZMatrix PolymakeFile::readMatrixProperty(const char *p, int height, int width)
{
  ZMatrix ret(0,width);

  assert(hasProperty(p,true));
  list<PolymakeProperty>::iterator prop=findProperty(p);
  stringstream s(prop->value);
//  for(int i=0;i<height;i++)
  for(int i=0;;i++)
  {
    if(i==height)break;
    ZVector w(width);
    for(int j=0;j<width;j++)
        {
          int v;
          eatComment(s);
          s>>v;
          if(s.eof())goto done;
          w[j]=v;
        }
    ret.appendRow(w);
  }
  done:

  if(height>=0)assert(ret.getHeight()==height);
//  cerr<<p;
 //   eatComment(s);
   // int v;
   // s>>v;
//  while(!s.eof())std::cerr<<char(s.get());
 // assert(s.eof());

  return ret;
}


void PolymakeFile::writeMatrixProperty(const char *p, const ZMatrix &m, bool indexed, const vector<string> *comments)
{
  stringstream t;

  if(comments)assert((int)comments->size()>=m.getHeight());
  if(isXml)
    {
      t << "<matrix>\n";
      for(int i=0;i<m.getHeight();i++)
        {
          t << "<vector>";
          for(int j=0;j<m.getWidth();j++)
            {
              if(j!=0)t<<" ";
              t<<m[i][j];
            }
          t<<endl;
          t << "</vector>\n";
        }
      t << "</matrix>\n";
    }
  else
    {
      for(int i=0;i<m.getHeight();i++)
        {
          for(int j=0;j<m.getWidth();j++)
            {
              if(j!=0)t<<" ";
              t<<m[i][j];
            }
          if(indexed)t<<"\t# "<<i;
          if(comments)t<<"\t# "<<(*comments)[i];
          t<<endl;
        }
    }
  writeProperty(p,t.str());
}


vector<list<int> > PolymakeFile::readMatrixIncidenceProperty(const char *p)
{
  vector<list<int> > ret;
  assert(hasProperty(p,true));
  list<PolymakeProperty>::iterator prop=findProperty(p);
  stringstream s(prop->value);

  while((s.peek()!=-1)&&(s.peek()!='\n')&&(s.peek()!=0))
    {
      //      fprintf(Stderr,"!\n");
      int c=s.get();
      //fprintf(Stderr,"%i",c);
      assert(c=='{');
      ret.push_back(readIntList(s));
      c=s.get();
      assert(c=='}');
      c=s.get();
      while(c==' ' || c=='\t')c=s.get();
      eatComment2(c,s);
      assert(c=='\n');
    }
  return ret;
}


void PolymakeFile::writeIncidenceMatrixProperty(const char *p, const vector<list<int> > &m)
{
  stringstream t;

  if(isXml)
    {
      t<<"<incidence_matrix>";
      for(unsigned i=0;i<m.size();i++)
        {
          t<<"<set>";
          list<int> temp=m[i];
          temp.sort();
          for(list<int>::const_iterator j=temp.begin();j!=temp.end();j++)
            {
              if(j!=temp.begin())t<<' ';
              t<< *j;
            }
          t<<"</set>\n"<<endl;
        }
      t<<"</incidence_matrix>\n";
    }
  else
    {
      for(unsigned i=0;i<m.size();i++)
        {
          t<<'{';
          list<int> temp=m[i];
          temp.sort();
          for(list<int>::const_iterator j=temp.begin();j!=temp.end();j++)
            {
              if(j!=temp.begin())t<<' ';
              t<< *j;
            }
          t<<'}'<<endl;
        }
    }
  writeProperty(p,t.str());
}


ZVector PolymakeFile::readCardinalVectorProperty(const char *p)
{
  assert(hasProperty(p,true));
  list<PolymakeProperty>::iterator prop=findProperty(p);
  stringstream s(prop->value);

  list<int> temp=readIntList(s);

  ZVector ret(temp.size());
  int I=0;
  for(list<int>::const_iterator i=temp.begin();i!=temp.end();i++,I++)ret[I]=*i;

  return ret;
}


void PolymakeFile::writeCardinalVectorProperty(const char *p, ZVector const &v)
{
  stringstream t;

  if(isXml)
    {
      t<<"<vector>";
      for(unsigned i=0;i<v.size();i++)
        {
          if(i!=0)t<<" ";
          t<<v[i];
        }
      t<<"</vector>\n";
    }
  else
    {
      for(unsigned i=0;i<v.size();i++)
        {
          if(i!=0)t<<" ";
          t<<v[i];
        }
      t<<endl;
    }
  writeProperty(p,t.str());
}


void PolymakeFile::writeStringProperty(const char *p, const string &s)
{
  if(isXml)
    writeProperty(p,s);
  else
    writeProperty(p,s);
}
}
