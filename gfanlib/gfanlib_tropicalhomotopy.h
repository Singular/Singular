/*
 * gfanlib_tropicalhomotopy.h
 *
 *  Created on: Apr 10, 2016
 *      Author: anders
 */

#ifndef GFANLIB_TROPICALHOMOTOPY_H_
#define GFANLIB_TROPICALHOMOTOPY_H_

#include "gfanlib_paralleltraverser.h"
#include "gfanlib_matrix.h"

namespace gfan{

class Flags{
public:
         static const bool computeDotProductInMatrix=true;
 };

/**
 * We identify six possibly different types needed with possibly varying precission:
 * 1) The entries of the circuits (or possibly their packed representation)
 * 2) The mixed volume contribution of a single cell. This is obtained from an entry of a circuit and therefore can be represented by the above type.
 * 3) The accumulated mixed volume. This will exceed the bound of the above type in many cases. Overflows are easily checked.
 * 4) The type that dotVector uses as a result when dotting with the target. (Also used in campareInequalities)
 * 5) The intermediate type for dotVector.
 * 6) The type used in compareRevLexInverted
 *
 *
 * Type 1 and 2 are the same.
 * Type 3 is typically different.
 *
 * To simplify our design:
 *  we assume that type 4 is the same as 1 and 2. This is reasonable, as we need some bound to make type 6 efficient.
 *  we use a special (longer) type for 5, as that allows to do overflow checks at the end, assuming some bound on the target.
 *  In 6, we observe that there is no accumulation taking place. Moreover, with the assumption that 4 and 1 are the same, we only need a type with double precission to do the comparisons here, and now overflow check will be required.
 *
 *
 * To conclude, we make two types. A single precision type for 1,2,4 and a double precision type for 3,5,6
 * We further need to make assumptions on the absolute value of the entries of the target vector and the number of entries in combination to ensure that dot product computations do not overflow.
 * Overflow checks are then only needed:
 *  when casting the return value of dotVector
 *  when doing the dotDivVector operation. But since bounds are known, in most cases checks are not needed
 *  when accumulating the mixed volume
 *
 *
 *  Suggested implementations:
 *   a pair of 32/64 bit integers with only the overflow checking listed above
 *   a pair of gmp integers for exact precision.
 */





template<class mvtyp>
static Matrix<mvtyp> simplex(int n, mvtyp d)
{
          Matrix<mvtyp> ret(n,n+1);
          for(int i=0;i<n;i++)ret[i][i+1]=d;
          return ret;
}



template<class mvtyp, class mvtypDouble, class mvtypDivisor>
        class SingleTropicalHomotopyTraverser{
                 class InequalityComparisonResult{//actual comparison functions were moved to the InequalityTable
                 public:
                          bool empty;
                          int configurationIndex;//used for finding best
                          int columnIndex;//used for finding best
                 };
        class InequalityTable //circuit table // This table has been moved inside the IntegersectionTraverser simply because it is used nowhere else and is specific to mixed cells in Cayley configurations.
         {
                /* All methods are marked to show if they can overflow without throwing/asserting.
                 * Overflowing methods at the moment are:
                 *  replaceFirstOrSecond:       subroutine calls may overflow (dotDivVector)
                 *  compareInequalities:           only if target entries are too big
                 *  dotVector:                     only if target entries are too big
                 *  setChoicesFromEarlierHomotopy: only if tuple entries are too big
                 */
                std::vector<Matrix<mvtyp> > tuple;
                std::vector<int> offsets;
                std::vector<std::pair<int,int> > choices;
                Matrix<mvtyp> A;//one row for each potential inequality, to store entries with indices in chosen
                Vector<mvtyp> tempA;
                Vector<mvtyp> Abounds;// a negative bound for each row of A, bounding the absolute value of the rows;
                std::vector<mvtyp> svec;//used locally
                int subconfigurationIndex;
                mvtyp denominator;
                int m;
                int k;
                bool isLegalIndex(int subconfigurationIndex, int columnIndex)const
                {// Cannot overflow
                        return choices[subconfigurationIndex].first!=columnIndex && choices[subconfigurationIndex].second!=columnIndex;
                }
                  mvtyp dotVector(int subconfigurationIndex, int columnIndex, Vector<mvtyp> const &target, int onlyK=-1)const
                  {   // May overflow if entries of target are too big.
                          //if onlyK!=-1 then only the onlyKth subconfiguration is considered
                          mvtypDouble ret;
                          if(onlyK!=-1)
                          {
                                  if(onlyK==subconfigurationIndex)
                                  {
                                          int i=subconfigurationIndex;
                                          ret+=extendedMultiplication(A.UNCHECKEDACCESS(i,columnIndex+offsets[subconfigurationIndex]),target.UNCHECKEDACCESS((choices)[i].second+offsets[i]));
                                          ret-=extendedMultiplication(A.UNCHECKEDACCESS(i,columnIndex+offsets[subconfigurationIndex]),target.UNCHECKEDACCESS((choices)[i].first+offsets[i]));
                                          ret-=extendedMultiplication(denominator,target.UNCHECKEDACCESS((choices)[i].first+offsets[i]));// the multiplication can be merged with multiplication above except that that could cause and overflow.
                                          ret+=extendedMultiplication(denominator,target.UNCHECKEDACCESS(columnIndex+offsets[i]));
                                          return ret.castToSingle();
                                  }
                                  else
                                  {
                                          int i=onlyK;
                                          if(target.UNCHECKEDACCESS((choices)[i].first+offsets[i]).isNonZero())
                                          {
                                                  ret+=extendedMultiplication(A.UNCHECKEDACCESS(i,columnIndex+offsets[subconfigurationIndex]),target.UNCHECKEDACCESS((choices)[i].second+offsets[i]));
                                                  ret-=extendedMultiplication(A.UNCHECKEDACCESS(i,columnIndex+offsets[subconfigurationIndex]),target.UNCHECKEDACCESS((choices)[i].first+offsets[i]));
                                          }
                                          return ret.castToSingle();
                                  }
                          }
                          for(int i=0;i<tuple.size();i++)
                          {
                                  ret+=extendedMultiplication(A.UNCHECKEDACCESS(i,columnIndex+offsets[subconfigurationIndex]),target.UNCHECKEDACCESS((choices)[i].second+offsets[i]));
                                  if(i==subconfigurationIndex)
                                  {
                                          ret-=extendedMultiplication(A.UNCHECKEDACCESS(i,columnIndex+offsets[subconfigurationIndex]),target.UNCHECKEDACCESS((choices)[i].first+offsets[i]));
                                          ret-=extendedMultiplication(denominator,target.UNCHECKEDACCESS((choices)[i].first+offsets[i]));// the multiplication can be merged with multiplication above except that that could cause and overflow.
                                          ret+=extendedMultiplication(denominator,target.UNCHECKEDACCESS(columnIndex+offsets[i]));
                                  }
                                  else
                                  {
                                          ret-=extendedMultiplication(A.UNCHECKEDACCESS(i,columnIndex+offsets[subconfigurationIndex]),target.UNCHECKEDACCESS((choices)[i].first+offsets[i]));
                                  }
                          }
                          return ret.castToSingle();
                  }
                  void assignDotProducts(Vector<mvtyp> const &target, int onlyK=-1)
                  {// Cannot overflow
                          int J=0;
                          for(int i=0;i<k;i++)
                                  for(int j=0;j<tuple[i].getWidth();j++,J++)
                                          A[k][J]=dotVector(i,j,target,onlyK);
                  }
                  bool isReverseLexInvertedLessThanZero(int subconfigurationIndex, int columnIndex)const __attribute__ ((always_inline))//As in ReverseLexicographicInvertedTermOrder. Compare against zero
                  {// Cannot overflow
                          int i;
                          int index=columnIndex+offsets[subconfigurationIndex];
                          for(i=0;i<subconfigurationIndex;i++)
                                  if(A.UNCHECKEDACCESS(i,index).isNonZero())
                                  {
                                          if(choices[i].first<choices[i].second)
                                                  return A.UNCHECKEDACCESS(i,index).isNegative();
                                          else
                                                  return A.UNCHECKEDACCESS(i,index).isPositive();
                                  }

                                  mvtyp a=A.UNCHECKEDACCESS(i,index);
                                  {
                                          int firstIndex=choices[i].first;
                                          int secondIndex=choices[i].second;
                                          int thirdIndex=columnIndex;
                                          mvtyp firstValue=-a-denominator;
                                          mvtyp secondValue=a;
                                          mvtyp thirdValue=denominator;

                                          // Bubble sort
                                          if(secondIndex<firstIndex)
                                          {
                                                  std::swap(secondIndex,firstIndex);
                                                  std::swap(secondValue,firstValue);
                                          }
                                          if(thirdIndex<secondIndex)
                                          {
                                                  std::swap(secondIndex,thirdIndex);
                                                  std::swap(secondValue,thirdValue);
                                          }
                                          if(secondIndex<firstIndex)
                                          {
                                                  std::swap(secondIndex,firstIndex);
                                                  std::swap(secondValue,firstValue);
                                          }

                                          if(firstValue.isNonZero())
                                                  return firstValue.isPositive();
                                          if(secondValue.isNonZero())
                                                  return secondValue.isPositive();
                                          if(thirdValue.isNonZero())
                                                  return thirdValue.isPositive();
                                  }
                                  i++;
                                  for(;i<k;i++)
                                          if(A.UNCHECKEDACCESS(i,index).isNonZero())
                                          {
                                                  if(choices[i].first<choices[i].second)
                                                          return A.UNCHECKEDACCESS(i,index).isNegative();
                                                  else
                                                          return A.UNCHECKEDACCESS(i,index).isPositive();
                                          }


                                  return false;
                  }
         public:
                        void computeABounds()
                        {//Cannot overflow
                                for(int i=0;i<A.getHeight();i++)
                                        Abounds[i]=mvtyp::computeNegativeBound(&(A[i][0]),A.getWidth());
                        }
                        void checkABounds()const//remove?
                        {//Cannot overflow
                                for(int i=0;i<A.getHeight();i++)
                                {
                                        mvtyp M=0;
                                        mvtyp m=0;
                                        for(int j=0;j<A.getWidth();j++)
                                        {
                                                if(M<=A[i][j])M=A[i][j];
                                                if(A[i][j]<=m)m=A[i][j];
                                        }
                                        assert(Abounds[i]<=m);
                                        assert(Abounds[i]<=-M);
                                }
                        }
                  mvtypDouble getCoordinateOfInequality(int subconfigurationIndex, int columnIndex, int i, int j)const
                  {// Cannot overflows
                          //get (i,j)th coordinate of (subconfigurationIndex,columnIndex)th inequality
                          if(i==subconfigurationIndex)
                          {
                                  if(choices[i].first==j)return -A.UNCHECKEDACCESS(i,offsets[subconfigurationIndex]+columnIndex).extend()-denominator.extend();
                                  else if(choices[i].second==j)return A.UNCHECKEDACCESS(i,offsets[subconfigurationIndex]+columnIndex).extend();
                                  else if(j==columnIndex)return denominator.extend();
                                  else return mvtypDouble(0);
                          }
                          else
                                  if(choices[i].first==j)return -A.UNCHECKEDACCESS(i,offsets[subconfigurationIndex]+columnIndex).extend();
                                  else if(choices[i].second==j)return A.UNCHECKEDACCESS(i,offsets[subconfigurationIndex]+columnIndex).extend();
                                  else return mvtypDouble(0);
                  }
                  int sort2uniquely(int *v, int a, int b)const//a and b different
                  {// Cannot overflow
                          v[a>b]=a;
                          v[b>a]=b;
                          return 2;
                  }
                  int sort3uniquely(int *v, int a, int b, int c)const//a and b and c different
                  {// Cannot overflow
                          v[(a>b)+int(a>c)]=a;
                          v[(b>a)+int(b>c)]=b;
                          v[(c>a)+int(c>b)]=c;
                          return 3;
                  }
                  int sort4uniquely(int *v, int a, int b, int c, int d)const// a and b different and different from c and d, but c may equal d
                  {// Cannot overflow
                          if(c!=d)
                          {
                          v[(a>b)+int(a>c)+int(a>d)]=a;
                          v[(b>a)+int(b>c)+int(b>d)]=b;
                          v[(c>a)+int(c>b)+int(c>d)]=c;
                          v[(d>a)+int(d>b)+int(d>c)]=d;
                          return 4;
                          }
                          else return sort3uniquely(v,a,b,c);
                  }
                  bool compareReverseLexicographicInverted(int i1, int j1, int i2, int j2, mvtyp s1, mvtyp s2)const//s1 and s2 are always negative
                  {// cannot overflow
                          for(int i=0;i<k;i++)
                          {
                                          if(i1!=i && i2!=i)
                                          {
                                                  int temp=determinantSign1(A.UNCHECKEDACCESS(i,offsets[i1]+j1),s1,A.UNCHECKEDACCESS(i,offsets[i2]+j2),s2);
                                                  if(temp)
                                                  {
                                                          if(choices[i].first<choices[i].second)
                                                                  return temp<0;
                                                          else
                                                                  return temp>0;
                                                  }
                                          }
                                  int indices[4];
                                  int F=choices[i].first;
                                  int S=choices[i].second;
                                  int toCheck;
                                  if(i1==i)
                                          if(i2==i)
                                                  toCheck=sort4uniquely(indices,F,S,j1,j2);
                                          else
                                                  toCheck=sort3uniquely(indices,F,S,j1);
                                  else
                                          if(i2==i)
                                                  toCheck=sort3uniquely(indices,F,S,j2);
                                          else
                                                  toCheck=sort2uniquely(indices,F,S);

                                  for(int J=0;J<toCheck;J++)
                                  {
                                          int j=indices[J];
                                          int temp=determinantSign2(getCoordinateOfInequality(i1,j1,i,j),s1,getCoordinateOfInequality(i2,j2,i,j),s2);
                                          if(temp>0)
                                                  return true;
                                          else if(temp<0)
                                                  return false;
                                  }
                          }
                          return false;
                  }
                  mvtyp getVolume()
                  {// Cannot overflow
                          return denominator;
                  }
                void replaceFirstOrSecond(bool first, int subconfigurationIndex, int newIndex, Vector<mvtyp> const &target)__attribute__ ((always_inline))//updates the inequality table according to replacing first or second by newIndex in the subconfigurationIndex'th configuration
                {// Cannot overflow
                        int newIndex2=newIndex;for(int i=0;i<subconfigurationIndex;i++)newIndex2+=tuple[i].getWidth();
                        int oldIndex=first?choices[subconfigurationIndex].first:choices[subconfigurationIndex].second;
                        (first?choices[subconfigurationIndex].first:choices[subconfigurationIndex].second)=newIndex;

                        mvtyp nextDenominator=(first?A[subconfigurationIndex][newIndex2].extend()+denominator.extend():-A[subconfigurationIndex][newIndex2].extend()).castToSingle();
                        mvtyp t=nextDenominator;
                        mvtypDivisor denominatorDivisor(denominator);
                        for(int c=0;c<k+Flags::computeDotProductInMatrix;c++)tempA.UNCHECKEDACCESS(c)=A.UNCHECKEDACCESS(c,newIndex2);

                        (-Abounds[subconfigurationIndex].extend()).castToSingle();//This conversion will fail if the following fails
                        for(int u=0;u<m;u++)
                                svec[u]=first?-A.UNCHECKEDACCESS(subconfigurationIndex,u):A.UNCHECKEDACCESS(subconfigurationIndex,u);
                        mvtyp svecBound=Abounds[subconfigurationIndex];

                        if(first)
                                for(int j=0;j<tuple[subconfigurationIndex].getWidth();j++)
                                        svec[offsets[subconfigurationIndex]+j].subWithOverflowChecking(denominator);
                        svecBound.subWithOverflowChecking(denominator);


                        for(int a=0;a<k+Flags::computeDotProductInMatrix;a++)
                                if(first||a!=subconfigurationIndex)
                                Abounds.UNCHECKEDACCESS(a)=mvtyp::dotDivVector(&A.UNCHECKEDACCESS(a,0),&(svec[0]),t,tempA.UNCHECKEDACCESS(a),denominatorDivisor,m,Abounds[a],svecBound);
        //                        for(int u=0;u<m;u++)
        //                                *A.UNCHECKEDACCESSRESTRICT(a,u)=dotDiv(svec[u],tempA.UNCHECKEDACCESS(a),t,A.UNCHECKEDACCESS(a,u),denominatorDivisor);


                        {
                                for(int a=0;a<k+Flags::computeDotProductInMatrix;a++)
                                        {
                                                A[a][offsets[subconfigurationIndex]+oldIndex]=tempA[a].negatedWithOverflowChecking();
                                                Abounds[a]=min(Abounds[a],negabs(tempA[a]));
                                        }

                                if(!first)
                                {
                                        A[subconfigurationIndex][offsets[subconfigurationIndex]+oldIndex]=denominator.negatedWithOverflowChecking();
                                        Abounds[subconfigurationIndex].subWithOverflowChecking(denominator);
                                }
                        }
                        denominator=nextDenominator;

                        // We clear these unused entries of A to ensure that these columns are not chosen when comparing inequalities
                        for(int i=0;i<k+Flags::computeDotProductInMatrix;i++)
                        {
                                A.UNCHECKEDACCESS(i,offsets[subconfigurationIndex]+choices[subconfigurationIndex].first)=0;
                                A.UNCHECKEDACCESS(i,offsets[subconfigurationIndex]+choices[subconfigurationIndex].second)=0;
                        }
                }
                void replaceFirst(int subconfigurationIndex, int newIndex, Vector<mvtyp> const &target){replaceFirstOrSecond(true,subconfigurationIndex,newIndex,target);}
                void replaceSecond(int subconfigurationIndex, int newIndex, Vector<mvtyp> const &target){replaceFirstOrSecond(false,subconfigurationIndex,newIndex,target);}

