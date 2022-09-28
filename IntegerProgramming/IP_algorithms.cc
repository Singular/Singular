// IP_algorithms.cc

#ifndef IP_ALGORITHMS_CC
#define IP_ALGORITHMS_CC

#include "IP_algorithms.h"
#include <time.h>
#include <unistd.h>

/////////////// method for printing compilation settings ////////////////////

void print_flags(ofstream& output)
{
  output<<"compiler settings:"<<endl;

#ifdef NO_SUPPORT_DRIVEN_METHODS
  output<<"NO_SUPPORT_DRIVEN_METHODS"<<endl;
#endif

#ifdef SUPPORT_DRIVEN_METHODS

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED
  output<<"SUPPORT_DRIVEN_METHODS"<<endl;
#endif

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED
  output<<"SUPPORT_DRIVEN_METHODS_EXTENDED"<<endl;
  output<<"List Support Variables:  "<<endl;
  output<<List_Support_Variables<<endl;
#endif

#ifdef SUPPORT_VARIABLES_FIRST
  output<<"SUPPORT_VARIABLES_FIRST"<<endl;
#endif

#ifdef SUPPORT_VARIABLES_LAST
  output<<"SUPPORT_VARIABLES_LAST"<<endl;
#endif

#endif  // SUPPORT_DRIVEN_METHODS

#ifdef DL_LIST
  output<<"doubly linked lists"<<endl<<endl;
#endif

#ifdef SL_LIST
  output<<"simply linked lists"<<endl<<endl;
#endif

}




////////////////////////////////////////////////////////////////////////////
///////////////////// IP algorithms ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




int Conti_Traverso(INPUT_FILE MATRIX,
                   const int& version,
                   const int& S_pair_criteria,
                   const float& interred_percentage,
                   const BOOLEAN& verbose)
{


/////////////////////////// input /////////////////////////////////////////

  char format_string[128]; // to verify file format
  int constraints;       // number of equality constraints
  int variables;         // number of variables (without auxiliary variables)

  ifstream input(MATRIX);

  // verfifie existence of file

  if(!input)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "cannot read from input file, possibly not found"<<endl;
    return 0;
  }

  // read format specification

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading format specification,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"MATRIX"))
    cerr<<"Warning: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  // read number of variables

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"columns:"))
    cerr<<"WARNING: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>variables;

  if(!input)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(variables<=0)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "number of variables / matrix columns must be positive"<<endl;
    return 0;
  }

  // read term ordering

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"cost"))
    cerr<<"WARNING: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"vector:"))
    cerr<<"WARNING: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  term_ordering w(variables,input,W_LEX);

  if(w.error_status()<0)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading cost vector, input format not accepted"
        <<endl;
    return 0;
  }

  if(w.is_nonnegative()==FALSE)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "cost vectors with negative components are not supported"<<endl;
    return 0;
  }

  // read number of constraints

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of constraints / matrix rows,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"rows:"))
    cerr<<"WARNING: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>constraints;

  if(!input)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of constraints / matrix rows,\n"
      "input format not accepted"
        <<endl;
    return 0;
  }

  if(constraints<=0)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "number of constraints / matrix rows must be positive"<<endl;
    // Solving problems without constraints is possible, but not very
    // interesting (because trivial). To avoid the problems and the overhead
    // incurred by an "empty" matrix, such problems are refused.
    return 0;
  }

  // read matrix

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading matrix,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"matrix:"))
    cerr<<"WARNING: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  matrix A(constraints,variables,input);
  if(A.error_status()<0)
  {
    cerr<<"ERROR: int Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading matrix, input format not accepted"<<endl;
    return 0;
  }


///////////////////////// computation ////////////////////////////////////////

  // prepare time measurement
  clock_t start, end;

  // create toric ideal
  ideal I(A,w,CONTI_TRAVERSO);

  // compute the standard basis
  start=clock();
  I.reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);
  end=clock();

  // time measurement
  float elapsed=((float) (end-start))/CLOCKS_PER_SEC;


///////////////////////// output ////////////////////////////////////////////

  // create output file

  char GROEBNER[128];
  int i=0;
  while(MATRIX[i]!='\0' && MATRIX[i]!='.')
  {
    GROEBNER[i]=MATRIX[i];
    i++;
  }
  GROEBNER[i]='\0';
  strcat(GROEBNER,".GB.ct");

  ofstream output(GROEBNER);

  // format output file

  output.flags(output.flags()|ios::fixed);
  // output of fixed point numbers

  output<<"GROEBNER"<<endl<<endl;

  output<<"computed with algorithm:"<<endl;
  output<<"ct"<<endl;
  output<<"from file(s):"<<endl;
  output<<MATRIX<<endl;
  output<<"computation time"<<endl;
  output<<setw(6)<<setprecision(2)<<elapsed<<" sec"<<endl<<endl;

  output<<"term ordering:"<<endl;
  output<<"elimination block"<<endl;
  // elimination variables (>0)
  output<<constraints+1<<endl;
  output<<"LEX"<<endl;
  output<<"weighted block"<<endl;
  // weighted variables (>0)
  output<<variables<<endl;
  output<<"W_LEX"<<endl;
  w.format_print_weight_vector(output);

  output<<"size:"<<endl;
  output<<I.number_of_generators()<<endl<<endl;

  output<<"Groebner basis:"<<endl;
  I.format_print(output);
  output<<endl;

  if(verbose==TRUE)
  {
    output<<"settings for the Buchberger algorithm:"<<endl;

    output<<"version:"<<endl;
    if(version==0)
      output<<"1a"<<endl;
    else
      output<<version<<endl;

    output<<"S-pair criteria:"<<endl;
    if(S_pair_criteria & REL_PRIMENESS)
      output<<"relatively prime leading terms"<<endl;
    if(S_pair_criteria & M_CRITERION)
      output<<"criterion M"<<endl;
    if(S_pair_criteria & F_CRITERION)
      output<<"criterion F"<<endl;
    if(S_pair_criteria & B_CRITERION)
      output<<"criterion B"<<endl;
    if(S_pair_criteria & SECOND_CRITERION)
      output<<"second criterion"<<endl;
    output<<endl;

    print_flags(output);
    output<<endl;
  }

  return 1;
}




////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




int Positive_Conti_Traverso(INPUT_FILE MATRIX,
                            const int& version,
                            const int& S_pair_criteria,
                            const float& interred_percentage,
                            const BOOLEAN& verbose)
{


/////////////////////////// input /////////////////////////////////////////

  char format_string[128]; // to verify file format
  int constraints;       // number of equality constraints
  int variables;         // number of variables (without auxiliary variables)

  ifstream input(MATRIX);

  // verfifie existence of file

  if(!input)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "cannot read from input file, possibly not found"<<endl;
    return 0;
  }

  // read format specification

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading format specification,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"MATRIX"))
    cerr<<"Warning: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  // read number of variables

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"columns:"))
    cerr<<"WARNING: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>variables;

  if(!input)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(variables<=0)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "number of variables / matrix columns must be positive"<<endl;
    return 0;
  }

  // read term ordering

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      " input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"cost"))
    cerr<<"WARNING: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"vector:"))
    cerr<<"WARNING: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  term_ordering w(variables,input,W_LEX);

  if(w.error_status()<0)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading cost vector, input format not accepted"
        <<endl;
    return 0;
  }

  if(w.is_nonnegative()==FALSE)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "cost vectors with negative components are not supported"<<endl;
    return 0;
  }

  // read number of constraints

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of constraints / matrix rows,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"rows:"))
    cerr<<"WARNING: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>constraints;

  if(!input)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of constraints / matrix rows,\n"
      "input format not accepted"
        <<endl;
    return 0;
  }

  if(constraints<=0)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "number of constraints / matrix rows must be positive"<<endl;
    // Solving problems without constraints is possible, but not very
    // interesting (because trivial). To avoid the problems and the overhead
    // incurred by an "empty" matrix, such problems are refused.
    return 0;
  }

  // read matrix

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading matrix,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"matrix:"))
    cerr<<"WARNING: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  matrix A(constraints,variables,input);
  if(A.error_status()<0)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading matrix, input format not accepted"<<endl;
    return 0;
  }

  if(A.is_nonnegative()==FALSE)
  {
    cerr<<"ERROR: int Positive_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input matrix has negative coefficients,\n"
      "please use the general Conti-Traverso algorithm"<<endl;
    return 0;
  }


