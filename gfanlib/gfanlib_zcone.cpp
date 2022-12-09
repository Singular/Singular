/*
 * lib_cone.cpp
 *
 *  Created on: Sep 29, 2010
 *      Author: anders
 */
#include <stddef.h>
#include "gfanlib_zcone.h"

#include <vector>
#include <set>
#include <sstream>

#include <config.h>
extern "C"
{
#ifdef HAVE_CDDLIB_SETOPER_H
#include <cddlib/setoper.h>
#include <cddlib/cdd.h>
#else
#ifdef HAVE_CDD_SETOPER_H
#include <cdd/setoper.h>
#include <cdd/cdd.h>
#else
#include <setoper.h>
#include <cdd.h>
#endif //HAVE_CDDLIB_SETOPER_H
#endif //HAVE_CDD_SETOPER_H
}
extern "C" time_t dd_statStartTime; /*cddlib*/

namespace gfan{
        bool isCddlibRequired()
        {
                return true;
        }
        void initializeCddlibIfRequired() // calling this frequently will cause memory leaks because deinitialisation is not possible with old versions of cddlib.
        {
          if (dd_statStartTime==0)
          {
                dd_set_global_constants();
          }
        }
        void deinitializeCddlibIfRequired()
        {
        #ifdef HAVE_DD_FREE_GLOBAL_CONSTANTS
        //        dd_free_global_constants();
        #endif
        }
  static void ensureCddInitialisation()
  {
          // A more complicated initialisation than the following (meaning attempts to count the number of times
          // cddlib was requested to be initialised) would require cddlib to be thread aware.
          // The error below is implemented with an assert(0) because throwing an exception may leave the impression that
          // it is possible to recover from this error. While that may be true, it would not work in full generality,
          // as the following if statement cannot test whether dd_free_global_constants() has also been called.
          // Moverover, in multithreaded environments it would be quite difficult to decide if cddlib was initialised.
          if(!dd_one[0]._mp_num._mp_d)
          {
                  std::cerr<<"CDDLIB HAS NOT BEEN INITIALISED!\n"
                                  "\n"
                                  "Fix this problem by calling the following function in your initialisation code:\n"
                                  "dd_set_global_constants();\n"
                                  "(after possibly setting the gmp allocators) and\n"
                                  "dd_free_global_constants()\n"
                                  "in your deinitialisation code (only available for cddlib version>=094d).\n"
                                  "This requires the header includes:\n"
                                  "#include \"cdd/setoper.h\"\n"
                                  "#include \"cdd/cdd.h\"\n"
                                  "\n"
                                  "Alternatively, you may call gfan:initializeCddlibIfRequired() and deinitializeCddlibIfRequired()\n"
                                  "if gfanlib is the only code using cddlib. If at some point cddlib is no longer required by gfanlib\n"
                                  "these functions may do nothing.\n"
                                  "Because deinitialisation is not possible in cddlib <094d, the functions may leak memory and should not be called often.\n"
                                  "\n"
                                  "This error message will never appear if the initialisation was done properly, and therefore never appear in a shipping version of your software.\n";
                  assert(0);
          }
          /*
          static bool initialized;
          if(!initialized)
      {
                        dd_set_global_constants();
                        initialized=true;
      }*/
  }


class LpSolver
{
  static dd_MatrixPtr ZMatrix2MatrixGmp(ZMatrix const &g, dd_ErrorType *Error)
  {
    int n=g.getWidth();
    dd_MatrixPtr M=NULL;
    dd_rowrange m_input,i;
    dd_colrange d_input,j;
    dd_RepresentationType rep=dd_Inequality;
    // dd_boolean found=dd_FALSE, newformat=dd_FALSE, successful=dd_FALSE;
    char command[dd_linelenmax], comsave[dd_linelenmax];
    dd_NumberType NT;

    (*Error)=dd_NoError;

    rep=dd_Inequality; // newformat=dd_TRUE;

    m_input=g.getHeight();
    d_input=n+1;

    NT=dd_Rational;

    M=dd_CreateMatrix(m_input, d_input);
    M->representation=rep;
    M->numbtype=NT;

    for (i = 0; i < m_input; i++) {
      dd_set_si(M->matrix[i][0],0);
      for (j = 1; j < d_input; j++) {
        g[i][j-1].setGmp(mpq_numref(M->matrix[i][j]));
        mpz_set_ui(mpq_denref(M->matrix[i][j]), 1);
        mpq_canonicalize(M->matrix[i][j]);
      }
    }

    // successful=dd_TRUE;

    return M;
  }
  static dd_MatrixPtr ZMatrix2MatrixGmp(ZMatrix const &inequalities, ZMatrix const &equations, dd_ErrorType *err)
  {
    ZMatrix g=inequalities;
    g.append(equations);
    int numberOfInequalities=inequalities.getHeight();
    int numberOfRows=g.getHeight();
    dd_MatrixPtr A=NULL;
    ensureCddInitialisation();
    A=ZMatrix2MatrixGmp(g, err);
    for(int i=numberOfInequalities;i<numberOfRows;i++)
      set_addelem(A->linset,i+1);
    return A;
  }
  static ZMatrix getConstraints(dd_MatrixPtr A, bool returnEquations)
  {
    int rowsize=A->rowsize;
    int n=A->colsize-1;

    ZMatrix ret(0,n);
    for(int i=0;i<rowsize;i++)
      {
        bool isEquation=set_member(i+1,A->linset);
        if(isEquation==returnEquations)
          {
            QVector v(n);
            for(int j=0;j<n;j++)v[j]=Rational(A->matrix[i][j+1]);
            ret.appendRow(QToZVectorPrimitive(v));
          }
      }
    return ret;
  }
  static bool isFacet(ZMatrix const &g, int index)
  {
    bool ret;
    // dd_MatrixPtr M=NULL,M2=NULL,M3=NULL;
    dd_MatrixPtr M=NULL;
    // dd_colrange d;
    dd_ErrorType err=dd_NoError;
    // dd_rowset redrows,linrows,ignoredrows, basisrows;
    // dd_colset ignoredcols, basiscols;
    // dd_DataFileType inputfile;
    FILE *reading=NULL;

    ensureCddInitialisation();

    M=ZMatrix2MatrixGmp(g, &err);
    if (err!=dd_NoError) goto _L99;

    // d=M->colsize;

    //static dd_Arow temp;
    dd_Arow temp;
    dd_InitializeArow(g.getWidth()+1,&temp);

    ret= !dd_Redundant(M,index+1,temp,&err);

    dd_FreeMatrix(M);
    dd_FreeArow(g.getWidth()+1,temp);

    if (err!=dd_NoError) goto _L99;
    return ret;
   _L99:
    assert(0);
    return false;
  }

