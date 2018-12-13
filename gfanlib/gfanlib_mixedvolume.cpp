/*
 * gfanlib_mixedvolume.cpp
 *
 *  Created on: Apr 10, 2016
 *      Author: anders
 */
#include <stddef.h>
#include "gfanlib_mixedvolume.h"

#include "gfanlib_circuittableint.h"
#include "gfanlib_tropicalhomotopy.h"

#include "gfanlib_z.h"

using namespace std;

typedef gfan::CircuitTableInt32 mvtyp;

namespace gfan{

static Integer stringToInteger(char const *s)
{
        Integer ret;
        while(*s)ret=Integer(10)*ret+((*(s++))-'0');
        return ret;
}


class MVExceptionRethrow: public exception
{
  virtual const char* what() const throw()
  {
    return "Exception: Rethrown after detecting aborting flag in tropical homotopy.";
  }
}MVExceptionRethrow;


class MVAmbientDimensionMismatch: public std::exception
{
  virtual const char* what() const throw()
  {
        return "Exception: The number of polytopes passed to the mixed volume computation does not match the ambient dimension.";
  }
}MVAmbientDimensionMismatch;


static std::vector<Matrix<mvtyp> > convertTuple(std::vector<IntMatrix> const &tuple)
{
        vector<Matrix<mvtyp> > ret;
        ret.reserve(tuple.size());
        for(int i=0;i<tuple.size();i++)
        {
                Matrix<mvtyp> a(tuple[i].getHeight(),tuple[i].getWidth());
                for(int j=0;j<a.getHeight();j++)
                        for(int k=0;k<a.getWidth();k++)
                                a[j][k]=tuple[i][j][k];
                ret.push_back(a);
        }
        return ret;
}

//        mvtyp::Double
        Integer mixedVolume(vector<IntMatrix> const &tuple, int nthreads, int steps)
        {
                //Zero-dimensional volumes are obtained by taking determinants of 0x0 matrices. Such determinants are 1. Hence the volume polynomial is constant 1. The coefficient of the product of the variables is 1.
                //Alternative explanation. A generic polynomial system with zero variables has exactly 1 solution.
                if(tuple.size()==0){return Integer(1);}

                // For now, let's make in an error to pass the wrong number of polytopes. Of course we could define the
                // mixed volume to be 0 in that case, but the code calling will almost certainly be wrong. It is safer to throw.
                // for(const auto &x:tuple) // Range based for is not supported until gcc 4.6/
                for(auto x=tuple.begin();x!=tuple.end();x++)
                        if(x->getHeight()!=tuple.size())
                                {throw MVAmbientDimensionMismatch;}

                std::vector<Matrix<mvtyp> > tuple2=convertTuple(tuple);
                //                assert(tuple2.size());
                typedef SpecializedRTraverser<mvtyp,mvtyp::Double,mvtyp::Divisor> MySpecializedRTraverser;
                vector<MySpecializedRTraverser> T1;
                int N=nthreads;
                if(N==0)N=1;// Even if we do not parallelize, we still need one traverser.
                T1.reserve(N);
                vector<Traverser*> I;
                for(int i=0;i<N;i++)T1.emplace_back(tuple2);
                for(int i=0;i<N;i++)I.push_back(&(T1[i]));

//                debug<<"Initialized.\n";

                if(nthreads)
                        traverse_threaded(&(I[0]),N,steps);
                else
                        traverse_simple(I[0]);

                bool aborted=false;
                for(int i=0;i<N;i++)aborted|=I[i]->aborting;

                if(aborted)
                {
                        throw MVExceptionRethrow;
                }

                mvtyp::Double total;
                int totalSteps=0;
                for(int i=0;i<N;i++)
                {
//                        debug<<"#"<<((MySpecializedRTraverser*)(I[i]))->mixedVolume.toString()<<"Steps:"<<((MySpecializedRTraverser*)(I[i]))->numberOfExpensiveSteps<<"\n";
                        //debug<<((SpecializedMTraverser*)(I[i]))->T.counter;
                        total.addWithOverflowCheck(((MySpecializedRTraverser*)(I[i]))->mixedVolume);
                        totalSteps+=((MySpecializedRTraverser*)(I[i]))->numberOfExpensiveSteps;
                }
//                debug<<"Total:"<<(int)total.v<<"\n";
//                debug<<"Totalsteps:"<<totalSteps<<"\n";


                return stringToInteger(total.toString().c_str());
        }


        namespace MixedVolumeExamples{
                vector<IntMatrix> cyclic(int n)
                {
                        vector<IntMatrix> ret;
                        for(int i=1;i<n;i++)
                        {
                                IntMatrix m(n,n);
                                for(int y=0;y<n;y++)
                                        for(int x=0;x<n;x++)
                                                m[y][x]=((x-y+n)%n)<i;
                                ret.push_back(m);
                        }

                        IntMatrix last(n,2);
                        for(int y=0;y<n;y++)last[y][0]=1;
                        ret.push_back(last);

                        return ret;
                }
                vector<IntMatrix> noon(int n)
                {
                        vector<IntMatrix> ret;
                        for(int i=0;i<n;i++)
                        {
                                IntMatrix m(n,n+1);
                                for(int y=0;y<n-1;y++)
                                        m[y+(i<=y)][y]=2;
                                for(int x=0;x<n;x++)
                                        m[i][x]=1;
                                ret.push_back(m);
                        }
                        return ret;
                }
                vector<IntMatrix> chandra(int n)
                {
                        vector<IntMatrix> ret;
                        for(int i=0;i<n;i++)
                        {
                                IntMatrix m(n,n+1);
                                for(int y=0;y<n-1;y++)
                                        m[y][y+1]=1;
                                for(int x=0;x<n;x++)
                                        m[i][x]+=1;
                                ret.push_back(m);
                        }
                        return ret;
                }
                vector<IntMatrix> katsura(int n)
                {
                        n++;
                        vector<IntMatrix> ret;
                        for(int i=0;i<n-1;i++)
                        {
                                IntMatrix m(n,n+1-((i+1)/2));
                                for(int y=0;y<n-((i+1)/2);y++)
                                {
                                        m[n-1-y][y]=1;
                                        m[(n-1-y-i)>0 ? (n-1-y-i) : -(n-1-y-i)][y]+=1;
                                }
                                m[i][m.getWidth()-1]=1;
                                ret.push_back(m);
                        }
                        ret.push_back(combineLeftRight(IntMatrix::identity(n),IntMatrix(n,1)));
                        return ret;
                }
                vector<IntMatrix> gaukwa(int n)
                {
                        vector<IntMatrix> ret;
                        for(int i=0;i<2*n;i++)
                                ret.push_back(combineLeftRight(combineOnTop(IntMatrix::identity(n),i*IntMatrix::identity(n)),IntMatrix(2*n,1)));
                        return ret;
                }
                vector<IntMatrix> eco(int n)
                {
                        vector<IntMatrix> ret;
                        for(int i=0;i<n-1;i++)
                        {
                                IntMatrix m(n,n-i);
                                for(int y=0;y<n-(i+1);y++)
                                {
                                        m[y+i][y]=1;
                                        m[n-1][y]=1;
                                        if(y)m[y-1][y]=1;
                                }
                                ret.push_back(m);
                        }
                        ret.push_back(combineLeftRight(combineOnTop(IntMatrix::identity(n-1),IntMatrix(1,n-1)),IntMatrix(n,1)));
                        return ret;
                }
        }

}
