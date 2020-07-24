// solve_IP.cc

// This file provides the interface to call the implemented IP-algorithms.
// This includes a short help text on the choice of algorithms and their
// arguments and the required formats of the input file.

#ifndef SOLVE_IP_CC
#define SOLVE_IP_CC

#include "IP_algorithms.h"

int main(int argc, char *argv[])
{

  // initialize arguments for the IP-algorithms (Buchberger)
  // with default settings
  BOOLEAN verbose=FALSE;
  int version=1;
  int S_pair_criteria=11;
  double interreduction_percentage=12.0;

///////////////////////// parse options /////////////////////////////////////

  int alg_option=0;
  // no algorithm specified yet

  if(argc>=3)
    // argc has to be at least 3 (except when help is required, see below):
    // program name, (MATRIX or) GROEBNER file, PROBLEM file
  {

    for(int i=1;i<argc-2;i++)
      // these are the input options
    {

      if(!strcmp(argv[i],"-v") || !strcmp(argv[i],"--verbose"))
      {
        verbose=TRUE;
        continue;
        // read next argument
      }

      if(!strcmp(argv[i],"-V") || !strcmp(argv[i],"-version"))
      {
        // check if next argument is a valid version number
        i++;

        if(!strcmp(argv[i],"1"))
        {
          version=1;
          continue;
        }
        if(!strcmp(argv[i],"1a"))
        {
          version=0;
          continue;
        }
        if(!strcmp(argv[i],"2"))
        {
          version=2;
          continue;
        }
        if(!strcmp(argv[i],"3"))
        {
          version=3;
          continue;
        }

        // When reaching this line, the version argument was invalid.
        cerr<<"ERROR: invalid version argument\n"
            <<"Type `solve_IP --help' for help on options.\n";
        return 1;
      }

      if(!strcmp(argv[i],"-S"))
      {
        // initialize argument to the use of no criteria
        S_pair_criteria=0;

        do
        {
          // Check what S-pair criterion the next argument indicates.
          // If none: The algorithm is called without S-pair criteria.
          i++;

          if(i>=argc-2)
            // file arguments reached
            break;

          if(!strcmp(argv[i],"RP"))
          {
            S_pair_criteria|=1;
            continue;
          }
          if(!strcmp(argv[i],"M"))
          {
            S_pair_criteria|=2;
            continue;
          }
          if(!strcmp(argv[i],"F"))
          {
            S_pair_criteria|=4;
            continue;
          }
          if(!strcmp(argv[i],"B"))
          {
            S_pair_criteria|=8;
            continue;
          }
          if(!strcmp(argv[i],"2"))
          {
            S_pair_criteria|=16;
            continue;
          }

          // option does not belong to an S-pair criterion
          break;

        }
        while(1);

        i--;        // reset counter so that the outer loop
        continue;   // continues with the just considered argument
      }

      if(!strcmp(argv[i],"-p"))
      {
        // read interreduction percentage
        i++;

        char** rest;
        interreduction_percentage=strtod(argv[i],rest);
        // The function strtod converts a string into a double. As the
        // specification is not correct, it may cause trouble...
        // For example, if the argument can be read as a float, the value
        // of rest should be NULL. This is not the case. Therefore, we can
        // only check by the following string comparison if a float has been
        // built. An argument as 15xy is thereby considered as valid
        // (with interreduction_percentage==15.0).

        if(!(strcmp(argv[i],*rest)))
          // argument was no float
        {
          cerr<<"ERROR: invalid argument for interreduction percentage\n"
              <<"Type `solve_IP --help' for help on options.\n";
          return 1;
        }

        continue;
      }

      if(!strcmp(argv[i],"-alg") || !strcmp(argv[i],"--algorithm"))
      {
        // check if next argument is a valid algorithm specification
        i++;

        if(!strcmp(argv[i],"ct") || !strcmp(argv[i],"pct") ||
           !strcmp(argv[i],"ect") || !strcmp(argv[i],"pt") ||
           !strcmp(argv[i],"hs") || !strcmp(argv[i],"du") ||
           !strcmp(argv[i],"blr"))
        {
          alg_option=i;
          continue;
        }

        // When reaching this line, the algorithm argument was invalid.
        cerr<<"ERROR: unkwon algorithm\n"
            <<"Type `solve_IP --help' for help on options.\n";
        return 1;
      }

      // When reaching this line, the argument is no legal option.
      cerr<<"ERROR: invalid option "<<argv[i]
          <<"\nType `solve_IP --help' for help on options.\n";
      return 1;
    }


///////////////////// call IP-algorithms ////////////////////////////////////

    // open first input stream (MATRIX or GROEBNER file)
    ifstream first_input(argv[argc-2]);

    if(!first_input)
    {
      cerr<<"ERROR: cannot open first input file, possibly not found\n";
      return 1;
    }

    // open second input stream (PROBLEM file)
    ifstream second_input(argv[argc-1]);

    if(!second_input)
    {
      cerr<<"ERROR: cannot open second input file, possibly not found\n";
      return 1;
    }

    // check if the first input file is a MATRIX file; in that case,
    // we have to compute a Groebner basis first
    char format_string[128];
    first_input>>format_string;

    if(!strcmp(format_string,"MATRIX"))
    {
      if(alg_option==0)
      {
        cerr<<"ERROR: first input file is a MATRIX file;\n"
          "algorithm has to be specified\n";
        return 1;
      }

      // perform specified algorithm to compute a Groebner basis of the
      // toric ideal defined by the input matrix

      int success;
      if(!strcmp(argv[alg_option],"ct"))
        success=Conti_Traverso(argv[argc-2], version, S_pair_criteria,
                               interreduction_percentage, verbose);
      if(!strcmp(argv[alg_option],"pct"))
        success=Positive_Conti_Traverso(argv[argc-2], version,
                                        S_pair_criteria,
                                        interreduction_percentage, verbose);
      if(!strcmp(argv[alg_option],"ect"))
        success=Elim_Conti_Traverso(argv[argc-2], version, S_pair_criteria,
                                    interreduction_percentage, verbose);
      if(!strcmp(argv[alg_option],"pt"))
        success=Pottier(argv[argc-2], version, S_pair_criteria,
                        interreduction_percentage, verbose);
      if(!strcmp(argv[alg_option],"hs"))
        success=Hosten_Sturmfels(argv[argc-2], version, S_pair_criteria,
                                 interreduction_percentage, verbose);
      if(!strcmp(argv[alg_option],"du"))
        success=DiBiase_Urbanke(argv[argc-2], version, S_pair_criteria,
                                interreduction_percentage, verbose);
      if(!strcmp(argv[alg_option],"blr"))
        success=Bigatti_LaScala_Robbiano(argv[argc-2], version,
                                         S_pair_criteria,
                                         interreduction_percentage, verbose);
      if(!success)
      {
        cerr<<"ERROR: could not perform required IP-algorithm,\n"
          "no GROEBNER file created\n";
        return 1;
      }

      // Groebner basis successfully computed
      // now solve problems

      char* GROEBNER=new char[128];
      memset(GROEBNER,0,128);
      int i=0;
      while(argv[argc-2][i]!='\0' && argv[argc-2][i]!='.' && argv[argc-2][i]>' ')
      {
        GROEBNER[i]=argv[argc-2][i];
        i++;
      }
      strcat(GROEBNER,".GB.");
      strcat(GROEBNER,argv[alg_option]);

      success=solve(argv[argc-1],GROEBNER);
      delete[] GROEBNER;
      return(!success);
      // ! because 0 indicates in general a regular program termination
    }

    else
      // first input file is no MATRIX file, GROEBNER file assumed
    {
      if(argc>3)
        // input options are ignored when program is called with a
        // GROEBNER file
        cerr<<"WARNING: program called with a GROEBNER file,\n"
          "specified options will not have any effect\n";
          return(!solve(argv[argc-1],argv[argc-2]));
    }
  }

  // handle the case of "missing" arguments
    // invalid arguments
    cerr<<"USAGE: solve_IP [options] toric_file problem_file\n"
        <<"Type `solve_IP --help' for help on options.\n";

  return 1;

}
#endif  // SOLVE_IP_CC