///////////////////////// computation ////////////////////////////////////////

  // prepare time measurement
  clock_t start, end;

  // create toric ideal
  ideal I(A,w,POSITIVE_CONTI_TRAVERSO);

  // compute the standard basis
  start=clock();
  I.reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);
  end=clock();

  // time measurement
  float elapsed=((float) (end-start))/CLOCKS_PER_SEC;


///////////////////////// output ////////////////////////////////////////////

  // create output file

  char GROEBNER[128];
  int i=0;
  while(MATRIX[i]!='\0' && MATRIX[i]!='.')
  {
    GROEBNER[i]=MATRIX[i];
    i++;
  }
  GROEBNER[i]='\0';
  strcat(GROEBNER,".GB.pct");

  ofstream output(GROEBNER);

  // format output file

  output.flags(output.flags()|ios::fixed);
  // output of fixed point numbers

  output<<"GROEBNER"<<endl<<endl;

  output<<"computed with algorithm:"<<endl;
  output<<"pct"<<endl;
  output<<"from file(s):"<<endl;
  output<<MATRIX<<endl;
  output<<"computation time"<<endl;
  output<<setw(6)<<setprecision(2)<<elapsed<<" sec"<<endl<<endl;

  output<<"term ordering:"<<endl;
  output<<"elimination block"<<endl;
  // elimination variables (>0)
  output<<constraints<<endl;
  output<<"LEX"<<endl;
  output<<"weighted block"<<endl;
  // weighted variables (>0)
  output<<variables<<endl;
  output<<"W_LEX"<<endl;
  w.format_print_weight_vector(output);
  output<<endl;

  output<<"size:"<<endl;
  output<<I.number_of_generators()<<endl<<endl;

  output<<"Groebner basis:"<<endl;
  I.format_print(output);
  output<<endl;

  if(verbose==TRUE)
  {
    output<<"settings for the Buchberger algorithm:"<<endl;

    output<<"version:"<<endl;
    if(version==0)
      output<<"1a"<<endl;
    else
      output<<version<<endl;

    output<<"S-pair criteria:"<<endl;
    if(S_pair_criteria & REL_PRIMENESS)
      output<<"relatively prime leading terms"<<endl;
    if(S_pair_criteria & M_CRITERION)
      output<<"criterion M"<<endl;
    if(S_pair_criteria & F_CRITERION)
      output<<"criterion F"<<endl;
    if(S_pair_criteria & B_CRITERION)
      output<<"criterion B"<<endl;
    if(S_pair_criteria & SECOND_CRITERION)
      output<<"second criterion"<<endl;
    output<<endl;

    print_flags(output);
    output<<endl;
  }

  return 1;
}




////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




int Elim_Conti_Traverso(INPUT_FILE MATRIX,
                        const int& version,
                        const int& S_pair_criteria,
                        const float& interred_percentage,
                        const BOOLEAN& verbose)
{


/////////////////////////// input /////////////////////////////////////////

  char format_string[128]; // to verify file format
  int constraints;       // number of equality constraints
  int variables;         // number of variables (without auxiliary variables)

  ifstream input(MATRIX);

  // verfifie existence of file

  if(!input)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "cannot read from input file, possibly not found"<<endl;
    return 0;
  }

  // read format specification

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading format specification,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"MATRIX"))
    cerr<<"Warning: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  // read number of variables

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"columns:"))
    cerr<<"WARNING: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>variables;

  if(!input)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(variables<=0)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "number of variables / matrix columns must be positive"<<endl;
    return 0;
  }

  // read term ordering

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"cost"))
    cerr<<"WARNING: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"vector:"))
    cerr<<"WARNING: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  term_ordering w(variables,input,W_LEX);

  if(w.error_status()<0)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading cost vector, input format not accepted"
        <<endl;
    return 0;
  }

  if(w.is_nonnegative()==FALSE)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "cost vectors with negative components are not supported"<<endl;
    return 0;
  }

  // read number of constraints

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of constraints / matrix rows,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"rows:"))
    cerr<<"WARNING: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>constraints;

  if(!input)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of constraints / matrix rows,\n"
      "input format not accepted"
        <<endl;
    return 0;
  }

  if(constraints<=0)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "number of constraints / matrix rows must be positive"<<endl;
    // Solving problems without constraints is possible, but not very
    // interesting (because trivial). To avoid the problems and the overhead
    // incurred by an "empty" matrix, such problems are refused.
    return 0;
  }

  // read matrix

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading matrix,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"matrix:"))
    cerr<<"WARNING: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  matrix A(constraints,variables,input);
  if(A.error_status()<0)
  {
    cerr<<"ERROR: int Elim_Conti_Traverso(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading matrix, input format not accepted"<<endl;
    return 0;
  }


///////////////////////// computation ////////////////////////////////////////

  // prepare time measurement
  clock_t start, end;

  // create toric ideal: this is done with the CONTI_TRAVERSO constructor;
  // some elimination has to be done later
  ideal I(A,w,CONTI_TRAVERSO);

  // compute the standard basis and eliminate auxiliary variables
  start=clock();
  I.reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);
  I.eliminate();
  end=clock();

  // time measurement
  float elapsed=((float) (end-start))/CLOCKS_PER_SEC;


///////////////////////// output ////////////////////////////////////////////

  // create output file

  char GROEBNER[128];
  int i=0;
  while(MATRIX[i]!='\0' && MATRIX[i]!='.')
  {
    GROEBNER[i]=MATRIX[i];
    i++;
  }
  GROEBNER[i]='\0';
  strcat(GROEBNER,".GB.ect");

  ofstream output(GROEBNER);

  // format output file

  output.flags(output.flags()|ios::fixed);
  // output of fixed point numbers

  output<<"GROEBNER"<<endl<<endl;

  output<<"computed with algorithm:"<<endl;
  output<<"ect"<<endl;
  output<<"from file(s):"<<endl;
  output<<MATRIX<<endl;
  output<<"computation time"<<endl;
  output<<setw(6)<<setprecision(2)<<elapsed<<" sec"<<endl<<endl;

  output<<"term ordering:"<<endl;
  output<<"elimination block"<<endl;
  // elimination variables (=0)
  output<<0<<endl;
  output<<"weighted block"<<endl;
  // weighted variables (>0)
  output<<variables<<endl;
  output<<"W_LEX"<<endl;
  w.format_print_weight_vector(output);

  output<<"size:"<<endl;
  output<<I.number_of_generators()<<endl<<endl;

  output<<"Groebner basis:"<<endl;
  I.format_print(output);
  output<<endl;

  if(verbose==TRUE)
  {
    output<<"settings for the Buchberger algorithm:"<<endl;

    output<<"version:"<<endl;
    if(version==0)
      output<<"1a"<<endl;
    else
      output<<version<<endl;

    output<<"S-pair criteria:"<<endl;
    if(S_pair_criteria & REL_PRIMENESS)
      output<<"relatively prime leading terms"<<endl;
    if(S_pair_criteria & M_CRITERION)
      output<<"criterion M"<<endl;
    if(S_pair_criteria & F_CRITERION)
      output<<"criterion F"<<endl;
    if(S_pair_criteria & B_CRITERION)
      output<<"criterion B"<<endl;
    if(S_pair_criteria & SECOND_CRITERION)
      output<<"second criterion"<<endl;
    output<<endl;

    print_flags(output);
    output<<endl;
  }

  return 1;
}




////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




