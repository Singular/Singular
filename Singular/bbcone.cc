#include <Singular/mod2.h>
#include <Singular/ipid.h>
#include <Singular/blackbox.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/longrat.h>
#include <Singular/subexpr.h>
#include <gfanlib/gfanlib.h>
#include <Singular/bbcone.h>
#include <sstream>



std::string toString1(gfan::ZMatrix const &m, char *tab=0)
{
  std::stringstream s;

  for(int i=0;i<m.getHeight();i++)
    {
      if(tab)s<<tab;
      for(int j=0;j<m.getWidth();j++)
	{
	  s<<m[i][j];
	  if(i+1!=m.getHeight() || j+1!=m.getWidth())
	    {
	      s<<",";
	    }
	}
      s<<std::endl;
    }
  return s.str();
}

std::string toString2(gfan::ZCone const &c)
{
  std::stringstream s;
  gfan::ZMatrix i=c.getInequalities();
  gfan::ZMatrix e=c.getEquations();
  s<<"AMBIENT_DIM"<<std::endl;
  s<<c.ambientDimension()<<std::endl;
  s<<"INEQUALITIES"<<std::endl;
  s<<toString1(i);
  s<<"EQUATIONS"<<std::endl;
  s<<toString1(e);
  return s.str();
}

BOOLEAN bbcone_Assign(leftv l, leftv r)
{
  gfan::ZCone* newZc = new gfan::ZCone();
  if (l->rtyp==IDHDL)
  {
    IDDATA((idhdl)l->data)=(char *)newZc;
  }
  else
  {
    l->data=(void *)newZc;
  }
  return FALSE;
}

char * bbcone_String(blackbox *b, void *d)
{ if (d==NULL) return omStrDup("invalid object");
   else
   {
     std::string s=toString2(*((gfan::ZCone*)d));
     char* ns = (char*) omAlloc(strlen(s.c_str()) + 10);
     sprintf(ns, "%s", s.c_str());
     omCheckAddr(ns);
     return ns;
   }
}

void bbcone_destroy(blackbox *b, void *d)
{
  if (d!=NULL)
  {
    gfan::ZCone* zc = (gfan::ZCone*) d;
    delete zc;
  }
}

void * bbcone_Copy(blackbox*b, void *d)
{       
  gfan::ZCone* zc = (gfan::ZCone*)d;
  gfan::ZCone* newZc = new gfan::ZCone(*zc);
  return newZc;
}

void bbcone_setup()
{
  blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  // all undefined entries will be set to default in setBlackboxStuff
  // the default Print is quite usefule,
  // all other are simply error messages
  b->blackbox_destroy=bbcone_destroy;
  b->blackbox_String=bbcone_String;
  //b->blackbox_Print=blackbox_default_Print;
  //b->blackbox_Init=blackbox_default_Init;
  b->blackbox_Copy=bbcone_Copy;
  b->blackbox_Assign=bbcone_Assign;
  int rt=setBlackboxStuff(b,"bbcone");
  Print("created type %d (bbcone)\n",rt); 
}