  /*
    Heuristic for checking if inequality of full dimensional cone is a
    facet. If the routine returns true then the inequality is a
    facet. If it returns false it is unknown.
   */
  static bool fastIsFacetCriterion(ZMatrix const &normals, int i)
  {
    int n=normals.getWidth();
    for(int j=0;j<n;j++)
      if(normals[i][j].sign()!=0)
        {
          int sign=normals[i][j].sign();
          bool isTheOnly=true;
          for(int k=0;k<normals.getHeight();k++)
            if(k!=i)
              {
                if(normals[i][j].sign()==sign)
                  {
                    isTheOnly=false;
                    break;
                  }
              }
          if(isTheOnly)return true;
        }
    return false;
  }

  static bool fastIsFacet(ZMatrix const &normals, int i)
  {
    if(fastIsFacetCriterion(normals,i))return true;
    return isFacet(normals,i);
  }

  class MyHashMap
  {
    typedef std::vector<std::set<ZVector> > Container;
    Container container;
    int tableSize;
  public:
    class iterator
    {
      class MyHashMap &hashMap;
      int index; // having index==-1 means that we are before/after the elements.
      std::set<ZVector>::iterator i;
    public:
      bool operator++()
      {
        if(index==-1)goto search;
        i++;
        while(i==hashMap.container[index].end())
          {
            search:
            index++;
            if(index>=hashMap.tableSize){
              index=-1;
              return false;
            }
            i=hashMap.container[index].begin();
          }
        return true;
      }
      ZVector const & operator*()const
      {
        return *i;
      }
      ZVector operator*()
      {
        return *i;
      }
      iterator(MyHashMap &hashMap_):
        hashMap(hashMap_)
        {
          index=-1;
        }
    };
    unsigned int function(const ZVector &v)
    {
      unsigned int ret=0;
      int n=v.size();
      for(int i=0;i<n;i++)
        ret=(ret<<3)+(ret>>29)+v.UNCHECKEDACCESS(i).hashValue();
      return ret%tableSize;
    }
    MyHashMap(int tableSize_):
      container(tableSize_),
      tableSize(tableSize_)
      {
        assert(tableSize_>0);
      }
    void insert(const ZVector &v)
    {
      container[function(v)].insert(v);
    }
    void erase(ZVector const &v)
    {
      container[function(v)].erase(v);
    }
    iterator begin()
    {
      iterator ret(*this);
      ++    ret;
      return ret;
    }
    int size()
    {
      iterator i=begin();
      int ret=0;
      do{ret++;}while(++i);
      return ret;
    }
  };