int Pottier(INPUT_FILE MATRIX,
            const int& version,
            const int& S_pair_criteria,
            const float& interred_percentage,
            const BOOLEAN& verbose)
{


/////////////////////////// input /////////////////////////////////////////

  char format_string[128]; // to verify file format
  int constraints;       // number of equality constraints
  int variables;         // number of variables (without auxiliary variables)

  ifstream input(MATRIX);

  // verfifie existence of file

  if(!input)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "cannot read from input file, possibly not found"<<endl;
    return 0;
  }

  // read format specification

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading format specification,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"MATRIX"))
    cerr<<"Warning: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  // read number of variables

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"columns:"))
    cerr<<"WARNING: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>variables;

  if(!input)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(variables<=0)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "number of variables / matrix columns must be positive"<<endl;
    return 0;
  }

  // read term ordering

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"cost"))
    cerr<<"WARNING: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"vector:"))
    cerr<<"WARNING: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  term_ordering w(variables,input,W_LEX);

  if(w.error_status()<0)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading cost vector, input format not accepted"
        <<endl;
    return 0;
  }

  if(w.is_nonnegative()==FALSE)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "cost vectors with negative components are not supported"<<endl;
    return 0;
  }

  // read number of constraints

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of constraints / matrix rows,\n"
      " input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"rows:"))
    cerr<<"WARNING: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>constraints;

  if(!input)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of constraints / matrix rows,\n "
      "input format not accepted"
        <<endl;
    return 0;
  }

  if(constraints<=0)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "number of constraints / matrix rows must be positive"<<endl;
    // Solving problems without constraints is possible, but not very
    // interesting (because trivial). To avoid the problems and the overhead
    // incurred by an "empty" matrix, such problems are refused.
    return 0;
  }

  // read matrix

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading matrix,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"matrix:"))
    cerr<<"WARNING: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  matrix A(constraints,variables,input);
  if(A.error_status()<0)
  {
    cerr<<"ERROR: int Pottier(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading matrix, input format not accepted"<<endl;
    return 0;
  }


///////////////////////// computation ////////////////////////////////////////

  // prepare time measurement
  clock_t start, end;

  // create toric ideal
  ideal I(A,w,POTTIER);

  // compute the standard basis and eliminate auxiliary variable
  start=clock();
  I.reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);
  I.eliminate();
  end=clock();

  // time measurement
  float elapsed=((float) (end-start))/CLOCKS_PER_SEC;


///////////////////////// output ////////////////////////////////////////////

  // create output file

  char GROEBNER[128];
  int i=0;
  while(MATRIX[i]!='\0' && MATRIX[i]!='.')
  {
    GROEBNER[i]=MATRIX[i];
    i++;
  }
  GROEBNER[i]='\0';
  strcat(GROEBNER,".GB.pt");

  ofstream output(GROEBNER);

  // format output file

  output.flags(output.flags()|ios::fixed);
  // output of fixed point numbers

  output<<"GROEBNER"<<endl<<endl;

  output<<"computed with algorithm:"<<endl;
  output<<"pt"<<endl;
  output<<"from file(s):"<<endl;
  output<<MATRIX<<endl;
  output<<"computation time"<<endl;
  output<<setw(6)<<setprecision(2)<<elapsed<<" sec"<<endl<<endl;

  output<<"term ordering:"<<endl;
  output<<"elimination block"<<endl;
  // elimination variables (=0)
  output<<0<<endl;
  output<<"weighted block"<<endl;
  // weighted variables (>0)
  output<<variables<<endl;
  output<<"W_LEX"<<endl;
  w.format_print_weight_vector(output);

  output<<"size:"<<endl;
  output<<I.number_of_generators()<<endl<<endl;

  output<<"Groebner basis:"<<endl;
  I.format_print(output);
  output<<endl;

  if(verbose==TRUE)
  {
    output<<"settings for the Buchberger algorithm:"<<endl;

    output<<"version:"<<endl;
    if(version==0)
      output<<"1a"<<endl;
    else
      output<<version<<endl;

    output<<"S-pair criteria:"<<endl;
    if(S_pair_criteria & REL_PRIMENESS)
      output<<"relatively prime leading terms"<<endl;
    if(S_pair_criteria & M_CRITERION)
      output<<"criterion M"<<endl;
    if(S_pair_criteria & F_CRITERION)
      output<<"criterion F"<<endl;
    if(S_pair_criteria & B_CRITERION)
      output<<"criterion B"<<endl;
    if(S_pair_criteria & SECOND_CRITERION)
      output<<"second criterion"<<endl;
    output<<endl;

    print_flags(output);
    output<<endl;
  }

  return 1;
}




////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




int Hosten_Sturmfels(INPUT_FILE MATRIX,
                     const int& version,
                     const int& S_pair_criteria,
                     const float& interred_percentage,
                     const BOOLEAN& verbose)
{


/////////////////////////// input /////////////////////////////////////////

  char format_string[128]; // to verify file format
  int constraints;       // number of equality constraints
  int variables;         // number of variables

  ifstream input(MATRIX);

  // verfifie existence of file

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "cannot read from input file, possibly not found"<<endl;
    return 0;
  }

  // read format specification

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading format specification,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"MATRIX"))
    cerr<<"Warning: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  // read number of variables

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"columns:"))
    cerr<<"WARNING: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>variables;

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(variables<=0)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "number of variables / matrix columns must be positive"<<endl;
    return 0;
  }

  // read term ordering

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"cost"))
    cerr<<"WARNING: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"vector:"))
    cerr<<"WARNING: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  term_ordering c(variables,input,W_LEX);

  if(c.error_status()<0)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading cost vector, input format not accepted"
        <<endl;
    return 0;
  }

  if(c.is_nonnegative()==FALSE)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "cost vectors with negative components are not supported"<<endl;
    return 0;
  }

  // read number of constraints

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of constraints / matrix rows,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"rows:"))
    cerr<<"WARNING: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>constraints;

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of constraints / matrix rows,\n"
      "input format not accepted"
        <<endl;
    return 0;
  }

  if(constraints<=0)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "number of constraints / matrix rows must be positive"<<endl;
    // Solving problems without constraints is possible, but not very
    // interesting (because trivial). To avoid the problems and the overhead
    // incurred by an "empty" matrix, such problems are refused.
    return 0;
  }

  // read matrix

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading matrix,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"matrix:"))
    cerr<<"WARNING: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  matrix A(constraints,variables,input);
  if(A.error_status()<0)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading matrix, input format not accepted"<<endl;
    return 0;
  }

  // read positive vector in the row space of the matrix
  // such a vector induces a homogeneous grading on the ideal

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading positive row space vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"positive"))
    cerr<<"WARNING: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading positive row space vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"row"))
    cerr<<"WARNING: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading positive row space vector,\n"
      " input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"space"))
    cerr<<"WARNING: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading positive row space vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"vector:"))
    cerr<<"WARNING: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  float *hom_grad=new float[variables];

  for(int i=0;i<variables;i++)
  {
    input>>hom_grad[i];

    if(!input)
    {
      cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
        "input failure when reading positive grading / row space vector,\n"
        "input format not accepted"<<endl;
      delete[] hom_grad;
      return 0;
    }

    if(hom_grad[i]<=0)
    {
      cerr<<"ERROR: int Hosten_Sturmfels(INPUT_FILE, const BOOLEAN&):\n"
        "row space vector / grading must be positive"<<endl;
      delete[] hom_grad;
      return 0;
    }
  }


///////////////////////// computation ////////////////////////////////////////

  // prepare time measurement
  clock_t start, end;

  // construct homogeneous term ordering
  term_ordering w(variables, hom_grad, W_REV_LEX, HOMOGENEOUS);

  delete[] hom_grad;

  // create toric ideal
  ideal I(A,w,HOSTEN_STURMFELS);

  // compute the standard basis of the saturation:

  start=clock();

  // determine saturation variables
  int *sat_var;
  int number_of_sat_var=A.hosten_shapiro(sat_var);
  // memory for sat_var is allocated in the hosten_shapiro procedure

  // saturate the ideal
  for(int i=0;i<number_of_sat_var;i++)
  {
    I.swap_variables(sat_var[i],variables-1);
    // This operation simply makes the i-th saturation variable the cheapest.
    // This involves swapping the weights in the ideal's term ordering,
    // the variables in the generating binomials and rebuilding the
    // list structure if SUPPORT_DRIVEN_METHODS_EXTENDED are used.

    I.reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);
    // This calculation saturates the ideal with respect to the i-th
    // variable.

    I.swap_variables_unsafe(sat_var[i],variables-1);
    // This operation does the same as ideal::swap_variables(...) except
    // from rebuilding the list structure. It may cause an inconsistent
    // structure, but is more efficient then the safe method. Before
    // performing a Groebner basis computation, however, the ideal structure
    // has to be rebuild. Procedures like ideal::swap_variables(...) and
    // ideal::change_term_ordering_to(...) do this. But these are exactly
    // the operations that follow.
  }

  delete[] sat_var;

  // Now the ideal is saturated. The last Groebner basis computation is done
  // with respect to the term ordering induced by the objective function.
  I.change_term_ordering_to(c);
  I.reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);

  end=clock();

  // time measurement
  float elapsed=((float) (end-start))/CLOCKS_PER_SEC;