                InequalityTable(std::vector<Matrix<mvtyp> > const &tuple_, int subconfigurationIndex_):
                        tempA(tuple_.size()+Flags::computeDotProductInMatrix),
                        tuple(tuple_),
                        choices(tuple_.size()),
                        subconfigurationIndex(subconfigurationIndex_),
                        offsets(tuple_.size())
                {// Cannot overflow
                        k=tuple.size();
                        m=0;
                        for(int i=0;i<tuple.size();i++)m+=tuple[i].getWidth();
                        svec.resize(m);
                        A=Matrix<mvtyp>(k+Flags::computeDotProductInMatrix,m);
                        {int offset=0;for(int i=0;i<tuple.size();i++){offsets[i]=offset;offset+=tuple[i].getWidth();}}
                        Abounds=Vector<mvtyp>(k+Flags::computeDotProductInMatrix);
                }
                void setChoicesInitially()
                {// Cannot overflow
                        //THIS WILL ONLY WORK FOR THE STARTING CONFIGURATION
                        //sets denominators,A and choices (these must have been initialized to the right sizes)
                        for(int i=0;i<k;i++)
                                choices[i]=std::pair<int, int> (i+0,i+1);
                        for(int i=0;i<m;i++)
                                for(int j=0;j<k;j++)
                                        A[j][i]=0;
                        //we follow the lemma in the article. Why does the proof of the lemma split into 3 cases and not just 2?
                        int a=0;
                        for(int i=0;i<k;i++)
                                for(int gamma=0;gamma<tuple[i].getWidth();gamma++,a++)
                                        if(gamma>i+1)
                                                for(int ii=i;ii<gamma;ii++)
                                                        A[ii][a]=-1;
                                        else
                                                for(int ii=gamma;ii<i;ii++)
                                                        A[ii][a]=1;
                        denominator=1;
                        for(int i=0;i<k;i++)Abounds[i]=-1;
        //                assignDotProducts(target);
                }
                void compareInequalities(InequalityComparisonResult &result, Vector<mvtyp> const &target, int onlyK=-1)
                {// Can only overflow if target entries are too big. Actually it seems that it is not this function that will overflow but dotVector.
                        bool empty=true;
                        int bestConfigurationIndex=-1;
                        int bestColumnIndex=-1;
                        mvtyp targetDotBest=0;

                        for(int i=0;i<k;i++)
                        {
                                // TO DO: exchange the line with the template parameter version:
//                    gfan::Matrix<mvtyp>::const_RowRef Ak=const_cast<const gfan::Matrix<mvtyp>&>(A)[k];
                    gfan::Matrix<CircuitTableInt32>::const_RowRef Ak=const_cast<const gfan::Matrix<mvtyp>&>(A)[k];
                    int offsetsi=offsets[i];
                    int tupleiwidth=tuple[i].getWidth();

                                if(onlyK!=-1)if(i!=onlyK)continue;
                                for(int j=0;j<tupleiwidth;j++)
                                        if(Flags::computeDotProductInMatrix || isLegalIndex(i,j))//unused inequalities will have value 0. Therefore isLegalIndex(i,j) is not required if values are stored.
                                        {
                                                mvtyp ineqDotTarget=Flags::computeDotProductInMatrix?Ak.UNCHECKEDACCESS(offsetsi+j):dotVector(i,j,target,onlyK);
                                                if(ineqDotTarget.isNegative())
                                                {
                                                        if(!isReverseLexInvertedLessThanZero(i,j))
                                                        {
                                                                if(empty||compareReverseLexicographicInverted(bestConfigurationIndex,bestColumnIndex,i,j,ineqDotTarget,targetDotBest))
                                                                {
                                                                        targetDotBest=ineqDotTarget;
                                                                        empty=false;
                                                                        bestConfigurationIndex=i;
                                                                        bestColumnIndex=j;
                                                                }
                                                        }
                //                                        assert(!ineq.isZero());
                                                }
                                        }
                        }
                        result.empty=empty;
                        result.configurationIndex=bestConfigurationIndex;
                        result.columnIndex=bestColumnIndex;
        //                assert(denominator>0);
                }
                void setChoicesFromEarlierHomotopy(InequalityTable const &parent, mvtyp degreeScaling, Vector<mvtyp> const &target)
                {        // Overflows may happen, but only if the tuple entries are too big.
                        // Notice that the code below has overflow checks everywhere, except in the loop marked with a comment.

                        //sets denominators,A and choices (these must have been initialized to the right sizes
                        //columns have been added to configuration this->subconfigurationIndex
                        //for that reason we need to introduce new circuits. Old circuits are preserved.
                        //chioices are "relative" so no update is needed.

                        choices=parent.choices;
                        int numberToDrop=(subconfigurationIndex!=0) ? k+1 : 0;

                        choices[subconfigurationIndex-1].first-=numberToDrop;
                        choices[subconfigurationIndex-1].second-=numberToDrop;

                        denominator=parent.denominator;
                        int offsetOld=0;
                        int offsetNew=0;
                        for(int i=0;i<k;i++)
                        {
                                int localNumberToDrop=0;
                                if(i==subconfigurationIndex-1)
                                        localNumberToDrop=numberToDrop;
                                for(int a=0;a<A.getHeight()-Flags::computeDotProductInMatrix;a++)
                                        if(a==subconfigurationIndex)
                                                for(int j=0;j<parent.tuple[i].getWidth()-localNumberToDrop;j++)
                                                        A.UNCHECKEDACCESS(a,offsetNew+j)=parent.A.UNCHECKEDACCESS(a,offsetOld+j+localNumberToDrop);
                                        else
                                        {
                                                // We can get an estimated 3 percent speed up by using known bounds to check if any of the multiplications below can overflow.
                                                // Moreover, these multiplications can be moved outside the outer loop, to give better vectorisation.
                                                for(int j=0;j<parent.tuple[i].getWidth()-localNumberToDrop;j++)
                                                        A.UNCHECKEDACCESS(a,offsetNew+j)=extendedMultiplication(degreeScaling,parent.A.UNCHECKEDACCESS(a,offsetOld+j+localNumberToDrop)).castToSingle();
                                        }
                                if(i==subconfigurationIndex)
                                {
                                        for(int j=parent.tuple[i].getWidth();j<tuple[i].getWidth();j++)
                                        {
                                                for(int a=0;a<A.getHeight()-Flags::computeDotProductInMatrix;a++)
                                                        A.UNCHECKEDACCESS(a,offsetNew+j)=0;
                                                {
                                                        int b=choices[subconfigurationIndex].second-1;
                                                        if(b>=0)
                                                                A.UNCHECKEDACCESS(subconfigurationIndex,offsetNew+j).msubWithOverflowChecking(tuple[i].UNCHECKEDACCESS(b,j),denominator);
                                                }
                                                for(int a=0;a<k+Flags::computeDotProductInMatrix;a++)
                                                {   // If tuple entries are not bounded, overflow can happen in the following loop
                                                        mvtypDouble tempDouble=A.UNCHECKEDACCESS(a,offsetNew+j).extend();
                                                        for(int b=0;b<k;b++)
                                                                if(choices[subconfigurationIndex].second!=b+1 &&choices[subconfigurationIndex].first!=b+1)
                                                                        tempDouble+=extendedMultiplication(tuple[i].UNCHECKEDACCESS(b,j),A.UNCHECKEDACCESS(a,offsetNew+b+1));
                                                        A.UNCHECKEDACCESS(a,offsetNew+j)=tempDouble.castToSingle();
                                                }
                                                mvtypDouble left=degreeScaling.extend();
                                                for(int b=0;b<k;b++)
                                                        left-=mvtyp(tuple[i].UNCHECKEDACCESS(b,j)).extend();

                                                mvtyp leftS=left.castToSingle();

                                                if(choices[subconfigurationIndex].second==0)
                                                        A.UNCHECKEDACCESS(subconfigurationIndex,offsetNew+j).msubWithOverflowChecking(leftS,denominator);
                                                else if(choices[subconfigurationIndex].first!=0)
                                                        for(int a=0;a<k+Flags::computeDotProductInMatrix;a++)
                                                                A.UNCHECKEDACCESS(a,offsetNew+j).maddWithOverflowChecking(leftS,mvtyp(A.UNCHECKEDACCESS(a,offsetNew)));
                                        }
                                        for(int j=0;j<parent.tuple[i].getWidth();j++)// <-- this loop has side effects on addExpressionOfCeb() above. Therefore it is placed after the loop above.
                                                for(int a=0;a<A.getHeight()-Flags::computeDotProductInMatrix;a++)
                                                        A.UNCHECKEDACCESS(a,offsetNew+j)=extendedMultiplication(A.UNCHECKEDACCESS(a,offsetNew+j),degreeScaling).castToSingle();
                                }
                                offsetOld+=parent.tuple[i].getWidth();
                                offsetNew+=tuple[i].getWidth();
                        }
                        denominator=extendedMultiplication(denominator,degreeScaling).castToSingle();
                        if(Flags::computeDotProductInMatrix)assignDotProducts(target,subconfigurationIndex);
                        computeABounds();
                        for(int a=0;a<k;a++)
                        for(int i=0;i<k+Flags::computeDotProductInMatrix;i++)
                        {
                                A[i][offsets[a]+choices[a].first]=0;
                                A[i][offsets[a]+choices[a].second]=0;
                        }
                }
        };
                struct StackItem{
                        int columnIndex;
                        int configurationIndex;
                        bool b;
                        int choice;
                        bool useFirstChanged,useSecondChanged;
                        StackItem(int columnIndex_, int configurationIndex_, bool b_, int choice_, bool useFirstChanged_, bool useSecondChanged_):
                                columnIndex(columnIndex_),
                                configurationIndex(configurationIndex_),
                                b(b_),
                                choice(choice_),
                                useFirstChanged(useFirstChanged_),
                                useSecondChanged(useSecondChanged_)
                        {
                        }
                };
        public:
                std::vector<std::pair<int,int> > choices;
                Vector<mvtyp> target;
                bool useFirstChanged;
                bool useSecondChanged;
                std::vector<StackItem> stack;
                int eliminatedK;
                int eliminatedKOffset;
                std::vector<Matrix<mvtyp> > tuple;
                std::vector<int> offsets;
                int m;
                InequalityComparisonResult result;
                InequalityTable inequalityTable;
                void constructInequalityTableFromParent(InequalityTable const &parentTable, mvtyp degreeScaling)
                {
                        inequalityTable.setChoicesFromEarlierHomotopy(parentTable, degreeScaling, target);
                }
                void constructInequalityTableInitially(mvtyp degreeScaling)
                {
                        std::vector<Matrix<mvtyp> > tempTuple;for(int i=0;i<tuple.size();i++)tempTuple.push_back(simplex<mvtyp>(tuple.size(),1));
                        InequalityTable tempTable(tempTuple,-1);
                        tempTable.setChoicesInitially();
                        constructInequalityTableFromParent(tempTable,degreeScaling);
                }
                SingleTropicalHomotopyTraverser(std::vector<Matrix<mvtyp> > const &tuple_, int m_, std::vector<std::pair<int,int> > const &choices_, Vector<mvtyp> const &target_, int eliminatedK_):
                        choices(choices_),
                        target(target_),
                        eliminatedK(eliminatedK_),
                        tuple(tuple_),
                        m(m_),
                        inequalityTable(tuple,eliminatedK),
                        offsets(tuple_.size())
                {
                        eliminatedKOffset=0;for(int i=0;i<eliminatedK;i++)eliminatedKOffset+=tuple_[i].getWidth();
                        {int offset=0;for(int i=0;i<tuple.size();i++){offsets[i]=offset;offset+=tuple[i].getWidth();}}

                        /* We also need to check that the input is within the required bounds
                         * This is important to not have overflows in:
                         *   dotVector
                         *   setChoicesFromEarlierHomotopy
                         * The number of summands is bounded by 3k+4*/

                        bool isOK=mvtyp::isSuitableSummationNumber(3*tuple.size()+4);
                        for(int i=0;i<target.size();i++)isOK&=mvtyp(target[i]).fitsInHalf();
                        for(int i=0;i<tuple.size();i++)
                                for(int j=0;j<tuple[i].getHeight();j++)
                                        for(int k=0;k<tuple[i].getWidth();k++)
                                                isOK&=mvtyp(tuple[i][j][k]).fitsInHalf();
                        if(!isOK)throw MVMachineIntegerOverflow;
                }
                virtual void process()
                {
                }
                bool findOutgoingAndProcess(bool doProcess)//sets up useFirstChanged and useSecondChanged and processes if process is true
                {//returns true if we are at a leaf
//                        inequalityTable.checkABounds();
                        useFirstChanged=false;
                        useSecondChanged=false;
                        int onlyK=-1;
#if 1
                        if(eliminatedK!=-1)
                                if(target[choices[eliminatedK].first+eliminatedKOffset]==target[choices[eliminatedK].second+eliminatedKOffset])
                                        onlyK=eliminatedK;
#endif

                        inequalityTable.compareInequalities(result,target,onlyK);

                        if(result.empty)
                        {
                                if(doProcess)process();
                                return true;
                        }

                         // reverse search rule:

                        mvtypDouble bestAtFirst=inequalityTable.getCoordinateOfInequality(result.configurationIndex,result.columnIndex,result.configurationIndex,choices[result.configurationIndex].first);
                        mvtypDouble bestAtSecond=inequalityTable.getCoordinateOfInequality(result.configurationIndex,result.columnIndex,result.configurationIndex,choices[result.configurationIndex].second);
                        if(bestAtFirst.isNegative())
                        {
                                if(bestAtSecond.isNegative())
                                {
                                        useFirstChanged=true;
                                        useSecondChanged=true;
                                }
                                else
                                {
                                        if(bestAtSecond.isZero())
                                        useFirstChanged=true;
                                        else
                                        if(choices[result.configurationIndex].second<result.columnIndex)
                                        useFirstChanged=true;
                                }
                        }
                        else
                        {
                                if(bestAtSecond.isNegative())
                                {
                                        if(bestAtFirst.isZero())
                                        useSecondChanged=true;
                                        else
                                        if(choices[result.configurationIndex].first<result.columnIndex)
                                        useSecondChanged=true;
                                }
                                else
                                {
                                        assert(0);
                                }
                        }
                        return false;
                }
                void goToFirstChild()
                {
//                        debug<<"First:  configuration index:"<<data.configurationIndex<<" column index:"<<data.columnIndex<<"\n";
                        assert(useFirstChanged);
                        {
                                stack.push_back(StackItem(
                                                result.columnIndex,
                                                result.configurationIndex,
                                                false,
                                                choices[result.configurationIndex].first,
                                                useFirstChanged,
                                                useSecondChanged));
                                choices[result.configurationIndex].first=result.columnIndex;
                                inequalityTable.replaceFirst(result.configurationIndex,result.columnIndex,target);
                        }
                }
                void goToSecondChild()
                {
//                        debug<<"Second: configuration index:"<<data.configurationIndex<<" column index:"<<data.columnIndex<<"\n";
                        assert(useSecondChanged);
                        {
                                stack.emplace_back(StackItem(
                                                result.columnIndex,
                                                result.configurationIndex,
                                                true,
                                                choices[result.configurationIndex].second,
                                                useFirstChanged,
                                                useSecondChanged));
                                choices[result.configurationIndex].second=result.columnIndex;
                                inequalityTable.replaceSecond(result.configurationIndex,result.columnIndex,target);
                        }
                }
                int numberOfChildren()
                {
                        return int(useFirstChanged)+int(useSecondChanged);
                }
                void goToNthChild(int n)
                {
                        if(n==0)
                                if(useFirstChanged)
                                        goToFirstChild();
                                else
                                        goToSecondChild();
                        else
                                goToSecondChild();
                }
                void goBack()
                {
                        StackItem &B=stack.back();
                        result.columnIndex=B.columnIndex;
                        result.configurationIndex=B.configurationIndex;
                        if(B.b)
                        {
                                choices[result.configurationIndex].second=B.choice;
                                inequalityTable.replaceSecond(result.configurationIndex,B.choice,target);
                        }
                        else
                        {
                                choices[result.configurationIndex].first=B.choice;
                                inequalityTable.replaceFirst(result.configurationIndex,B.choice,target);
                        }
                        useFirstChanged=B.useFirstChanged;
                        useSecondChanged=B.useSecondChanged;
                        stack.pop_back();
                }
                bool atRoot()
                {
                        return stack.empty();
                }
        };


/*
 *  This class concatenates several homotopies to a single tree.
 */
template<class mvtyp, class mvtypDouble, class mvtypDivisor>
        class TropicalRegenerationTraverser{
                // The following class is an attempt to separate homotopy data from the traversal logic.
                class Data{
                        static mvtyp degree(Matrix<mvtyp> const &m)//assumes entries of m are non-negative
                        {
                                  mvtyp ret=0;
                                  for(int i=0;i<m.getWidth();i++)
                                  {
                                          mvtyp s(0);
                                          for(int j=0;j<m.getHeight();j++)
                                                  s.addWithOverflowChecking(m[j][i]);
                                          ret=max(s,ret);
                                  }
                                  return ret;
                        }
                  public:
                          std::vector<Vector<mvtyp> > targets;
                          std::vector<Matrix<mvtyp> > tuple;
                          std::vector<std::vector<Matrix<mvtyp> > > tuples;
                          Vector<mvtyp> degrees;
                          bool isFiniteIndex(int level, int index)
                          {
                                  return index>=tuple[0].getHeight()+1;
                          }