  static ZMatrix normalizedWithSumsAndDuplicatesRemoved(ZMatrix const &a)
  {
    // TODO: write a version of this function which will work faster if the entries fit in 32bit
    if(a.getHeight()==0)return a;
    int n=a.getWidth();
    ZVector temp1(n);
//    ZVector temp2(n);
    ZMatrix ret(0,n);
    MyHashMap b(a.getHeight());

    for(int i=0;i<a.getHeight();i++)
      {
        assert(!(a[i].toVector().isZero()));
        b.insert(a[i].toVector().normalized());
      }

      {
        MyHashMap::iterator i=b.begin();

        do
          {
            MyHashMap::iterator j=i;
            while(++j)
              {
                ZVector const &I=*i;
                ZVector const &J=*j;
                for(int k=0;k<n;k++)temp1[k]=I.UNCHECKEDACCESS(k)+J.UNCHECKEDACCESS(k);
//                normalizedLowLevel(temp1,temp2);
//                b.erase(temp2);//this can never remove *i or *j
                b.erase(temp1.normalized());//this can never remove *i or *j
              }
          }
          while(++i);
      }
    ZMatrix original(0,n);
    {
      MyHashMap::iterator i=b.begin();
      do
        {
          original.appendRow(*i);
        }
      while(++i);
    }

    for(int i=0;i!=original.getHeight();i++)
      for(int j=0;j!=a.getHeight();j++)
        if(!dependent(original[i].toVector(),a[j].toVector()))
            {
              ZVector const &I=original[i];
              ZVector const &J=a[j];
              for(int k=0;k<n;k++)temp1[k]=I.UNCHECKEDACCESS(k)+J.UNCHECKEDACCESS(k);
//              normalizedLowLevel(temp1,temp2);
//              b.erase(temp2);//this can never remove *i or *j
              b.erase(temp1.normalized());//this can never remove *i or *j
            }
        {
          MyHashMap::iterator i=b.begin();
          do
          {
            ZVector temp=*i;
            ret.appendRow(temp);
          }
          while(++i);
      }
    return ret;
  }
public:
  static ZMatrix fastNormals(ZMatrix const &inequalities)
  {
    ZMatrix normals=normalizedWithSumsAndDuplicatesRemoved(inequalities);
    for(int i=0;i!=normals.getHeight();i++)
      if(!fastIsFacet(normals,i))
        {
          normals[i]=normals[normals.getHeight()-1];
          normals.eraseLastRow();
          i--;
        }
    return normals;
  }
  void removeRedundantRows(ZMatrix &inequalities, ZMatrix &equations, bool removeInequalityRedundancies)
  {
          ensureCddInitialisation();

    int numberOfEqualities=equations.getHeight();
    int numberOfInequalities=inequalities.getHeight();
    int numberOfRows=numberOfEqualities+numberOfInequalities;

    if(numberOfRows==0)return;//the full space, so description is already irredundant

    // dd_rowset r=NULL;
    ZMatrix g=inequalities;
    g.append(equations);

    // dd_LPSolverType solver=dd_DualSimplex;
    dd_MatrixPtr A=NULL;
    dd_ErrorType err=dd_NoError;

    A=ZMatrix2MatrixGmp(g,&err);
    if (err!=dd_NoError) goto _L99;

    for(int i=numberOfInequalities;i<numberOfRows;i++)
      set_addelem(A->linset,i+1);

    A->objective=dd_LPmax;

    dd_rowset impl_linset;
    dd_rowset redset;
    dd_rowindex newpos;

    if(removeInequalityRedundancies)
      dd_MatrixCanonicalize(&A, &impl_linset, &redset, &newpos, &err);
    else
      dd_MatrixCanonicalizeLinearity(&A, &impl_linset, &newpos, &err);

    if (err!=dd_NoError) goto _L99;

    {
      int n=A->colsize-1;
      equations=ZMatrix(0,n);     //TODO: the number of rows needed is actually known
      inequalities=ZMatrix(0,n);  //is known by set_card(). That might save some copying.

      {
        int rowsize=A->rowsize;
        QVector point(n);
        for(int i=0;i<rowsize;i++)
          {
            for(int j=0;j<n;j++)point[j]=Rational(A->matrix[i][j+1]);
            ((set_member(i+1,A->linset))?equations:inequalities).appendRow(QToZVectorPrimitive(point));
          }
      }
      assert(set_card(A->linset)==equations.getHeight());
      assert(A->rowsize==equations.getHeight()+inequalities.getHeight());

      set_free(impl_linset);
      if(removeInequalityRedundancies)
        set_free(redset);
      free(newpos);

      dd_FreeMatrix(A);
      return;
    }
   _L99:
    assert(!"Cddlib reported error when called by Gfanlib.");
  }
  ZVector relativeInteriorPoint(const ZMatrix &inequalities, const ZMatrix &equations)
  {
    QVector retUnscaled(inequalities.getWidth());
    ensureCddInitialisation();
    int numberOfEqualities=equations.getHeight();
    int numberOfInequalities=inequalities.getHeight();
    int numberOfRows=numberOfEqualities+numberOfInequalities;

    // dd_rowset r=NULL;
    ZMatrix g=inequalities;
    g.append(equations);

    dd_LPSolverType solver=dd_DualSimplex;
    dd_MatrixPtr A=NULL;
    dd_ErrorType err=dd_NoError;

    A=ZMatrix2MatrixGmp(g,&err);
    if (err!=dd_NoError) goto _L99;
    {
      dd_LPSolutionPtr lps1;
      dd_LPPtr lp,lp1;

      for(int i=0;i<numberOfInequalities;i++)
        dd_set_si(A->matrix[i][0],-1);
      for(int i=numberOfInequalities;i<numberOfRows;i++)
        set_addelem(A->linset,i+1);

      A->objective=dd_LPmax;
      lp=dd_Matrix2LP(A, &err);
      if (err!=dd_NoError) goto _L99;

      lp1=dd_MakeLPforInteriorFinding(lp);
      dd_LPSolve(lp1,solver,&err);
      if (err!=dd_NoError) goto _L99;

      lps1=dd_CopyLPSolution(lp1);

      assert(!dd_Negative(lps1->optvalue));

      for (int j=1; j <(lps1->d)-1; j++)
        retUnscaled[j-1]=Rational(lps1->sol[j]);

      dd_FreeLPData(lp);
      dd_FreeLPSolution(lps1);
      dd_FreeLPData(lp1);
      dd_FreeMatrix(A);
      return QToZVectorPrimitive(retUnscaled);
    }
_L99:
    assert(0);
    return QToZVectorPrimitive(retUnscaled);
  }
  void dual(ZMatrix const &inequalities, ZMatrix const &equations, ZMatrix &dualInequalities, ZMatrix &dualEquations)
  {
    int result;

    dd_MatrixPtr A=NULL;
    dd_ErrorType err=dd_NoError;

    ensureCddInitialisation();

    A=ZMatrix2MatrixGmp(inequalities, equations, &err);

    dd_PolyhedraPtr poly;
    poly=dd_DDMatrix2Poly2(A, dd_LexMin, &err);

    if (poly->child==NULL || poly->child->CompStatus!=dd_AllFound) assert(0);

    dd_MatrixPtr      A2=dd_CopyGenerators(poly);

    dualInequalities=getConstraints(A2,false);
    dualEquations=getConstraints(A2,true);

    dd_FreeMatrix(A2);
    dd_FreeMatrix(A);
    dd_FreePolyhedra(poly);

    return;
   _L99:
    assert(0);
  }
  // this procedure is take from cddio.c.
  static void dd_ComputeAinc(dd_PolyhedraPtr poly)
  {
  /* This generates the input incidence array poly->Ainc, and
     two sets: poly->Ared, poly->Adom.
  */
    dd_bigrange k;
    dd_rowrange i,m1;
    dd_colrange j;
    dd_boolean redundant;
    dd_MatrixPtr M=NULL;
    mytype sum,temp;

    dd_init(sum); dd_init(temp);
    if (poly->AincGenerated==dd_TRUE) goto _L99;

    M=dd_CopyOutput(poly);
    poly->n=M->rowsize;
    m1=poly->m1;

    /* this number is same as poly->m, except when
        poly is given by nonhomogeneous inequality:
        !(poly->homogeneous) && poly->representation==Inequality,
        it is poly->m+1.   See dd_ConeDataLoad.
     */
    poly->Ainc=(set_type*)calloc(m1, sizeof(set_type));
    for(i=1; i<=m1; i++) set_initialize(&(poly->Ainc[i-1]),poly->n);
    set_initialize(&(poly->Ared), m1);
    set_initialize(&(poly->Adom), m1);

    for (k=1; k<=poly->n; k++){
      for (i=1; i<=poly->m; i++){
        dd_set(sum,dd_purezero);
        for (j=1; j<=poly->d; j++){
          dd_mul(temp,poly->A[i-1][j-1],M->matrix[k-1][j-1]);
          dd_add(sum,sum,temp);
        }
        if (dd_EqualToZero(sum)) {
          set_addelem(poly->Ainc[i-1], k);
        }
      }
      if (!(poly->homogeneous) && poly->representation==dd_Inequality){
        if (dd_EqualToZero(M->matrix[k-1][0])) {
          set_addelem(poly->Ainc[m1-1], k);  /* added infinity inequality (1,0,0,...,0) */
        }
      }
    }

    for (i=1; i<=m1; i++){
      if (set_card(poly->Ainc[i-1])==M->rowsize){
        set_addelem(poly->Adom, i);
      }
    }
    for (i=m1; i>=1; i--){
      if (set_card(poly->Ainc[i-1])==0){
        redundant=dd_TRUE;
        set_addelem(poly->Ared, i);
      }else {
        redundant=dd_FALSE;
        for (k=1; k<=m1; k++) {
          if (k!=i && !set_member(k, poly->Ared)  && !set_member(k, poly->Adom) &&
              set_subset(poly->Ainc[i-1], poly->Ainc[k-1])){
            if (!redundant){
              redundant=dd_TRUE;
            }
            set_addelem(poly->Ared, i);
          }
        }
      }
    }
    dd_FreeMatrix(M);
    poly->AincGenerated=dd_TRUE;
  _L99:;
    dd_clear(sum);  dd_clear(temp);
  }


