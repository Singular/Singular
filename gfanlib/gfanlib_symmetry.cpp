/*
 * gfanlib_symmetry.cpp
 *
 *  Created on: Oct 22, 2010
 *      Author: anders
 */
#include <stddef.h>
#include "gfanlib_symmetry.h"
#include <map>

using namespace std;

namespace gfan{
class Trie
{
        class TrieNode
        {
                typedef map<int,class TrieNode> Map;
                Map m;
        public:
                TrieNode()
                {

                }
                TrieNode(IntVector const &v, int i)
                {
                  if(i<(int)v.size())
                        m[v[i]]=TrieNode(v,i+1);
                }
                int stabilizerSize(ZVector const &v, int i)const
                {
                  int ret=0;
                  if(i==(int)v.size())return 1;
                  for(Map::const_iterator j=m.begin();j!=m.end();j++)
                    {
                      if(v[i]==v[j->first])
                        ret+=j->second.stabilizerSize(v,i+1);
                    }
                  return ret;
                }
                void search(ZVector const &v, ZVector  &building, Permutation &tempPerm, Permutation &ret, ZVector &optimal, int i, bool &isImproving)const
                {
                  if(i==(int)v.size()){ret=tempPerm;optimal=building;isImproving=false;return;}
                        if(isImproving)
                                building[i]=-0x7fffffff;
                        else
                                building[i]=optimal[i];
                        for(Map::const_iterator j=m.begin();j!=m.end();j++)
                                if(building[i]<v[j->first])
                                {
                                        isImproving=true;
                                        building[i]=v[j->first];
                                }
                                for(Map::const_iterator j=m.begin();j!=m.end();j++)
                                if(v[j->first]==building[i])
                                {
                                        tempPerm[i]=j->first;
                                        j->second.search(v,building,tempPerm,ret,optimal,i+1,isImproving);
                                }
                }
                void searchStabalizer(ZVector const &v, ZVector  &building, Permutation &tempPerm, Permutation &ret, ZVector &optimal, int i, bool &isImproving, ZVector const &toBeFixed)const
                {
                  if(i==(int)v.size())
                                if(!(tempPerm.apply(v)<optimal))
                                        {
                                        ret=tempPerm;
                                        optimal=tempPerm.apply(v);
                                        return;
                                        }
                                for(Map::const_iterator j=m.begin();j!=m.end();j++)
                                        if(toBeFixed[i]==toBeFixed[j->first])
                                {
                                        tempPerm[i]=j->first;
                                        j->second.searchStabalizer(v,building,tempPerm,ret,optimal,i+1,isImproving,toBeFixed);
                                }
                }
/* this code contains mistakes          void searchStabalizer(IntegerVector const &v, IntegerVector  &building, IntegerVector &tempPerm, IntegerVector &ret, IntegerVector &optimal, int i, bool &isImproving, IntegerVector const &toBeFixed)const
                {
                        if(i==v.size()){ret=tempPerm;optimal=building;isImproving=false;debug<<"DEEP";return;}
                        if(isImproving)
                                building[i]=-0x7fffffff;
                        else
                                building[i]=optimal[i];
                        for(Map::const_iterator j=m.begin();j!=m.end();j++)
                                if(toBeFixed[i]==toBeFixed[j->first])
                                if(v[j->first]>building[i])
                                {
                                        isImproving=true;
                                        building[i]=v[j->first];
                                }
                                for(Map::const_iterator j=m.begin();j!=m.end();j++)
                                        if(toBeFixed[i]==toBeFixed[j->first])
                                if(v[j->first]==building[i])
                                {
                                        debug.printInteger(i);debug<<":";
                                        debug.printInteger(j->first);debug<<" ";
                                        tempPerm[i]=j->first;
                                        j->second.searchStabalizer(v,building,tempPerm,ret,optimal,i+1,isImproving,toBeFixed);
                                }
                }*/
        //      void doubleSearch();
                void insert(Permutation const &v, int i)
                {
                  if(i==(int)v.size())return;
                        if(m.count(v[i]))
                                m[v[i]].insert(v,i+1);
                        else
                                m[v[i]]=                TrieNode(v,i+1);
                }
/*                void print(int i, int n)const
                {
                        if(i==n)return;
                        for(Map::const_iterator j=m.begin();j!=m.end();j++)
                        {
                                {for(int j=0;j<2*i;j++)debug<<" ";}
                                debug.printInteger(j->first);
                                debug<<"\n";
                                j->second.print(i+1,n);
                        }
                        }*/
                int size(int i,int n)const
                {
                        if(i==n)return 1;
                        int ret=0;
                        for(Map::const_iterator j=m.begin();j!=m.end();j++)
                                ret+=j->second.size(i+1,n);
                        return ret;
                }
        };
public:
        TrieNode theTree;
        int n;
        Trie(int n_):
                theTree(Permutation(n_),0),
                n(n_)
        {
        }
        int size()const
        {
                return theTree.size(0,n);
        }
        void insert(Permutation const &v)
        {
                theTree.insert(v,0);
//              debug<<v;
//              theTree.print(0,v.size());

//              debug<<"---------------------------------------------\n";
        }
        /**
         * returns the sigma from the set with sigma(v) maximal in the lexicographic ordering.
         */
        Permutation search(ZVector const &v)
        {
                Permutation tempPerm(v.size());
                Permutation ret(v.size());
                ZVector building(v.size());
                ZVector optimal=v;//the identity is always in the trie
                bool isImproving=true;
                theTree.search(v,building,tempPerm,ret,optimal,0,isImproving);
                return ret;
        }
        Permutation searchStabalizer(ZVector const &v, ZVector const &toBeFixed)
        {
                Permutation tempPerm(v.size());
                Permutation ret(v.size());
                ZVector building(v.size());
                ZVector optimal=v;//the identity is always in the trie
                bool isImproving=true;
                theTree.searchStabalizer(v,building,tempPerm,ret,optimal,0,isImproving,toBeFixed);
                return ret;
        }
        int stabilizerSize(ZVector const &v)const
        {
          return theTree.stabilizerSize(v,0);
        }
};







/*IntegerVector SymmetryGroup::identity(int n)
{
  IntegerVector v(n);
  for(int i=0;i<n;i++)v[i]=i;

  return v;
}
*/

Permutation Permutation::inverse()const
{
  return applyInverse(Permutation(size()));
}


SymmetryGroup::SymmetryGroup(int n):
//  byteTable(0),
  trie(0)
{
  elements.insert(Permutation(n));
}


int SymmetryGroup::sizeOfBaseSet()const
{
  assert(!elements.empty());
  return elements.begin()->size();
}

IntMatrix SymmetryGroup::getGenerators()const
{
  IntMatrix ret(0,this->sizeOfBaseSet());
  for(ElementContainer::const_iterator i=elements.begin();i!=elements.end();i++)ret.appendRow(i->toIntVector());
  return ret;
}

void SymmetryGroup::computeClosure(Permutation const &v) //does this work??
{
  ElementContainer newOnes;

  newOnes.insert(v);

  while(!newOnes.empty())
    {
      Permutation v=*newOnes.begin();
      for(ElementContainer::const_iterator i=elements.begin();i!=elements.end();i++)
        {
          {
            Permutation n=i->apply(v);
            if(0==elements.count(n))
              newOnes.insert(n);
          }
          {
            Permutation n=v.apply(v);
            if(0==elements.count(n))
              newOnes.insert(n);
          }
        }
      newOnes.erase(v);
      elements.insert(v);
    }
}


void SymmetryGroup::computeClosure(IntMatrix const &l)
{
  for(int i=0;i<l.getHeight();i++)computeClosure(Permutation(l[i]));
}


/*
void SymmetryGroup::print(FILE *f)
{
  AsciiPrinter P(f);
  P.printString("Printing SymmetryGroup\n");
  IntegerVectorList l;
  for(ElementContainer::const_iterator i=elements.begin();i!=elements.end();i++)
    {
      //      P.printVector(*i);
      //      P.printNewLine();
      l.push_back(*i);
    }
  P.printVectorList(l);
  fprintf(f,"Group order=%i\n",elements.size());
  P.printString("Done printing SymmetryGroup.\n");
}
*/


Permutation Permutation::apply(Permutation const &b)const
{
  IntVector ret(size());
  assert(size()==b.size());
  for(unsigned i=0;i<size();i++)ret[i]=b[(*this)[i]];
  return Permutation(ret);
}

Permutation Permutation::applyInverse(Permutation const &b)const
{
  IntVector ret(size());
  assert(size()==b.size());
  for(unsigned i=0;i<size();i++)ret[(*this)[i]]=b[i];
  return Permutation(ret);
}

IntVector Permutation::apply(IntVector const &v)const
{
  IntVector ret(size());
  assert(size()==v.size());
  for(unsigned i=0;i<size();i++)ret[i]=v[(*this)[i]];
  return ret;
}

ZVector Permutation::apply(ZVector const &v)const
{
  ZVector ret(size());
  assert(size()==v.size());
  for(unsigned i=0;i<size();i++)ret[i]=v[(*this)[i]];
  return ret;
}


ZVector Permutation::applyInverse(ZVector const &v)const
{
  ZVector ret(size());
  assert(size()==v.size());
  for(unsigned i=0;i<size();i++)ret[(*this)[i]]=v[i];
  return ret;
}
//ZVector apply(ZVector const &v)const;
//ZMatrix apply(ZMatrix const &m)const;
//IntVector applyInverse(IntVector const &v)const;
//ZMatrix applyInverse(ZMatrix const &m)const;

ZVector SymmetryGroup::orbitRepresentative(ZVector const &v, Permutation *usedPermutation)const
{
  if(trie){
    if(usedPermutation)
      {
        *usedPermutation=trie->search(v);
        return usedPermutation->apply(v);
      }
    return trie->search(v).apply(v);
  }
  ZVector ret=v;
  ElementContainer::const_iterator usedPerm;
  for(ElementContainer::const_iterator i=elements.begin();i!=elements.end();i++)
    {
      ZVector q=i->apply(v);
      if(! (q<ret))//negation to make sure that usedPerm is set
        {
          usedPerm=i;
          ret=q;
        }
    }

  if(usedPermutation)*usedPermutation=*usedPerm;

  if(trie)
  {
//        debug<<"Input"<<v<<"\n";
//        debug<<"Bruteforce"<<ret<<"\n";
          Permutation triePerm=trie->search(v);
//        debug<<"Trie"<<compose(triePerm,v)<<"\n";
          assert((triePerm.apply(v)-ret).isZero());
  }

  return ret;
}

ZVector SymmetryGroup::orbitRepresentativeFixing(ZVector const &v, ZVector const &fixed)const
{
        if(trie){
                return trie->searchStabalizer(v,fixed).apply(v);
        }
  ZVector ret=v;

  for(ElementContainer::const_iterator i=elements.begin();i!=elements.end();i++)
    if(i->apply(fixed)==fixed)
      {
        ZVector q=i->apply(v);
        if(ret<q)ret=q;
      }
        if(trie){
                ZVector temp=trie->searchStabalizer(v,fixed).apply(v);
//              debug<<"Input"<<v;
//              debug<<"Brute"<<ret;
//              debug<<"Quick"<<temp;
                assert((temp-ret).isZero());
//              return compose(trie->searchStabalizer(v,fixed),v);
        }
  return ret;
}

bool Permutation::isPermutation(IntVector const &a)
{
  int n=a.size();
  IntVector temp(n);
  for(int i=0;i<n;i++)temp[i]=-1;
  for(int i=0;i<n;i++)
    {
      if(a[i]<0 || a[i]>=n)return false;
      temp[i]=i;
    }
  for(int i=0;i<n;i++)if(temp[i]<0)return false;
  return true;
}


bool Permutation::arePermutations(IntMatrix const &m)
{
  for(int i=0;i<m.getHeight();i++)if(!isPermutation(m[i]))return false;
  return true;
}

int SymmetryGroup::orbitSize(ZVector const &stable)const
{
  int groupSize=elements.size();

  int n=stable.size();
  int numFixed=0;

  if(trie)
    {
      numFixed=trie->stabilizerSize(stable);
    }
  else
    {
      for(SymmetryGroup::ElementContainer::const_iterator j=elements.begin();j!=elements.end();j++)
        {
          bool doesFix=true;

          for(int i=0;i<n;i++)
            if(stable[i]!=stable[(*j)[i]])
              {
                doesFix=false;
                break;
              }
          if(doesFix)numFixed++;
        }
    }
  return groupSize/numFixed;
}


ZVector Permutation::fundamentalDomainInequality()const
{
  for(unsigned i=0;i<size();i++)
    if((*this)[i]!=(int)i)
      return ZVector::standardVector(size(),i)-ZVector::standardVector(size(),(*this)[i]);
  return ZVector(size());
}


ZMatrix SymmetryGroup::fundamentalDomainInequalities()const
{
  set<ZVector> ret2;
  for(ElementContainer::const_iterator i=elements.begin();i!=elements.end();i++)
    ret2.insert(i->fundamentalDomainInequality());

  ZMatrix ret(0,sizeOfBaseSet());
  for(set<ZVector>::const_iterator i=ret2.begin();i!=ret2.end();i++)
    if(!i->isZero())ret.appendRow(*i);

  return ret;
}


void SymmetryGroup::createTrie()
{
  trie=new Trie(sizeOfBaseSet());
  for(ElementContainer::const_iterator i=elements.begin();i!=elements.end();i++)
    trie->insert(*i);
}



bool SymmetryGroup::isTrivial()const
{
  ElementContainer::const_iterator i=elements.begin();
  assert(i!=elements.end());
  i++;
  return i==elements.end();
}
#if 0
static int mergeSortRek(IntegerVector &v, int begin, int end, IntegerVector &temp)
{
  if(end-begin<2)return 0;
  int med=(begin+end)>>1;
  int nswaps=mergeSortRek(v,begin,med,temp);
  nswaps+=mergeSortRek(v,med,end,temp);

  {
    int Astart=begin;
    int Alength=med-begin;
    int Bstart=med;
    int Blength=end-med;
    int nextFree=begin;
    while(nextFree!=end)
      {
//        debug<<"Astart:"<<Astart<<"Alength:"<<Alength<<"Bstart:"<<Bstart<<"Blength:"<<Blength<<"nextFree:"<<nextFree<<"\n";
        if(Blength==0 || (Alength!=0 && v[Astart]<v[Bstart]))
          {
            temp[nextFree++]=v[Astart++];
            Alength--;
          }
        else
          {
            temp[nextFree++]=v[Bstart++];
            nswaps+=Alength;
            Blength--;
          }
      }
    for(int i=begin;i!=end;i++)v[i]=temp[i];
  }
//debug<<"return\n";
  return nswaps;
}

int mergeSort(IntegerVector &v)
{
  IntegerVector temp(v.size());
  return mergeSortRek(v,0,v.size(),temp);
}
#endif
}