                          std::vector<Matrix<mvtyp> > produceIthTuple(int i)
                                {
                                  int n=tuple[0].getHeight();
                                  std::vector<Matrix<mvtyp> > ret;
                                  for(int j=0;j<tuple.size();j++)
                                  {
                                          if(j<i)ret.push_back(tuple[j]);
                                          if(j==i)ret.push_back(combineLeftRight(simplex<mvtyp>(n,degree(tuple[j])),tuple[j]));
                                          if(j>i)ret.push_back(simplex<mvtyp>(n,1));
                                  }
                                  return ret;
                                }
                          Data(std::vector<Matrix<mvtyp> > const &tuple_):tuple(tuple_)
                          {
                                  int n=tuple[0].getHeight();

                                  {//adjusting to positive orthant
                                          for(int i=0;i<tuple.size();i++)
                                                  for(int j=0;j<tuple[i].getHeight();j++)
                                                  {
                                                          mvtyp m;
                                                          if(tuple[i].getWidth()==0)
                                                                  m=0;
                                                          else
                                                                  m=tuple[i][j][0];
                                                          for(int k=0;k<tuple[i].getWidth();k++)m=min(m,tuple[i][j][k]);
                                                          for(int k=0;k<tuple[i].getWidth();k++)tuple[i][j][k].subWithOverflowChecking(m);
                                                  }
                                  }

                                  for(int i=0;i<tuple.size();i++)
                                          degrees.push_back(degree(tuple[i]));

                                  for(int i=0;i<tuple.size();i++)
                                          tuples.push_back(produceIthTuple(i));

                                  for(int i=0;i<tuple.size();i++)
                                  {
                                          Vector<mvtyp> targ;
                                          for(int j=0;j<tuple.size();j++)
                                          {
                                                  if(j==i)
                                                          targ=concatenation(targ,concatenation(Vector<mvtyp>::allOnes(n+1),Vector<mvtyp>(tuple[i].getWidth())));
                                                  else
                                                          targ=concatenation(targ,Vector<mvtyp>(tuples[i][j].getWidth()));
                                          }
                                          targets.push_back(targ);
                                  }
                          };