///////////////////////// output ////////////////////////////////////////////

  // create output file

  char GROEBNER[128];
  int i=0;
  while(MATRIX[i]!='\0' && MATRIX[i]!='.')
  {
    GROEBNER[i]=MATRIX[i];
    i++;
  }
  GROEBNER[i]='\0';
  strcat(GROEBNER,".GB.hs");

  ofstream output(GROEBNER);

  // format output file

  output.flags(output.flags()|ios::fixed);
  // output of fixed point numbers

  output<<"GROEBNER"<<endl<<endl;

  output<<"computed with algorithm:"<<endl;
  output<<"hs"<<endl;
  output<<"from file(s):"<<endl;
  output<<MATRIX<<endl;
  output<<"computation time"<<endl;
  output<<setw(6)<<setprecision(2)<<elapsed<<" sec"<<endl<<endl;

  output<<"term ordering:"<<endl;
  output<<"elimination block"<<endl;
  // elimination variables (=0)
  output<<0<<endl;
  output<<"weighted block"<<endl;
  // weighted variables (>0)
  output<<variables<<endl;
  output<<"W_LEX"<<endl;
  c.format_print_weight_vector(output);

  output<<"size:"<<endl;
  output<<I.number_of_generators()<<endl<<endl;

  output<<"Groebner basis:"<<endl;
  I.format_print(output);
  output<<endl;

  if(verbose==TRUE)
  {
    output<<"settings for the Buchberger algorithm:"<<endl;

    output<<"version:"<<endl;
    if(version==0)
      output<<"1a"<<endl;
    else
      output<<version<<endl;

    output<<"S-pair criteria:"<<endl;
    if(S_pair_criteria & REL_PRIMENESS)
      output<<"relatively prime leading terms"<<endl;
    if(S_pair_criteria & M_CRITERION)
      output<<"criterion M"<<endl;
    if(S_pair_criteria & F_CRITERION)
      output<<"criterion F"<<endl;
    if(S_pair_criteria & B_CRITERION)
      output<<"criterion B"<<endl;
    if(S_pair_criteria & SECOND_CRITERION)
      output<<"second criterion"<<endl;
    output<<endl;

    print_flags(output);
    output<<endl;
  }

  return 1;
}




////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




int DiBiase_Urbanke(INPUT_FILE MATRIX,
                    const int& version,
                    const int& S_pair_criteria,
                    const float& interred_percentage,
                    const BOOLEAN& verbose)
{


/////////////////////////// input /////////////////////////////////////////

  char format_string[128]; // to verify file format
  int constraints;       // number of equality constraints
  int variables;         // number of variables

  ifstream input(MATRIX);

  // verfifie existence of file

  if(!input)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "cannot read from input file, possibly not found"<<endl;
    return 0;
  }

  // read format specification

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading format specification,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"MATRIX"))
    cerr<<"Warning: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  // read number of variables

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"columns:"))
    cerr<<"WARNING: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>variables;

  if(!input)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(variables<=0)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "number of variables / matrix columns must be positive"<<endl;
    return 0;
  }

  // read term ordering

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"cost"))
    cerr<<"WARNING: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      " input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"vector:"))
    cerr<<"WARNING: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  term_ordering c(variables,input,W_LEX);

  if(c.error_status()<0)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading cost vector, input format not accepted"
        <<endl;
    return 0;
  }

  if(c.is_nonnegative()==FALSE)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "cost vectors with negative components are not supported"<<endl;
    return 0;
  }

  // read number of constraints

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of constraints / matrix rows,\n"
      " input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"rows:"))
    cerr<<"WARNING: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  input>>constraints;

  if(!input)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of constraints / matrix rows,\n"
      "input format not accepted"
        <<endl;
    return 0;
  }

  if(constraints<=0)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "number of constraints / matrix rows must be positive"<<endl;
    // Solving problems without constraints is possible, but not very
    // interesting (because trivial). To avoid the problems and the overhead
    // incurred by an "empty" matrix, such problems are refused.
    return 0;
  }

  // read matrix

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading matrix,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"matrix:"))
    cerr<<"WARNING: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input file has suspicious format"<<endl;

  matrix A(constraints,variables,input);
  if(A.error_status()<0)
  {
    cerr<<"ERROR: int DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading matrix, input format not accepted"<<endl;
    return 0;
  }


///////////////////////// computation ////////////////////////////////////////

  // prepare time measurement
  clock_t start, end;

  // compute flip variables (also to check the suitability of the algorithm)
  int* F;
  int r=A.compute_flip_variables(F);

  if(r<0)
    // algorithm not suitable
  {
    cout<<"ERROR: DiBiase_Urbanke(INPUT_FILE, const BOOLEAN&):\n"
      "Kernel of the input matrix contains no vector with nonzero "
      "components,\ninstance cannot be solved with this algorithm.\n"
      "No output file created. Please use another algorithm."<<endl;
    return 0;
  }

  ideal *I;
  // We use a pointer here because we need to distinguish two cases
  // for the ideal construction.

  start=clock();

  if(r==0)
    // no variable flip needed
    // create toric ideal from the lattice basis already with respect to the
    // objective function
  {
    I=new ideal(A,c,DIBIASE_URBANKE);
    I->reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);
  }

  else
  {
    // construct term ordering to start with
    // Here we have much freedom: The term ordering must only be an
    // elimination ordering for the actual flip variable.
    // To avoid supplementary functions to manipulate term orderings, we
    // simply realize this as follows: The weight of the actual
    // flip variable is set to 1, all other weights are set to zero. This
    // still allows the use of different term orderings (by setting the
    // refining ordering).

    float* weights=new float[variables];
    for(int j=0;j<variables;j++)
      weights[j]=0;
    weights[F[0]]=1;

    term_ordering w(variables, weights, W_LEX);
    // Which term ordering is the best here?

    delete[] weights;

    // create toric ideal
    I=new ideal(A,w,DIBIASE_URBANKE);

    I->reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);
    I->flip_variable_unsafe(F[0]);
    // "unsafe" means that head and tail can be exchanged (cf. the routine
    // ideal::swap_variables_unsafe(...) )
    // But the following change of the term ordering will correct this.

    for(int l=1;l<r;l++)
    {
      w.swap_weights(F[l-1],F[l]);
      // This means concretely:
      // The weight of x_F[l-1] is set to zero, that of x_F[l] to one.
      // Now, x_F[l] is the elimination variable.

      I->change_term_ordering_to(w);
      I->reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);
      I->flip_variable_unsafe(F[l]);
    }

    // Now we have a generating system of the saturated ideal.
    // Compute Groebner basis with respect to the objective function.
    I->change_term_ordering_to(c);
    I->reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);

  }

  end=clock();

  // time measurement
  float elapsed=((float) (end-start))/CLOCKS_PER_SEC;


///////////////////////// output ////////////////////////////////////////////

  // create output file

  char GROEBNER[128];
  int i=0;
  while(MATRIX[i]!='\0' && MATRIX[i]!='.')
  {
    GROEBNER[i]=MATRIX[i];
    i++;
  }
  GROEBNER[i]='\0';
  strcat(GROEBNER,".GB.du");

  ofstream output(GROEBNER);

  // format output file

  output.flags(output.flags()|ios::fixed);
  // output of fixed point numbers

  output<<"GROEBNER"<<endl<<endl;

  output<<"computed with algorithm:"<<endl;
  output<<"du"<<endl;
  output<<"from file(s):"<<endl;
  output<<MATRIX<<endl;
  output<<"computation time"<<endl;
  output<<setw(6)<<setprecision(2)<<elapsed<<" sec"<<endl<<endl;

  output<<"term ordering:"<<endl;
  output<<"elimination block"<<endl;
  // elimination variables (=0)
  output<<0<<endl;
  output<<"weighted block"<<endl;
  // weighted variables (>0)
  output<<variables<<endl;
  output<<"W_LEX"<<endl;
  c.format_print_weight_vector(output);

  output<<"size:"<<endl;
  output<<I->number_of_generators()<<endl<<endl;

  output<<"Groebner basis:"<<endl;
  I->format_print(output);
  output<<endl;

  if(verbose==TRUE)
  {
    output<<"settings for the Buchberger algorithm:"<<endl;

    output<<"version:"<<endl;
    if(version==0)
      output<<"1a"<<endl;
    else
      output<<version<<endl;

    output<<"S-pair criteria:"<<endl;
    if(S_pair_criteria & REL_PRIMENESS)
      output<<"relatively prime leading terms"<<endl;
    if(S_pair_criteria & M_CRITERION)
      output<<"criterion M"<<endl;
    if(S_pair_criteria & F_CRITERION)
      output<<"criterion F"<<endl;
    if(S_pair_criteria & B_CRITERION)
      output<<"criterion B"<<endl;
    if(S_pair_criteria & SECOND_CRITERION)
      output<<"second criterion"<<endl;
    output<<endl;

    print_flags(output);
    output<<endl;
  }


