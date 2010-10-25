#include "../gfanlib.h"
#include <iostream>
#include <sstream>
using namespace gfan;
using namespace std;

/* Compile using:
 *  g++ test.cpp -L ../gfanlib_zcone.o -L /Users/anders/gfan/cddlib/lib/libcddgmp.a -L /sw/lib/libgmp.a
 */


/*Printing:
 */
string toString(gfan::ZMatrix const &m, char *tab=0)
{
  stringstream s;

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
      s<<endl;
    }
  return s.str();
}

string toPrintString(gfan::ZMatrix const &m, int fieldWidth, char *tab=0)
{
  stringstream s;

  for(int i=0;i<m.getHeight();i++)
    {
      if(tab)s<<tab;
      for(int j=0;j<m.getWidth();j++)
	{
	  stringstream temp;
	  temp<<m[i][j];
	  string temp2=temp.str();
	  for(int k=temp2.size();k<fieldWidth;k++)s<<" ";
	  s<<temp2;  
	  if(i+1!=m.getHeight() || j+1!=m.getWidth())
	    {
	      s<<" ";
	    }
	}
      s<<endl;
    }
  return s.str();
}

string toString(gfan::ZCone const &c)
{
  stringstream s;
  ZMatrix i=c.getInequalities();
  ZMatrix e=c.getEquations();
  s<<"AMBIENT_DIM"<<endl;
  s<<c.ambientDimension()<<endl;
  s<<"INEQUALITIES"<<endl;
  s<<toString(i);
  s<<"EQUATIONS"<<endl;
  s<<toString(e);
  return s.str();
}

string toPrintString(gfan::ZCone const &c, char *nameOfCone)
{
  stringstream s;
  ZMatrix i=c.getInequalities();
  ZMatrix e=c.getEquations();
  s<<nameOfCone<<"[1]:"<<endl;
  s<<c.ambientDimension()<<endl;
  s<<nameOfCone<<"[2]:"<<endl;
  s<<toPrintString(i,6,"   ");
  s<<nameOfCone<<"[3]:"<<endl;
  s<<toPrintString(e,6,"   ");
  return s.str();
}


/* set ok to true before calling the method */
int integerToInt(gfan::Integer const &V, bool &ok)
{
  mpz_t v;
  mpz_init(v);
  V.setGmp(v);
  int ret=0;
  if(mpz_fits_sint_p(v))
    ret=mpz_get_si(v);
  else
    ok=false;
  mpz_clear(v);
  return ret;
}



/*

SKETCH:
-------

intmat zMatrixToIntmat(gfan::ZMatrix const &m)
{
  bool ok=true;

  int d=m.getHeight();
  int n=m.getWidth();

  intmat ret=.....;

  for(int i=0;i<d;i++)
    for(int j=0;j<n;j++)
      ret[i][j]=integerToInt(m[i][j],ok);


  if(!ok)
    {
      ERROR;
    }

  return ret;
}
*/

 /*
SKETCH:
-------

ZMatrix intmatToZMatrix(intmat m)
{
  int d=;
  int n=;
  ZMatrix ret(d,n);

  for(int i=0;i<d;i++)
    for(int j=0;j<n;j++)
      ret[i][j]=m[i][j];

  return ret;
}
 */



  /*
SKETCH:
-------

intmat zVectorToIntvec(gfan::ZVector const &v)
{
  bool ok=true;

  int n=v.size();

  intmat ret=.....;

  for(int i=0;i<n;i++)
    ret[i]=integerToInt(v[i],ok);


  if(!ok)
    {
      ERROR;
    }

  return ret;
}
  */



   /*

SKETCH:
-------

ZVector intvecToZVector(intvec v)
{
  int n=;
  ZVector ret(n);

  for(int i=0;i<n;i++)
      ret[i]=m[i];

  return ret;
}
   */


int main()
{
  ZMatrix A(3,2);

  A[0][0]=2;A[0][1]=2;
  A[1][0]=1;A[1][1]=2;
  A[2][0]=-2;A[2][1]=1;

  ZMatrix temp(0,2);
  ZCone C(A,temp);

  cout<<C;
  C.canonicalize();
  cout<<C;

  cout<<"Relative interior point"<<endl<<C.getRelativeInteriorPoint()<<endl;
  cout<<"Extreme rays"<<endl<<C.extremeRays()<<endl;
  cout<<"Dual cone"<<endl<<C.dualCone()<<endl;
  cout<<"Unique point"<<endl<<C.getUniquePoint()<<endl;
  cout<<"Inequalities"<<endl<<C.getInequalities()<<endl;


  ZMatrix B(1,2);

  B[0][0]=1;B[0][1]=5;
  ZCone C2(B,temp);
  C2.canonicalize();
  cout<<""<<"Quotient lattice basis"<<endl<<C2.quotientLatticeBasis()<<endl;
  cout<<""<<"Semigroup generator"<<endl<<C2.semiGroupGeneratorOfRay()<<endl;





  std::cerr<<toString(C);
    std::cerr<<toPrintString(C,"_");

  return 0;
}