                          std::vector<std::pair<int,int> > firstIntersection()
                          {
                                  std::vector<std::pair<int,int> > ret;
                                  for(int i=0;i<tuple.size();i++)
                                          ret.push_back(std::pair<int,int>(i+0,i+1));
                                  return ret;
                          }

                          void castToNextLevel(std::vector<std::pair<int,int> > const &choices, int i, int S, std::vector<std::pair<int,int> > &ret)
                          {
                                  assert(ret.size()==choices.size());
                                  for(int j=0;j<choices.size();j++)
                                          ret[j]=choices[j];

                                  assert(ret[i].first>=S);
                                  assert(ret[i].second>=S);
                                  ret[i].first-=S;
                                  ret[i].second-=S;
                          }
                  };
                static int cayleyConfigurationWidth(std::vector<Matrix<mvtyp> > const &tuple)
                {
                          int m2=0;
                          for(int i=0;i<tuple.size();i++)
                                  m2+=tuple[i].getWidth();
                          return m2;
                }
        public:
                int depth;
                int counter;
                typedef SingleTropicalHomotopyTraverser<mvtyp,mvtypDouble,mvtypDivisor> MySingleTropicalHomotopyTraverser;
                std::vector<MySingleTropicalHomotopyTraverser> traversers;
                Data fullData;
                int level;
                bool deadEnd;
                bool isLevelLeaf;
                bool isSolutionVertex;
                std::vector<bool> isLevelLeafStack;
                TropicalRegenerationTraverser(std::vector<Matrix<mvtyp> > const &tuple_):
                        fullData(tuple_),counter(0),depth(0)
                {
                        assert(tuple_.size());
                        for(int i=0;i<tuple_.size();i++)
                                traversers.push_back(MySingleTropicalHomotopyTraverser(fullData.tuples[i],cayleyConfigurationWidth(fullData.tuples[i]),fullData.firstIntersection(),fullData.targets[i],i));
                        traversers[0].constructInequalityTableInitially(fullData.degrees[0]);
                        level=0;
                }
                virtual void process()
                {
                }
                bool findOutgoingAndProcess(bool doProcess)
                {
                        isSolutionVertex=false;
                        deadEnd=false;
                        isLevelLeaf=traversers[level].findOutgoingAndProcess(false);
                        if(isLevelLeaf)
                        {//leaf
                                bool isFinite=fullData.isFiniteIndex(level,traversers[level].choices[level].first)&&fullData.isFiniteIndex(level,traversers[level].choices[level].second);
                                deadEnd=!isFinite;
                                if(isFinite && (level==fullData.tuple.size()-1))
                                {
                                        isSolutionVertex=true;
                                        if(doProcess){process();}
                                        return true;
                                }
                        }
                        return false;
                }
                int numberOfChildren()
                {
                        if(isLevelLeaf&&(level==fullData.tuple.size()-1))return 0;
                        if(!isLevelLeaf)
                                return traversers[level].numberOfChildren();
                        else
                                return 1-deadEnd;
                }
                void goToNthChild(int n)
                {
                        depth++;
                        isLevelLeafStack.push_back(isLevelLeaf);
                        if(!isLevelLeaf)
                                traversers[level].goToNthChild(n);
                        else
                        {
                                fullData.castToNextLevel(traversers[level].choices,level,fullData.tuples[level][level].getWidth()-fullData.tuples[level+1][level].getWidth(),traversers[level+1].choices);
                                traversers[level+1].constructInequalityTableFromParent(traversers[level].inequalityTable,fullData.degrees[level+1]);
                                level++;
                        }
                }
                void print()
                {
                }
                void goBack()
                {
                        depth--;
                        counter++;
                        deadEnd=false;
                        if(traversers[level].atRoot())
                                level--;
                        else
                                traversers[level].goBack();
                        isLevelLeaf=isLevelLeafStack.back();
                        isLevelLeafStack.pop_back();
                }
        };