////////////////////////////// memory cleanup ////////////////////////////////

  if(r>0)
    delete[] F;
  delete I;

  return 1;
}




////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




int Bigatti_LaScala_Robbiano(INPUT_FILE MATRIX,
                             const int& version,
                             const int& S_pair_criteria,
                             const float& interred_percentage,
                             const BOOLEAN& verbose)
{


/////////////////////////// input /////////////////////////////////////////

  char format_string[128];     // to verify file format
  int constraints;       // number of equality constraints
  int variables;         // number of variables

  ifstream input(MATRIX);

  // verfifie existence of file

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "cannot read from input file, possibly not found"<<endl;
    return 0;
  }

  // read format specification

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading format specification,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"MATRIX"))
    cerr<<"Warning: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):"
      "\n"
      "input file has suspicious format"<<endl;

  // read number of variables

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"columns:"))
    cerr<<"WARNING: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):"
      "\n"
      "input file has suspicious format"<<endl;

  input>>variables;

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of variables / matrix columns,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(variables<=0)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "number of variables / matrix columns must be positive"<<endl;
    return 0;
  }

  // read term ordering

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"cost"))
    cerr<<"WARNING: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):"
      "\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading cost vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"vector:"))
    cerr<<"WARNING: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):"
      "\n"
      "input file has suspicious format"<<endl;

  term_ordering c(variables,input,W_LEX);

  if(c.error_status()<0)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading cost vector, input format not accepted"
        <<endl;
    return 0;
  }

  if(c.is_nonnegative()==FALSE)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "cost vectors with negative components are not supported"<<endl;
    return 0;
  }

  // read number of constraints

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading number of constraints / matrix rows,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"rows:"))
    cerr<<"WARNING: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):"
      "\n"
      "input file has suspicious format"<<endl;

  input>>constraints;

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading number of constraints / matrix rows,\n"
      "input format not accepted"
        <<endl;
    return 0;
  }

  if(constraints<=0)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "number of constraints / matrix rows must be positive"<<endl;
    // Solving problems without constraints is possible, but not very
    // interesting (because trivial). To avoid the problems and the overhead
    // incurred by an "empty" matrix, such problems are refused.
    return 0;
  }

  // read matrix

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading matrix,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"matrix:"))
    cerr<<"WARNING: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):"
      "\n"
      "input file has suspicious format"<<endl;

  matrix A(constraints,variables,input);
  if(A.error_status()<0)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure when reading matrix, input format not accepted"<<endl;
    return 0;
  }

  // read positive vector in the row space of the matrix
  // such a vector induces a grading with respect to which the ideal is
  // homogeneous

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading positive row space vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"positive"))
    cerr<<"WARNING: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):"
      "\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading positive row space vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"row"))
    cerr<<"WARNING: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):"
      "\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading positive row space vector,\n"
      " input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"space"))
    cerr<<"WARNING: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):"
      "\n"
      "input file has suspicious format"<<endl;

  input>>format_string;

  if(!input)
  {
    cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):\n"
      "input failure before reading positive row space vector,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"vector:"))
    cerr<<"WARNING: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):"
      "\n"
      "input file has suspicious format"<<endl;

  float *hom_grad=new float[variables];

  for(int _i=0;_i<variables;_i++)
  {
    input>>hom_grad[_i];

    if(!input)
    {
      cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):"
        "\n"
        "input failure when reading positive grading / row space vector,\n"
        "input format not accepted"<<endl;
      delete[] hom_grad;
      return 0;
    }

    if(hom_grad[_i]<=0)
    {
      cerr<<"ERROR: int Bigatti_LaScala_Robbiano(INPUT_FILE, const BOOLEAN&):"
        "\n"
        "row space vector / grading must be positive"<<endl;
      delete[] hom_grad;
      return 0;
    }
  }



///////////////////////// computation ////////////////////////////////////////

  // prepare time measurement
  clock_t start, end;

  // construct homogeneous term ordering
  term_ordering w(variables, hom_grad, W_REV_LEX, HOMOGENEOUS);

  delete[] hom_grad;

  // create toric ideal
  ideal I(A,w,BIGATTI_LASCALA_ROBBIANO);

  // compute the standard basis
  start=clock();
  I.reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);

  // now we have a generating system
  // to perform the substitution of the auxiliary variable U by the saturation
  // variables:
  // make U the revlex most expensive variable by swapping with the first,
  // recompute the Groebner basis, undo the swap
  I.swap_variables(0,variables);
  I.reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);
  I.swap_variables(0,variables);

  I.pseudo_eliminate();
  // eliminate the auxiliary variable

  // Now the ideal is saturated. Compute the Groebner basis
  // with respect to the term ordering induced by the objective function.
  I.change_term_ordering_to(c);
  I.reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);

  end=clock();

  // time measurement
  float elapsed=((float) (end-start))/CLOCKS_PER_SEC;


///////////////////////// output ////////////////////////////////////////////

  // create output file

  char GROEBNER[128];
  int i=0;
  while(MATRIX[i]!='\0' && MATRIX[i]!='.')
  {
    GROEBNER[i]=MATRIX[i];
    i++;
  }
  GROEBNER[i]='\0';
  strcat(GROEBNER,".GB.blr");

  ofstream output(GROEBNER);

  // format output file

  output.flags(output.flags()|ios::fixed);
  // output of fixed point numbers

  output<<"GROEBNER"<<endl<<endl;

  output<<"computed with algorithm:"<<endl;
  output<<"blr"<<endl;
  output<<"from file(s):"<<endl;
  output<<MATRIX<<endl;
  output<<"computation time"<<endl;
  output<<setw(6)<<setprecision(2)<<elapsed<<" sec"<<endl<<endl;

  output<<"term ordering:"<<endl;
  output<<"elimination block"<<endl;
  // elimination variables (00)
  output<<0<<endl;
  output<<"weighted block"<<endl;
  // weighted variables (>0)
  output<<variables<<endl;
  output<<"W_LEX"<<endl;
  c.format_print_weight_vector(output);

  output<<"size:"<<endl;
  output<<I.number_of_generators()<<endl<<endl;

  output<<"Groebner basis:"<<endl;
  I.format_print(output);
  output<<endl;

  if(verbose==TRUE)
  {
    output<<"settings for the Buchberger algorithm:"<<endl;

    output<<"version:"<<endl;
    if(version==0)
      output<<"1a"<<endl;
    else
      output<<version<<endl;

    output<<"S-pair criteria:"<<endl;
    if(S_pair_criteria & REL_PRIMENESS)
      output<<"relatively prime leading terms"<<endl;
    if(S_pair_criteria & M_CRITERION)
      output<<"criterion M"<<endl;
    if(S_pair_criteria & F_CRITERION)
      output<<"criterion F"<<endl;
    if(S_pair_criteria & B_CRITERION)
      output<<"criterion B"<<endl;
    if(S_pair_criteria & SECOND_CRITERION)
      output<<"second criterion"<<endl;
    output<<endl;

    print_flags(output);
    output<<endl;
  }

  return 1;
}




////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




