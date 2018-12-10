/// testdata.cc

/// Implements the data generation routines declared in testdata.h.

#ifndef TESTDATA_CC
#define TESTDATA_CC



#include "testdata.h"




int random_matrix(const short& rows, const short& columns,
                  const Integer& lower_bound, const Integer& upper_bound,
                  ofstream& MATRIX)
{
  /// check arguments

  if(rows<=0)
  {
    cerr<<"ERROR: int random_matrix(const short&, const short&, \n"
      "                         const Integer&, const Integer&, ofstream&):\n"
      "first argument out of range: number of matrix rows must be positive"
        <<endl;
    return 0;
  }

  if(columns<=0)
  {
    cerr<<"ERROR: int random_matrix(const short&, const short&, \n"
      "                         const Integer&, const Integer&, ofstream&):\n"
      "second argument out of range: number of matrix columns must be positive"
        <<endl;
    return 0;
  }

  if(upper_bound<0)
  {
    cerr<<"ERROR: int random_matrix(const short&, const short&, \n"
      "                         const Integer&, const Integer&, ofstream&):\n"
      "fourth argument (upper bound for random cost vector entries) must be\n"
      "nonnegative\n"<<endl;
    return 0;
  }

  if(upper_bound<lower_bound)
  {
    cerr<<"ERROR: int random_matrix(const short&, const short&, \n"
      "                         const Integer&, const Integer&, ofstream&):\n"
      "third argument (lower bound for random entries) must be less\n"
      "or equal the fourth argument (upper bound)"<<endl;
    return 0;
  }

  /// create test file

  MATRIX<<"MATRIX"<<endl<<endl;

  MATRIX<<"columns:"<<endl;
  MATRIX<<columns<<endl<<endl;

  /// random cost vector
  MATRIX<<"cost vector:"<<endl;
  for(short j=0;j<columns;j++)
    MATRIX<<setw(4)<<rand()%(upper_bound+1);
  /// random entries between 0 and upper_bound
  MATRIX<<endl;

  MATRIX<<"rows:"<<endl;
  MATRIX<<rows<<endl<<endl;

  /// random matrix
  MATRIX<<"matrix:"<<endl;
  for(short i=0;i<rows;i++)
  {
    for(short j=0;j<columns;j++)
      MATRIX<<setw(4)<<rand()%(upper_bound-lower_bound+1)+lower_bound;
    /// random entries between lower_bound and upper_bound
    MATRIX<<endl;
  }
  MATRIX<<endl;

  MATRIX<<"positive row space vector:"<<endl;

  return 1;

}




int transportation_problem(const short& sources, const short& targets,
                           const Integer& upper_bound, ofstream& MATRIX)
{
  /// check arguments

  if(sources<=0)
  {
    cerr<<"ERROR: int transportation_problem(const short&, const short&, \n"
      "                         const Integer&, ofstream&):\n"
      "first argument out of range: number of sources must be positive"
        <<endl;
    return 0;
  }

  if(targets<=0)
  {
    cerr<<"ERROR: int transportation_problem(const short&, const short&, \n"
      "                         const Integer&, ofstream&):\n"
      "second argument out of range: number of targets must be positive"
        <<endl;
    return 0;
  }

  if(upper_bound<0)
  {
    cerr<<"ERROR: int transportation_problem(const short&, const short&, \n"
      "                         const Integer&, const Integer&, ofstream&):\n"
      "third argument (upper bound for random cost vector entries) must be\n"
      "nonnegative\n"<<endl;
    return 0;
  }

  /// create test file

  MATRIX<<"MATRIX"<<endl<<endl;

  MATRIX<<"columns:"<<endl;
  MATRIX<<sources*targets<<endl<<endl;

  /// random cost vector
  MATRIX<<"cost vector:"<<endl;
  for(short j=0;j<sources*targets;j++)
    MATRIX<<setw(4)<<rand()%(upper_bound+1);
  /// random entries between 0 and upper_bound
  MATRIX<<endl;

  MATRIX<<"rows:"<<endl;
  MATRIX<<sources+targets<<endl<<endl;

  /// constraint matrix in the usual formulation of the transportation problem
  /// as an IP problem
  MATRIX<<"matrix:"<<endl;

  for(int i=0;i<targets;i++)
    /// generate matrix
  {
    for(int k=0;k<targets;k++)
      for(int j=0;j<sources;j++)
        MATRIX<<setw(2)<<(int)(i==k);
    MATRIX<<endl;
  }

  for(int j=0;j<sources;j++)
  {
    for(int i=0;i<targets;i++)
      for(int k=0;k<sources;k++)
        MATRIX<<setw(2)<<(int)(j==k);
    MATRIX<<endl;
  }

  MATRIX<<endl;

  MATRIX<<"positive row space vector:"<<endl;
  for(short j=0;j<sources*targets;j++)
    MATRIX<<setw(2)<<1<<endl;
  MATRIX<<endl;

  return 1;
}



int random_problems(const short& vector_dimension,
                    const long& number_of_instances,
                    const Integer& lower_bound, const Integer& upper_bound,
                    ofstream& PROBLEM)
{
  /// check arguments

  if(vector_dimension<=0)
  {
    cerr<<"ERROR: int random_problems(const short&, const long&, \n"
      "                         const Integer&, const Integer&, ofstream&):\n"
      "first argument out of range: vector dimension must be positive"
        <<endl;
    return 0;
  }

  if(number_of_instances<0)
  {
    cerr<<"ERROR: int random_problems(const short&, const long&, \n"
      "                         const Integer&, const Integer&, ofstream&):\n"
      "second argument out of range: number of instances must be nonnegative"
        <<endl;
    return 0;
  }

  if(upper_bound<lower_bound)
  {
    cerr<<"ERROR: int random_problems(const short&, const long&, \n"
      "                         const Integer&, const Integer&, ofstream&):\n"
      "third argument (lower bound for random entries) must be less\n"
      "or equal the fourth argument (upper bound)"<<endl;
    return 0;
  }

  /// create random problems

  PROBLEM<<"PROBLEM"<<endl<<endl;

  PROBLEM<<"vector size:"<<endl;
  PROBLEM<<vector_dimension<<endl<<endl;

  PROBLEM<<"number of instances:"<<endl;
  PROBLEM<<number_of_instances<<endl<<endl;

  PROBLEM<<"right hand or initial solution vectors:"<<endl;
  for(short i=0;i<number_of_instances;i++)
  {
    for(short j=0;j<vector_dimension;j++)
      PROBLEM<<setw(4)<<rand()%(upper_bound+1);
    /// random entries between 0 and upper_bound
    PROBLEM<<endl;
  }
  PROBLEM<<endl;

  return 1;
}
#endif  /// TESTDATA_CC