  std::vector<std::vector<int> > extremeRaysInequalityIndices(const ZMatrix &inequalities)
  {
    int dim2=inequalities.getHeight();
    if(dim2==0)return std::vector<std::vector<int> >();
    // int dimension=inequalities.getWidth();

    dd_MatrixPtr A=NULL;
    dd_ErrorType err=dd_NoError;

    ensureCddInitialisation();
    A=ZMatrix2MatrixGmp(inequalities, &err);

    dd_PolyhedraPtr poly;
    poly=dd_DDMatrix2Poly2(A, dd_LexMin, &err);

    if (poly->child==NULL || poly->child->CompStatus!=dd_AllFound) assert(0);
    if (poly->AincGenerated==dd_FALSE) dd_ComputeAinc(poly);

    std::vector<std::vector<int> > ret;

    /*
      How do we interpret the cddlib output?  For a long ting gfan has
      been using poly->n as the number of rays of the cone and thus
      returned sets of indices that actually gave the lineality
      space. The mistake was then caught later in PolyhedralCone. On Feb
      17 2009 gfan was changed to check the length of each set to make
      sure that it does not specify the lineality space and only return
      those sets giving rise to rays.  This does not seem to be the best
      strategy and might even be wrong.
     */


    for (int k=1; k<=poly->n; k++)
      {
        int length=0;
        for (int i=1; i<=poly->m1; i++)
          if(set_member(k,poly->Ainc[i-1]))length++;
        if(length!=dim2)
          {
            std::vector<int> v(length);
            int j=0;
            for (int i=1; i<=poly->m1; i++)
              if(set_member(k,poly->Ainc[i-1]))v[j++]=i-1;
            ret.push_back(v);
          }
      }

    dd_FreeMatrix(A);
    dd_FreePolyhedra(poly);

    return ret;
   _L99:
    assert(0);
    return std::vector<std::vector<int> >();
  }

};

INST_VAR LpSolver lpSolver;

bool ZCone::isInStateMinimum(int s)const
{
  return state>=s;
}


bool operator<(ZCone const &a, ZCone const &b)
{
  assert(a.state>=3);
  assert(b.state>=3);

  if(a.n<b.n)return true;
  if(a.n>b.n)return false;

  if(a.equations<b.equations)return true;
  if(b.equations<a.equations)return false;

  if(a.inequalities<b.inequalities)return true;
  if(b.inequalities<a.inequalities)return false;

  return false;
}


bool operator!=(ZCone const &a, ZCone const &b)
{
  return (a<b)||(b<a);
}


void ZCone::ensureStateAsMinimum(int s)const
{
  if((state<1) && (s==1))
    {
     {
        QMatrix m=ZToQMatrix(equations);
        m.reduce();
        m.removeZeroRows();

        ZMatrix newInequalities(0,inequalities.getWidth());
        for(int i=0;i<inequalities.getHeight();i++)
          {
            QVector w=ZToQVector(inequalities[i]);
            w=m.canonicalize(w);
            if(!w.isZero())
              newInequalities.appendRow(QToZVectorPrimitive(w));
          }

        inequalities=newInequalities;
        inequalities.sortAndRemoveDuplicateRows();
        equations=QToZMatrixPrimitive(m);
      }

      if(!(preassumptions&PCP_impliedEquationsKnown))
      if(inequalities.getHeight()>1)//there can be no implied equation unless we have at least two inequalities
        lpSolver.removeRedundantRows(inequalities,equations,false);

      assert(inequalities.getWidth()==equations.getWidth());
      }
  if((state<2) && (s>=2) && !(preassumptions&PCP_facetsKnown))
    {
/*       if(inequalities.size()>25)
         {
          IntegerVectorList h1;
          IntegerVectorList h2;
          bool a=false;
          for(IntegerVectorList::const_iterator i=inequalities.begin();i!=inequalities.end();i++)
            {
              if(a)
                h1.push_back(*i);
              else
                h2.push_back(*i);
              a=!a;
            }
          PolyhedralCone c1(h1,equations);
          PolyhedralCone c2(h2,equations);
          c1.ensureStateAsMinimum(2);
          c2.ensureStateAsMinimum(2);
          inequalities=c1.inequalities;
          for(IntegerVectorList::const_iterator i=c2.inequalities.begin();i!=c2.inequalities.end();i++)
            inequalities.push_back(*i);
        }
*/
      if(equations.getHeight())
        {
          QMatrix m=ZToQMatrix(equations);
          m.reduce();
          m.REformToRREform();
          ZMatrix inequalities2(0,equations.getWidth());
          for(int i=0;i<inequalities.getHeight();i++)
            {
              inequalities2.appendRow(QToZVectorPrimitive(m.canonicalize(ZToQVector(inequalities[i]))));
            }
          inequalities=LpSolver::fastNormals(inequalities2);
          goto noFallBack;
        fallBack://alternativ (disabled)
          lpSolver.removeRedundantRows(inequalities,equations,true);
        noFallBack:;
        }
      else
        inequalities=LpSolver::fastNormals(inequalities);
    }
  if((state<3) && (s>=3))
    {
      QMatrix equations2=ZToQMatrix(equations);
      equations2.reduce(false,false,true);
      equations2.REformToRREform(true);
      for(int i=0;i<inequalities.getHeight();i++)
        {
          inequalities[i]=QToZVectorPrimitive(equations2.canonicalize(ZToQVector(inequalities[i])));
        }
      inequalities.sortRows();
      equations=QToZMatrixPrimitive(equations2);
    }
  if(state<s)
    state=s;
}

std::ostream &operator<<(std::ostream &f, ZCone const &c)
{
  f<<"Ambient dimension:"<<c.n<<std::endl;
  f<<"Inequalities:"<<std::endl;
  f<<c.inequalities<<std::endl;
  f<<"Equations:"<<std::endl;
  f<<c.equations<<std::endl;
  return f;
}

std::string ZCone::toString()const
{
        std::stringstream f;
        f<<*this;
        return f.str();
}

ZCone::ZCone(int ambientDimension):
  preassumptions(PCP_impliedEquationsKnown|PCP_facetsKnown),
  state(1),
  n(ambientDimension),
  multiplicity(1),
  linearForms(ZMatrix(0,ambientDimension)),
  inequalities(ZMatrix(0,ambientDimension)),
  equations(ZMatrix(0,ambientDimension)),
  haveExtremeRaysBeenCached(false)
{
}


ZCone::ZCone(ZMatrix const &inequalities_, ZMatrix const &equations_, int preassumptions_):
  preassumptions(preassumptions_),
  state(0),
  n(inequalities_.getWidth()),
  multiplicity(1),
  linearForms(ZMatrix(0,inequalities_.getWidth())),
  inequalities(inequalities_),
  equations(equations_),
  haveExtremeRaysBeenCached(false)
  {
  assert(preassumptions_<4);//OTHERWISE WE ARE DOING SOMETHING STUPID LIKE SPECIFYING AMBIENT DIMENSION
  assert(equations_.getWidth()==n);
  ensureStateAsMinimum(1);
}

void ZCone::canonicalize()
{
  ensureStateAsMinimum(3);
}

void ZCone::findFacets()
{
  ensureStateAsMinimum(2);
}

ZMatrix ZCone::getFacets()const
{
  ensureStateAsMinimum(2);
  return inequalities;
}

void ZCone::findImpliedEquations()
{
  ensureStateAsMinimum(1);
}

ZMatrix ZCone::getImpliedEquations()const
{
  ensureStateAsMinimum(1);
  return equations;
}

ZVector ZCone::getRelativeInteriorPoint()const
{
  ensureStateAsMinimum(1);
//  assert(state>=1);

  return lpSolver.relativeInteriorPoint(inequalities,equations);
}

ZVector ZCone::getUniquePoint()const
{
  ZMatrix rays=extremeRays();
  ZVector ret(n);
  for(int i=0;i<rays.getHeight();i++)
    ret+=rays[i];

  return ret;
}

ZVector ZCone::getUniquePointFromExtremeRays(ZMatrix const &extremeRays)const
{
  ZVector ret(n);
  for(int i=0;i<extremeRays.getHeight();i++)
    if(contains(extremeRays[i]))ret+=extremeRays[i];
  return ret;
}


int ZCone::ambientDimension()const
{
  return n;
}


int ZCone::codimension()const
{
  return ambientDimension()-dimension();
}


int ZCone::dimension()const
{
//  assert(state>=1);
  ensureStateAsMinimum(1);
  return ambientDimension()-equations.getHeight();
}


int ZCone::dimensionOfLinealitySpace()const
{
  ZMatrix temp=inequalities;
  temp.append(equations);
  ZCone temp2(ZMatrix(0,n),temp);
  return temp2.dimension();
}


bool ZCone::isOrigin()const
{
  return dimension()==0;
}


bool ZCone::isFullSpace()const
{
  for(int i=0;i<inequalities.getHeight();i++)
    if(!inequalities[i].isZero())return false;
  for(int i=0;i<equations.getHeight();i++)
    if(!equations[i].isZero())return false;
  return true;
}


ZCone intersection(const ZCone &a, const ZCone &b)
{
  assert(a.ambientDimension()==b.ambientDimension());
  ZMatrix inequalities=a.inequalities;
  inequalities.append(b.inequalities);
  ZMatrix equations=a.equations;
  equations.append(b.equations);

  equations.sortAndRemoveDuplicateRows();
  inequalities.sortAndRemoveDuplicateRows();

  {
    ZMatrix Aequations=a.equations;
    ZMatrix Ainequalities=a.inequalities;
    Aequations.sortAndRemoveDuplicateRows();
    Ainequalities.sortAndRemoveDuplicateRows();
    if((Ainequalities.getHeight()==inequalities.getHeight()) && (Aequations.getHeight()==equations.getHeight()))return a;
    ZMatrix Bequations=b.equations;
    ZMatrix Binequalities=b.inequalities;
    Bequations.sortAndRemoveDuplicateRows();
    Binequalities.sortAndRemoveDuplicateRows();
    if((Binequalities.getHeight()==inequalities.getHeight()) && (Bequations.getHeight()==equations.getHeight()))return b;
  }

  return ZCone(inequalities,equations);
}

/*
PolyhedralCone product(const PolyhedralCone &a, const PolyhedralCone &b)
{
  IntegerVectorList equations2;
  IntegerVectorList inequalities2;

  int n1=a.n;
  int n2=b.n;

  for(IntegerVectorList::const_iterator i=a.equations.begin();i!=a.equations.end();i++)
    equations2.push_back(concatenation(*i,IntegerVector(n2)));
  for(IntegerVectorList::const_iterator i=b.equations.begin();i!=b.equations.end();i++)
    equations2.push_back(concatenation(IntegerVector(n1),*i));
  for(IntegerVectorList::const_iterator i=a.inequalities.begin();i!=a.inequalities.end();i++)
    inequalities2.push_back(concatenation(*i,IntegerVector(n2)));
  for(IntegerVectorList::const_iterator i=b.inequalities.begin();i!=b.inequalities.end();i++)
    inequalities2.push_back(concatenation(IntegerVector(n1),*i));

  PolyhedralCone ret(inequalities2,equations2,n1+n2);
  ret.setMultiplicity(a.getMultiplicity()*b.getMultiplicity());
  ret.setLinearForm(concatenation(a.getLinearForm(),b.getLinearForm()));

  ret.ensureStateAsMinimum(a.state);
  ret.ensureStateAsMinimum(b.state);

  return ret;
}*/


ZCone ZCone::positiveOrthant(int dimension)
{
  return ZCone(ZMatrix::identity(dimension),ZMatrix(0,dimension));
}


ZCone ZCone::givenByRays(ZMatrix const &generators, ZMatrix const &linealitySpace)
{
  //rewrite modulo lineality space
/*  ZMatrix newGenerators(generators.getHeight(),generators.getWidth());
  {
    QMatrix l=ZToQMatrix(linealitySpace);
    l.reduce();
    for(int i=0;i<generators.getHeight();i++)
      newGenerators[i]=QToZVectorPrimitive(l.canonicalize(ZToQVector(generators[i])));
  }
*/
//          ZCone dual(newGenerators,linealitySpace);
          ZCone dual(generators,linealitySpace);
//  dual.findFacets();
//  dual.canonicalize();
  ZMatrix inequalities=dual.extremeRays();

/*  ZMatrix span=generators;
  span.append(linealitySpace);
  QMatrix m2Q=ZToQMatrix(span);
  ZMatrix equations=QToZMatrixPrimitive(m2Q.reduceAndComputeKernel());
*/
  ZMatrix equations=dual.generatorsOfLinealitySpace();
//  equations.reduce();equations.removeZeroRows();


  return ZCone(inequalities,equations,PCP_impliedEquationsKnown|PCP_facetsKnown);
}


bool ZCone::containsPositiveVector()const
{
  ZCone temp=intersection(*this,ZCone::positiveOrthant(n));
  return temp.getRelativeInteriorPoint().isPositive();
}


bool ZCone::contains(ZVector const &v)const
{
  for(int i=0;i<equations.getHeight();i++)
    {
      if(!dot(equations[i],v).isZero())return false;
    }
  for(int i=0;i<inequalities.getHeight();i++)
    {
      if(dot(inequalities[i],v).sign()<0)return false;
    }
  return true;
}


bool ZCone::containsRowsOf(ZMatrix const &m)const
{
  for(int i=0;i<m.getHeight();i++)
    if(!contains(m[i]))return false;
  return true;
}


bool ZCone::contains(ZCone const &c)const
{
  ZCone c2=intersection(*this,c);
  ZCone c3=c;
  c2.canonicalize();
  c3.canonicalize();
  return !(c2!=c3);
}


bool ZCone::containsRelatively(ZVector const &v)const
{
  ensureStateAsMinimum(1);
//  assert(state>=1);
  for(int i=0;i<equations.getHeight();i++)
    {
      if(!dot(equations[i],v).isZero())return false;
    }
  for(int i=0;i<inequalities.getHeight();i++)
    {
      if(dot(inequalities[i],v).sign()<=0)return false;
    }
  return true;
}


bool ZCone::isSimplicial()const
{
//  assert(state>=2);
  ensureStateAsMinimum(2);
  return codimension()+inequalities.getHeight()+dimensionOfLinealitySpace()==n;
}


ZCone ZCone::linealitySpace()const
{
  ZCone ret(ZMatrix(0,n),combineOnTop(equations,inequalities));
//  ret.ensureStateAsMinimum(state);
  return ret;
}


ZCone ZCone::dualCone()const
{
  ensureStateAsMinimum(1);
//  assert(state>=1);

  ZMatrix dualInequalities,dualEquations;
  lpSolver.dual(inequalities,equations,dualInequalities,dualEquations);
  ZCone ret(dualInequalities,dualEquations);
  ret.ensureStateAsMinimum(state);

  return ret;
}


ZCone ZCone::negated()const
{
  ZCone ret(-inequalities,equations,(areFacetsKnown()?PCP_facetsKnown:0)|(areImpliedEquationsKnown()?PCP_impliedEquationsKnown:0));
//  ret.ensureStateAsMinimum(state);
  return ret;
}


ZMatrix ZCone::extremeRays(ZMatrix const *generatorsOfLinealitySpace)const
{
//  assert((dimension()==ambientDimension()) || (state>=3));
  if(dimension()!=ambientDimension())
    ensureStateAsMinimum(3);

  if(haveExtremeRaysBeenCached)return cachedExtremeRays;
  ZMatrix ret(0,n);
  std::vector<std::vector<int> > indices=lpSolver.extremeRaysInequalityIndices(inequalities);

  for(unsigned i=0;i<indices.size();i++)
    {
      /* At this point we know lineality space, implied equations and
         also inequalities for the ray. To construct a vector on the
         ray which is stable under (or independant of) angle and
         linearity preserving transformation we find the dimension 1
         subspace orthogonal to the implied equations and the
         lineality space and pick a suitable primitive generator */

          /* To be more precise,
           * let E be the set of equations, and v the inequality defining a  ray R.
           * We wish to find a vector satisfying these, but it must also be orthogonal
           * to the lineality space of the cone, that is, in the span of {E,v}.
           * One way to get such a vector is to project v to E an get a vector p.
           * Then v-p is in the span of {E,v} by construction.
           * The vector v-p is also in the orthogonal complement to E by construction,
           * that is, the span of R.
           * We wish to argue that it is not zero.
           * That would imply that v=p, meaning that v is in the span of the equations.
           * However, that would contradict that R is a ray.
           * In case v-p does not satisfy the inequality v (is this possible?), we change the sign.
           *
           * As a consequence we need the following procedure
           * primitiveProjection():
           *    Input: E,v
           *    Output: A primitive representation of the vector v-p, where p is the projection of v onto E
           *
           * Notice that the output is a Q linear combination of the input and that p is
           * a linear combination of E. The check that p has been computed correctly,
           * it suffices to check that v-p satisfies the equations E.
           * The routine will actually first compute a multiple of v-p.
           * It will do this using floating point arithmetics. It will then transform
           * the coefficients to get the multiple of v-p into integers. Then it
           * verifies in exact arithmetics, that with these coefficients we get a point
           * satisfying E. It then returns the primitive vector on the ray v-p.
           * In case of a failure it falls back to an implementation using rational arithmetics.
           */


          std::vector<int> asVector(inequalities.getHeight());
          for(unsigned j=0;j<indices[i].size();j++){asVector[indices[i][j]]=1;}
          ZMatrix equations=this->equations;
          ZVector theInequality;

          for(unsigned j=0;j<asVector.size();j++)
            if(asVector[j])
              equations.appendRow(inequalities[j]);
            else
              theInequality=inequalities[j];

          assert(!theInequality.isZero());

          ZVector thePrimitiveVector;
          if(generatorsOfLinealitySpace)
          {
            QMatrix temp=ZToQMatrix(combineOnTop(equations,*generatorsOfLinealitySpace));
            thePrimitiveVector=QToZVectorPrimitive(temp.reduceAndComputeVectorInKernel());
          }
          else
          {
            QMatrix linealitySpaceOrth=ZToQMatrix(combineOnTop(this->equations,inequalities));


            QMatrix temp=combineOnTop(linealitySpaceOrth.reduceAndComputeKernel(),ZToQMatrix(equations));
            thePrimitiveVector=QToZVectorPrimitive(temp.reduceAndComputeVectorInKernel());
          }
          if(!contains(thePrimitiveVector))thePrimitiveVector=-thePrimitiveVector;
          ret.appendRow(thePrimitiveVector);
    }

  cachedExtremeRays=ret;
  haveExtremeRaysBeenCached=true;

  return ret;
}


Integer ZCone::getMultiplicity()const
{
  return multiplicity;
}


void ZCone::setMultiplicity(Integer const &m)
{
  multiplicity=m;
}


ZMatrix ZCone::getLinearForms()const
{
  return linearForms;
}


void ZCone::setLinearForms(ZMatrix const &linearForms_)
{
  linearForms=linearForms_;
}


ZMatrix ZCone::quotientLatticeBasis()const
{
//  assert(isInStateMinimum(1));// Implied equations must have been computed in order to know the span of the cone
  ensureStateAsMinimum(1);


  int a=equations.getHeight();
  int b=inequalities.getHeight();

  // Implementation below could be moved to nonLP part of code.

  // small vector space defined by a+b equations.... big by a equations.

  ZMatrix M=combineLeftRight(combineLeftRight(
                                                  equations.transposed(),
                                                  inequalities.transposed()),
                                 ZMatrix::identity(n));
  M.reduce(false,true);
  /*
    [A|B|I] is reduced to [A'|B'|C'] meaning [A'|B']=C'[A|B] and A'=C'A.

    [A'|B'] is in row echelon form, implying that the rows of C' corresponding to zero rows
    of [A'|B'] generate the lattice cokernel of [A|B] - that is the linealityspace intersected with Z^n.

    [A'] is in row echelon form, implying that the rows of C' corresponding to zero rows of [A'] generate
    the lattice cokernel of [A] - that is the span of the cone intersected with Z^n.

    It is clear that the second row set is a superset of the first. Their difference is a basis for the quotient.
   */
  ZMatrix ret(0,n);

  for(int i=0;i<M.getHeight();i++)
    if(M[i].toVector().subvector(0,a).isZero()&&!M[i].toVector().subvector(a,a+b).isZero())
      {
        ret.appendRow(M[i].toVector().subvector(a+b,a+b+n));
      }

  return ret;
}


ZVector ZCone::semiGroupGeneratorOfRay()const
{
  ZMatrix temp=quotientLatticeBasis();
  assert(temp.getHeight()==1);
  for(int i=0;i<inequalities.getHeight();i++)
    if(dot(temp[0].toVector(),inequalities[i].toVector()).sign()<0)
      {
        temp[0]=-temp[0].toVector();
        break;
      }
  return temp[0];
}


ZCone ZCone::link(ZVector const &w)const
{
  /* Observe that the inequalities giving rise to facets
   * also give facets in the link, if they are kept as
   * inequalities. This means that the state cannot decrease
   * when taking links - that is why we specify the PCP flags.
   */
  ZMatrix inequalities2(0,n);
  for(int j=0;j<inequalities.getHeight();j++)
    if(dot(w,inequalities[j]).sign()==0)inequalities2.appendRow(inequalities[j]);
  ZCone C(inequalities2,equations,(areImpliedEquationsKnown()?PCP_impliedEquationsKnown:0)|(areFacetsKnown()?PCP_facetsKnown:0));
  C.ensureStateAsMinimum(state);

  C.setLinearForms(getLinearForms());
  C.setMultiplicity(getMultiplicity());

  return C;
}

bool ZCone::hasFace(ZCone const &f)const
{
  if(!contains(f.getRelativeInteriorPoint()))return false;
  ZCone temp=faceContaining(f.getRelativeInteriorPoint());
  temp.canonicalize();
//  ZCone temp2=*this;
  ZCone temp2=f;
  temp2.canonicalize();
//  std::cout << temp << std::endl;
//  std::cout << temp2 << std::endl;

  return !(temp2!=temp);
}

ZCone ZCone::faceContaining(ZVector const &v)const
{
  assert(n==(int)v.size());
  assert(contains(v));
  ZMatrix newEquations=equations;
  ZMatrix newInequalities(0,n);
  for(int i=0;i<inequalities.getHeight();i++)
    if(dot(inequalities[i],v).sign()!=0)
      newInequalities.appendRow(inequalities[i]);
    else
      newEquations.appendRow(inequalities[i]);

  ZCone ret(newInequalities,newEquations,(state>=1)?PCP_impliedEquationsKnown:0);
  ret.ensureStateAsMinimum(state);
  return ret;
}


ZMatrix ZCone::getInequalities()const
{
  return inequalities;
}


ZMatrix ZCone::getEquations()const
{
  return equations;
}


ZMatrix ZCone::generatorsOfSpan()const
{
  ensureStateAsMinimum(1);
  QMatrix l=ZToQMatrix(equations);
  return QToZMatrixPrimitive(l.reduceAndComputeKernel());
}


ZMatrix ZCone::generatorsOfLinealitySpace()const
{
  QMatrix l=ZToQMatrix(combineOnTop(inequalities,equations));
  return QToZMatrixPrimitive(l.reduceAndComputeKernel());
}

}