int solve(INPUT_FILE PROBLEM, INPUT_FILE GROEBNER)
{

  char format_string[128];
  int problem_variables;
  int elimination_variables;
  int weighted_variables;
  char elimination_refinement[128];
  char weighted_refinement[128];
  char algorithm[128];
  long size;
  long instances;

  ifstream problem(PROBLEM);
  ifstream groebner(GROEBNER);

  // verfifie existence of files

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "cannot read from first input file "<< PROBLEM <<", possibly not found"<<endl;
    return 0;
  }

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "cannot read from second input file "<< GROEBNER <<", possibly not found"<<endl;
    return 0;
  }

// read GROEBNER file

  // read format specification

  groebner>>format_string;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading format specification of second input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"GROEBNER"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  // read algorithm

  groebner>>format_string;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading algorithm from second input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"computed"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  groebner>>format_string;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading algorithm from second input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"with"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  groebner>>format_string;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading algorithm from second input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"algorithm:"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  groebner>>algorithm;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading algorithm from second input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(algorithm,"ct") && strcmp(algorithm,"pct") &&
     strcmp(algorithm,"ect") && strcmp(algorithm,"pt") &&
     strcmp(algorithm,"hs") && strcmp(algorithm,"du")
     && strcmp(algorithm,"blr"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file computed with unknown algorithm"<<endl;

  // override optional lines

  do
  {
    groebner>>format_string;

    if(!groebner)
    {
      cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
        "input failure before reading term ordering from second \n"
        "input file, input format not accepted"<<endl;
      return 0;
    }
  }
  while(strcmp(format_string,"term"));

  // read term ordering

  groebner>>format_string;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading term ordering \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"ordering:"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  groebner>>format_string;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"elimination"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  groebner>>format_string;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"block"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  groebner>>elimination_variables;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(elimination_variables<0)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "number of elimination variables in second input file must be "
      "nonnegative"<<endl;
    return 0;
  }

  if(elimination_variables>0)
  {
    groebner>>elimination_refinement;

    if(!groebner)
    {
      cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
        "input failure when reading term ordering \n"
        "from second input file, input format not accepted"<<endl;
      return 0;
    }

    if(strcmp(elimination_refinement,"LEX") &&
       strcmp(elimination_refinement,"DEG_LEX") &&
       strcmp(elimination_refinement,"DEG_REV_LEX"))
    {
      cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
        "unknown elimination ordering in second input file"<<endl;
      return 0;
    }
  }

  groebner>>format_string;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"weighted"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  groebner>>format_string;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"block"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  groebner>>weighted_variables;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(weighted_variables<=0)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "number of weighted variables in second input file must be "
      "positive"<<endl;
    return 0;
  }

  groebner>>weighted_refinement;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(weighted_refinement,"W_LEX") &&
     strcmp(weighted_refinement,"W_REV_LEX") &&
     strcmp(weighted_refinement,"W_DEG_LEX") &&
     strcmp(weighted_refinement,"W_DEG_REV_LEX"))
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "unknown weighted ordering in second input file"<<endl;
    return 0;
  }

  int weighted_ref;
  if(!strcmp(weighted_refinement,"W_LEX"))
    weighted_ref=W_LEX;
  if(!strcmp(weighted_refinement,"W_REV_LEX"))
    weighted_ref=W_REV_LEX;
  if(!strcmp(weighted_refinement,"W_DEG_LEX"))
    weighted_ref=W_DEG_LEX;
  if(!strcmp(weighted_refinement,"W_DEG_REV_LEX"))
    weighted_ref=W_DEG_REV_LEX;

  term_ordering w(weighted_variables,groebner,weighted_ref);

  if(w.error_status()<0)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading new cost vector from second input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(w.is_nonnegative()==FALSE)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "cost vectors with negative components are not supported"<<endl;
    return 0;
  }

  if(elimination_variables>0)
  {
    int elimination_ref;
    if(!strcmp(elimination_refinement,"LEX"))
      elimination_ref=LEX;
    if(!strcmp(elimination_refinement,"DEG_LEX"))
      elimination_ref=DEG_LEX;
    if(!strcmp(elimination_refinement,"DEG_REV_LEX"))
      elimination_ref=DEG_REV_LEX;

    w.convert_to_elimination_ordering(elimination_variables,elimination_ref);

    if(w.error_status()<0)
    {
      cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
        "input failure when reading new cost vector from second input file,\n"
        "input format not accepted"<<endl;
      return 0;
    }
  }

  // read ideal

  groebner>>format_string;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading number of ideal generators \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"size:"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  groebner>>size;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading number of ideal generators \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(size<0)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "ideal in second input file is corrupt"<<endl;
    return 0;
  }

  groebner>>format_string;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading ideal generators \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"Groebner"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  groebner>>format_string;

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading ideal generators \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"basis:"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  ideal I(groebner,w,size);

  if(!groebner)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading ideal generators from second input file,\n"
      "input format not accepted."<<endl;
    return 0;
  }

  if(I.error_status()<0)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "cannot compute with corrupt ideal\n"<<endl;
    return 0;
  }

// read PROBLEM file

  // read format specification

  problem>>format_string;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading format specification of first input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"PROBLEM"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;

  // read vector dimension

  problem>>format_string;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading vector dimension from first input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"vector"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;

  problem>>format_string;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading vector dimension from first input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"size:"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;

  problem>>problem_variables;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading vector dimension from first input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(problem_variables<=0)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "vector dimension in first input file must be positive"<<endl;
    return 0;
  }

  // consistency with respect to the number of variables is checked later

  // read number of instances

  problem>>format_string;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading number of instances from first input file,"
      "\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"number"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;

  problem>>format_string;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading number of instances from first input file,"
      "\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"of"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;

  problem>>format_string;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading number of instances from first input file,"
      "\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"instances:"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;

  problem>>instances;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading number of instances from first input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(instances<=0)
  {
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "number of instances is <1, no output file created"<<endl;
    return 1;
    // no error
  }

  // read problem vectors (first part)

  problem>>format_string;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading right hand / initial solution vectors \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"right"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;

  problem>>format_string;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading right hand / initial solution vectors \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"hand"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;

  problem>>format_string;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading right hand / initial solution vectors \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"or"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;

  problem>>format_string;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading right hand / initial solution vectors \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"initial"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;

  problem>>format_string;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading right hand / initial solution vectors \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"solution"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;

  problem>>format_string;

  if(!problem)
  {
    cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading right hand / initial solution vectors \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"vectors:"))
    cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;

  // the vectors are read in the section "computation" because we need to
  // distinguish between the different algorithms


//////////////////// output (first part) ///////////////////////////////////

  // open output file (append mode)

  char SOLUTION[128];

  int i=0;
  while(GROEBNER[i]!='\0' && GROEBNER[i]!='.')
  {
    SOLUTION[i]=GROEBNER[i];
    i++;
  }
  SOLUTION[i]='\0';
  strcat(SOLUTION,".sol.");
  strcat(SOLUTION,algorithm);

  ofstream output(SOLUTION,ios::app);

  // format output file

  output.flags(output.flags()|ios::fixed);
  // output of fixed point numbers

  output<<"SOLUTION"<<endl<<endl;

  output<<"computed with algorithm:"<<endl;
  output<<algorithm<<endl;
  output<<"from file:"<<endl;
  output<<GROEBNER<<endl;


