// gentest.cc

// a program that generates some test data


#ifndef GEN_TEST_CC
#define GEN_TEST_CC

#include "testdata.h"

int main(void)
{
  ofstream output1("IP/testmatrix1");
  random_matrix(3,4,0,5,output1);
  ofstream output2("IP/testmatrix2");
  random_matrix(4,6,0,5,output2);
  ofstream output3("IP/testmatrix3");
  random_matrix(3,4,-5,5,output3);
  ofstream output4("IP/testmatrix4");
  random_matrix(5,8,0,1,output4);
  ofstream output5("IP/testmatrix5");
  transportation_problem(3,4,5,output5);
  ofstream output6("IP/testmatrix6");
  transportation_problem(3,5,5,output6);

  ofstream output1a("IP/testproblem1a");
  random_problems(3,5,-5,5,output1a);
  ofstream output1b("IP/testproblem1b");
  random_problems(3,5,0,5,output1b);
  ofstream output1c("IP/testproblem1c");
  random_problems(4,5,0,5,output1c);

  ofstream output2a("IP/testproblem2a");
  random_problems(4,5,-5,5,output2a);
  ofstream output2b("IP/testproblem2b");
  random_problems(4,5,0,5,output2b);
  ofstream output2c("IP/testproblem2c");
  random_problems(6,5,0,5,output2c);

  ofstream output3a("IP/testproblem3a");
  random_problems(3,5,-5,5,output3a);
  ofstream output3b("IP/testproblem3b");
  random_problems(3,5,0,5,output3b);
  ofstream output3c("IP/testproblem3c");
  random_problems(4,5,0,5,output3c);

  ofstream output4a("IP/testproblem4a");
  random_problems(5,5,-5,5,output4a);
  ofstream output4b("IP/testproblem4b");
  random_problems(5,5,0,5,output4b);
  ofstream output4c("IP/testproblem4c");
  random_problems(8,5,0,5,output4c);

  ofstream output5a("IP/testproblem5a");
  random_problems(3+4,5,-5,5,output5a);
  ofstream output5b("IP/testproblem5b");
  random_problems(3+4,5,0,5,output5b);
  ofstream output5c("IP/testproblem5c");
  random_problems(3*4,5,0,5,output5c);

  ofstream output6a("IP/testproblem6a");
  random_problems(3+5,5,-5,5,output6a);
  ofstream output6b("IP/testproblem6b");
  random_problems(3+5,5,0,5,output6b);
  ofstream output6c("IP/testproblem6c");
  random_problems(3*5,5,0,5,output6c);
  return 0;
}
#endif