        /*
         * This class glues Bjarne Knudsen's parallel traverser to our MultiLevelIntersectionTraverser.
         * This class should eventually be written so that it works on any homotopy.
         *
         * Since we are not sure about how exceptions would be handled by the parallel traverser,
         * we write ugly aborting code, which may actually work.
         */
        template<class mvtyp, class mvtypDouble, class mvtypDivisor>
        class SpecializedRTraverser: public Traverser
        {
        public:
                typedef TropicalRegenerationTraverser<mvtyp,mvtypDouble,mvtypDivisor> MyTropicalRegenerationTraverser;
                MyTropicalRegenerationTraverser T;
                mvtypDouble mixedVolume;
                int numberOfExpensiveSteps;
                SpecializedRTraverser(std::vector<Matrix<mvtyp> > const &tuple_):
                         mixedVolume(),
                         numberOfExpensiveSteps(0),
                         T(tuple_) //Constructor my throw if input is too big.
                {
                        numberOfExpensiveSteps++;
                        T.findOutgoingAndProcess(false);
                }
                int  getEdgeCountNext( void )
                {
                        if(!aborting)
                        {
                                try{
                                        return T.numberOfChildren();
                                }
                                catch(...){abort();}
                        }
                        return 0;
                }

                int  moveToNext( int   index,
                                   bool  collect_info )
                {
                        if(!aborting)
                        {
                                try{
                                        T.goToNthChild(index);
                                        numberOfExpensiveSteps++;
                                        T.findOutgoingAndProcess(false);
                                }
                                catch(...){abort();}
                        }
                        return 0;
                }

          void  moveToPrev( int  index )
          {
                  if(!aborting)
                  {
                          try{
                                  T.goBack(); //index ignored
                          }
                          catch(...){abort();}
                  }
          }

          void  collectInfo( void )
          {
                  if(!aborting)
                  {
                          try{
                                  if(T.isSolutionVertex)
                                          mixedVolume.addWithOverflowCheck(T.traversers[T.level].inequalityTable.getVolume().extend());
                          }
                          catch(...){abort();}
                  }
          }

          void  printState( void )
          {
                  T.print();
          }
        };
}

#endif /* GFANLIB_TROPICALHOMOTOPY_H_ */