/////////// computation and output (second part) //////////////////////////

  // distinguish 3 cases to verify the consistency of the vector dimension
  // and the number of variables

  // Conti-Traverso: vectors are read as right hand vectors
  // vector dimension = number of elimination variables without inversion
  // variable
  if(!strcmp(algorithm,"ct"))
  {
    if(problem_variables!=elimination_variables-1)
    {
      cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
        "vector dimension in first input file and number of variables in\n"
        "second input file do not match with respect to the\n"
        "Conti-Traverso-algorithm"<<endl;
      return 0;
    }

    Integer right_hand[weighted_variables+elimination_variables];

    for(int k=0;k<instances;k++)
    {
      // at the beginning, the variables of interest are zero
      for(i=0;i<weighted_variables;i++)
        right_hand[i]=0;

      // right hand vector is read from the input stream into the
      // elimination variables
      for(i=weighted_variables;
          i<weighted_variables+elimination_variables-1;i++)
      {
        problem>>right_hand[i];

        if(!problem)
        {
          cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
            "input failure when reading right hand vector "<<k+1<<" from "
            "first input \nfile, "
            "this and all following right hand vectors will be ignored"<<endl;
          return 0;
        }
      }

      // determine the exponent of the inversion variable, i.e.
      // - min{negative components of right_hand}.
      Integer min=0;
      for(i=weighted_variables;
          i<weighted_variables+elimination_variables-1;i++)
        if(right_hand[i]<min)
          min=right_hand[i];

      // transform right_hand so that all components are nonnegative
      if(min<0)
        for(i=weighted_variables;
            i<weighted_variables+elimination_variables-1;i++)
          right_hand[i]-=min;

      // set exponent of the inversion variable
      right_hand[weighted_variables+elimination_variables-1]=-min;

      // construct binomial to reduce
      binomial to_reduce(weighted_variables+elimination_variables,right_hand);

      // prepare time measurement
      clock_t start, end;

      // reduce binomial
      start=clock();
      I.reduce(to_reduce,TRUE);
      end=clock();

      // time measurement
      float elapsed=((float) (end-start))/CLOCKS_PER_SEC;

      // output

      output<<"right hand vector:"<<endl;
      for(i=weighted_variables;
          i<weighted_variables+elimination_variables-1;i++)
        output<<setw(6)<<right_hand[i]+min;
        // original vector
      output<<endl;

      output<<"solvable:"<<endl;

      BOOLEAN solvable=TRUE;
      for(i=weighted_variables;
          i<=weighted_variables+elimination_variables-1;i++)
        if(to_reduce[i]!=0)
        {
          solvable=FALSE;
          break;
        }

      if(solvable==TRUE)
      {
        output<<"YES"<<endl;
        output<<"optimal solution:"<<endl;
        for(i=0;i<weighted_variables;i++)
          output<<setw(6)<<to_reduce[i];
        output<<endl;
      }
      else
        output<<"NO"<<endl;

      output<<"computation time"<<endl;
      output<<setw(6)<<setprecision(2)<<elapsed<<" sec"<<endl<<endl;
    }
  }

  else
    // Positive Conti-Traverso: vectors are read as right hand vectors
    // vector dimension = number of elimination variables
    if(!strcmp(algorithm,"pct"))
    {
      if(problem_variables!=elimination_variables)
      {
        cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
          "vector dimension in first input file and number of variables in\n"
          "second input file do not match with respect to the\n"
          "Positive Conti-Traverso algorithm"<<endl;
        return 0;
      }

      Integer right_hand[weighted_variables+elimination_variables];
      BOOLEAN error=FALSE;    // to test legality of right hand vectors

      for(int k=0;k<instances;k++)
      {
        // at the beginning, the variables of interest are zero
        for(i=0;i<weighted_variables;i++)
          right_hand[i]=0;

        // right hand vector is read from the input stream into the
        // elimination variables
        for(i=weighted_variables;
            i<weighted_variables+elimination_variables;i++)
        {
          problem>>right_hand[i];

          if(!problem)
          {
            cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
              "input failure when reading right hand vector "<<k+1<<" from "
              "first input \nfile, this and all following right hand vectors "
              "will be ignored"<<endl;
            return 0;
          }

          if(right_hand[i]<0)
          {
            cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
              "right hand vectors with negative components cannot be handled\n"
              "by the Positive Conti-Traverso algorithm,\n"
              "right hand vector "<<k+1<<" will be ignored"<<endl;
            error=TRUE;
          }
        }

        if(error==TRUE)
        {
          error=FALSE;
          continue;
          // for-loop
        }

        // construct binomial to reduce
        binomial to_reduce(weighted_variables+elimination_variables,
                           right_hand);

        // prepare time measurement
        clock_t start, end;

        // reduce binomial
        start=clock();
        I.reduce(to_reduce,TRUE);
        end=clock();

        // time measurement
        float elapsed=((float) (end-start))/CLOCKS_PER_SEC;

        // output

        output<<"right hand vector:"<<endl;
        for(i=weighted_variables;
            i<weighted_variables+elimination_variables;i++)
          output<<setw(6)<<right_hand[i];
        // original vector
        output<<endl;

        output<<"solvable:"<<endl;

        BOOLEAN solvable=TRUE;
        for(i=weighted_variables;
            i<weighted_variables+elimination_variables;i++)
          if(to_reduce[i]!=0)
          {
            solvable=FALSE;
            break;
          }

        if(solvable==TRUE)
        {
          output<<"YES"<<endl;
          output<<"optimal solution:"<<endl;
          for(i=0;i<weighted_variables;i++)
            output<<setw(6)<<to_reduce[i];
          output<<endl;
        }
        else
          output<<"NO"<<endl;

        output<<"computation time"<<endl;
        output<<setw(6)<<setprecision(2)<<elapsed<<" sec"<<endl<<endl;
      }
    }

    else
      // other algorithms: vectors are read as initial solutions
      // vector dimension = number of weighted variables
    {
      if(problem_variables!=weighted_variables)
      {
        cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
          "vector dimension in first input file and number of variables in\n"
          "second input file do not match with respect to the\n"
          "chosen algorithm"<<endl;
        return 0;
      }

      Integer initial_solution[weighted_variables];

      for(int k=0;k<instances;k++)
      {
        // initial solution vector is read from the input stream into the
        // elimination variables
        for(i=0;i<weighted_variables;i++)
        {
          problem>>initial_solution[i];

          if(!problem)
          {
            cerr<<"ERROR: int solve(INPUT_FILE, INPUT_FILE):\n"
              "input failure when reading right hand vector "<<k+1<<" from "
              "first input \nfile, this and all following right hand vectors "
              "will be ignored"<<endl;
            return 0;
          }

          if(initial_solution[i]<0)
            cerr<<"WARNING: int solve(INPUT_FILE, INPUT_FILE):\n"
              "initial solution vectors should be nonnegative"<<endl;
        }

        // construct binomial to reduce
        binomial to_reduce(weighted_variables,initial_solution);

        // prepare time measurement
        clock_t start, end;

        // reduce binomial
        start=clock();
        I.reduce(to_reduce,TRUE);
        end=clock();

        // time measurement
        float elapsed=((float) (end-start))/CLOCKS_PER_SEC;

        // output

        output<<"initial solution vector:"<<endl;
        for(i=0;i<weighted_variables;i++)
          output<<setw(6)<<initial_solution[i];
        // original vector
        output<<endl;

        output<<"optimal solution:"<<endl;
        for(i=0;i<weighted_variables;i++)
          output<<setw(6)<<to_reduce[i];
        output<<endl;

        output<<"computation time"<<endl;
        output<<setw(6)<<setprecision(2)<<elapsed<<" sec"<<endl<<endl;
      }
    }

  return 1;

}



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




int change_cost(INPUT_FILE GROEBNER, INPUT_FILE NEW_COST,
                const int& version,
                const int& S_pair_criteria,
                const float& interred_percentage,
                const BOOLEAN& verbose)
{

  char format_string[128];
  int elimination_variables;
  int weighted_variables;
  char elimination_refinement[128];
  char weighted_refinement[128];
  int new_variables;
  char algorithm[128];
  long old_size;

  ifstream old(GROEBNER);
  ifstream _new(NEW_COST);

  // verify existence of files

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "cannot read from first input file, possibly not found"<<endl;
    return 0;
  }

  if(!_new)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "cannot read from second input file, possibly not found"<<endl;
    return 0;
  }


// read first part of GROEBNER file (until term ordering reached)

  // read format specification

  old>>format_string;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading format specification of first input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"GROEBNER"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;
  }

  // read algorithm

  old>>format_string;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading algorithm from first input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"computed"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;
  }

  old>>format_string;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading algorithm from first input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"with"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;
  }

  old>>format_string;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading algorithm from first input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"algorithm:"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;
  }

  old>>algorithm;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading algorithm from first input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(algorithm,"ct") && strcmp(algorithm,"pct") &&
     strcmp(algorithm,"ect") && strcmp(algorithm,"pt") &&
     strcmp(algorithm,"hs") && strcmp(algorithm,"du")
     && strcmp(algorithm,"blr"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "first input file computed with unknown algorithm"<<endl;
  }

  // override optional lines

  do
  {
    old>>format_string;

    if(!old)
    {
      cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
        "input failure before reading term ordering from first \n"
        "input file, input format not accepted"<<endl;
      return 0;
    }
  }
  while(strcmp(format_string,"term"));

  // read term ordering

  old>>format_string;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading term ordering \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"ordering:"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;
  }

  old>>format_string;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"elimination"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;
  }

  old>>format_string;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"block"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;
  }

  old>>elimination_variables;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(elimination_variables<0)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "number of elimination variables in first input file must be "
      "nonnegative"<<endl;
    return 0;
  }

  if(elimination_variables>0)
  {
    old>>elimination_refinement;

    if(!old)
    {
      cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
        "input failure when reading term ordering \n"
        "from first input file, input format not accepted"<<endl;
      return 0;
    }

    if(strcmp(elimination_refinement,"LEX") &&
       strcmp(elimination_refinement,"DEG_LEX") &&
       strcmp(elimination_refinement,"DEG_REV_LEX"))
    {
      cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
        "unknown elimination ordering in first input file"<<endl;
      return 0;
    }
  }

  old>>format_string;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"weighted"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;
  }

  old>>format_string;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"block"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;
  }

  old>>weighted_variables;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(weighted_variables<=0)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "number of weighted variables in first input file must be "
      "positive"<<endl;
    return 0;
  }

  old>>weighted_refinement;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading term ordering \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(weighted_refinement,"W_LEX") &&
     strcmp(weighted_refinement,"W_REV_LEX") &&
     strcmp(weighted_refinement,"W_DEG_LEX") &&
     strcmp(weighted_refinement,"W_DEG_REV_LEX"))
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "unknown weighted ordering in first input file"<<endl;
    return 0;
  }

// read NEW_COST file

  // read format specification

  _new>>format_string;

  if(!_new)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading format specification\n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"MATRIX") && strcmp(format_string,"NEW_COST"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;
  }

  // read number of variables

  _new>>format_string;

  if(!_new)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading number of variables \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"columns:") && strcmp(format_string,"variables:"))
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  _new>>new_variables;

  if(!_new)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading number of variables \n"
      "from second input file, input format not accepted"<<endl;
    return 0;
  }

  if(new_variables<=0)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "number of variables in second input file must be positive"<<endl;
    return 0;
  }

  // Now we can verify consistency of both files with respect to the number
  // of weighted variables:

  if(weighted_variables!=new_variables)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "number of variables in first and second input file do not match"<<endl;
    return 0;
  }

  // read term ordering

  _new>>format_string;

  if(!_new)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading new cost vector from second input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"cost"))
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  _new>>format_string;

  if(!_new)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading new cost vector from second input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"vector:"))
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "second input file has suspicious format"<<endl;

  // the term ordering to refine the weight is taken to be the same as that
  // for the old Groebner basis
  int weighted_ref;
  if(!strcmp(weighted_refinement,"W_LEX"))
    weighted_ref=W_LEX;
  if(!strcmp(weighted_refinement,"W_REV_LEX"))
    weighted_ref=W_REV_LEX;
  if(!strcmp(weighted_refinement,"W_DEG_LEX"))
    weighted_ref=W_DEG_LEX;
  if(!strcmp(weighted_refinement,"W_DEG_REV_LEX"))
    weighted_ref=W_DEG_REV_LEX;

  term_ordering w(weighted_variables,_new,weighted_ref);

  if(w.error_status()<0)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading new cost vector from second input file,\n"
      "input format not accepted"<<endl;
    return 0;
  }

  if(w.is_nonnegative()==FALSE)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "cost vectors with negative components are not supported"<<endl;
    return 0;
  }

  if(elimination_variables>0)
  {
    int elimination_ref;
    if(!strcmp(elimination_refinement,"LEX"))
      elimination_ref=LEX;
    if(!strcmp(elimination_refinement,"DEG_LEX"))
      elimination_ref=DEG_LEX;
    if(!strcmp(elimination_refinement,"DEG_REV_LEX"))
      elimination_ref=DEG_REV_LEX;
    w.convert_to_elimination_ordering(elimination_variables,elimination_ref);

    if(w.error_status()<0)
    {
      cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
        "input failure when reading new cost vector from second input file,\n"
        "input format not accepted"<<endl;
      return 0;
    }
  }

// read second part of the GROEBNER file

  // override old weight vector

  do
  {
    old>>format_string;

    if(!old)
    {
      cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
        "input failure before reading number of ideal generators \n"
        "from first input file, input format not accepted"<<endl;
      return 0;
    }
  }
  while(strcmp(format_string,"size:"));

  // read old Groebner basis

  old>>old_size;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading number of ideal generators \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(old_size<=0)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "ideal in first input file is corrupt"<<endl;
    return 0;
  }

  old>>format_string;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading ideal generators \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"Groebner"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;
  }

  old>>format_string;

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure before reading ideal generators \n"
      "from first input file, input format not accepted"<<endl;
    return 0;
  }

  if(strcmp(format_string,"basis:"))
  {
    cerr<<"WARNING: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "first input file has suspicious format"<<endl;
  }

  // read ideal generators from first input file, already with respect to
  // the new term ordering
  ideal I(old,w,old_size);

  if(!old)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "input failure when reading ideal generators from first input file,\n"
      "input format not accepted."<<endl;
    return 0;
  }

  if(I.error_status()<0)
  {
    cerr<<"ERROR: int change_cost(INPUT_FILE, INPUT_FILE):\n"
      "cannot compute with corrupt ideal\n"<<endl;
    return 0;
  }


///////////////////////// computation ////////////////////////////////////////

  // prepare time measurement
  clock_t start, end;

  // compute new Groebner basis
  start=clock();
  I.reduced_Groebner_basis(version,S_pair_criteria,interred_percentage);
  end=clock();

  // time measurement
  float elapsed=((float) (end-start))/CLOCKS_PER_SEC;


///////////////////////// output ////////////////////////////////////////////

  // create output file

  char NEW_GROEBNER[128];

  int i=0;
  while(NEW_COST[i]!='\0' && NEW_COST[i]!='.')
  {
    NEW_GROEBNER[i]=NEW_COST[i];
    i++;
  }
  NEW_GROEBNER[i]='\0';
  strcat(NEW_GROEBNER,".GB.");
  strcat(NEW_GROEBNER,algorithm);

  ofstream output(NEW_GROEBNER);

  // format output file

  output.flags(output.flags()|ios::fixed);
  // output of fixed point numbers

  output<<"GROEBNER"<<endl<<endl;

  output<<"computed with algorithm:"<<endl;
  output<<algorithm<<endl;
  output<<"from file(s):"<<endl;
  output<<GROEBNER<<", "<<NEW_COST<<endl;
  output<<"computation time"<<endl;
  output<<setw(6)<<setprecision(2)<<elapsed<<" sec"<<endl<<endl;

  output<<"term ordering:"<<endl;
  output<<"elimination block"<<endl;
  // elimination variables
  output<<elimination_variables<<endl;
  if(elimination_variables>0)
    output<<elimination_refinement<<endl;
  output<<"weighted block"<<endl;
  // weighted variables (>0)
  output<<weighted_variables<<endl;
  output<<weighted_refinement<<endl;
  w.format_print_weight_vector(output);

  output<<"size:"<<endl;
  output<<I.number_of_generators()<<endl<<endl;

  output<<"Groebner basis:"<<endl;
  I.format_print(output);
  output<<endl;

  if(verbose==TRUE)
  {
    output<<"settings for the Buchberger algorithm:"<<endl;

    output<<"version:"<<endl;
    if(version==0)
      output<<"1a"<<endl;
    else
      output<<version<<endl;

    output<<"S-pair criteria:"<<endl;
    if(S_pair_criteria & REL_PRIMENESS)
      output<<"relatively prime leading terms"<<endl;
    if(S_pair_criteria & M_CRITERION)
      output<<"criterion M"<<endl;
    if(S_pair_criteria & F_CRITERION)
      output<<"criterion F"<<endl;
    if(S_pair_criteria & B_CRITERION)
      output<<"criterion B"<<endl;
    if(S_pair_criteria & SECOND_CRITERION)
      output<<"second criterion"<<endl;
    output<<endl;

    print_flags(output);
    output<<endl;
  }

  return 1;
}

#endif // IP_ALGORITHMS_CC
