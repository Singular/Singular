// toric_ideal.cc

// This file provides an interface to compute toric ideals from a matrix
// without solving IP-problems.



#ifndef TORIC_IDEAL_CC
#define TORIC_IDEAL_CC



#include "IP_algorithms.h"



int main(int argc, char *argv[])
{

  // initialize arguments for the Buchberger algorithm
  // with default settings
  BOOLEAN verbose=FALSE;
  short version=1;
  short S_pair_criteria=11;
  double interreduction_percentage=12.0;


///////////////////////// parse options /////////////////////////////////////

  short alg_option=0;
  // no algorithm specified yet

  if(argc>=2 && strcmp(argv[1],"--help"))
    // argc has to be at least 2
    // program name, MATRIX file
  {

    for(short i=1;i<argc-1;i++)
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
        cerr<<"ERROR: invalid version argument"<<endl
            <<"Type `toric_ideal --help' for help on options."<<endl;
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
          cerr<<"ERROR: invalid argument for interreduction percentage"<<endl
              <<"Type `toric_ideal --help' for help on options."<<endl;
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
        cerr<<"ERROR: unkwon algorithm"<<endl
            <<"Type `toric_ideal --help' for help on options."<<endl;
        return 1;
      }

      // When reaching this line, the argument is no legal option.
      cerr<<"ERROR: invalid option "<<argv[i]<<endl
          <<"Type `toric_ideal --help' for help on options."<<endl;
      return 1;
    }


////////////// call algorithms for computing toric ideals /////////////////////

    // open input stream (MATRIX file)
    ifstream input(argv[argc-1]);

    if(!input)
    {
      cerr<<"ERROR: cannot open input file, possibly not found"<<endl;
      return 1;
    }

    if(alg_option==0)
    {
      cerr<<"ERROR: no valid algorithm specified"<<endl;
      return 1;
    }

    // perform specified algorithm to compute a Groebner basis of the
    // toric ideal defined by the input matrix

    int success;
    if(!strcmp(argv[alg_option],"ct"))
      success=Conti_Traverso(argv[argc-1], version, S_pair_criteria,
                             interreduction_percentage, verbose);
    if(!strcmp(argv[alg_option],"pct"))
      success=Positive_Conti_Traverso(argv[argc-1], version,
                                      S_pair_criteria,
                                      interreduction_percentage, verbose);
    if(!strcmp(argv[alg_option],"ect"))
      success=Elim_Conti_Traverso(argv[argc-1], version, S_pair_criteria,
                                  interreduction_percentage, verbose);
    if(!strcmp(argv[alg_option],"pt"))
      success=Pottier(argv[argc-1], version, S_pair_criteria,
                      interreduction_percentage, verbose);
    if(!strcmp(argv[alg_option],"hs"))
      success=Hosten_Sturmfels(argv[argc-1], version, S_pair_criteria,
                               interreduction_percentage, verbose);
    if(!strcmp(argv[alg_option],"du"))
      success=DiBiase_Urbanke(argv[argc-1], version, S_pair_criteria,
                              interreduction_percentage, verbose);
    if(!strcmp(argv[alg_option],"blr"))
      success=Bigatti_LaScala_Robbiano(argv[argc-1], version,
                                       S_pair_criteria,
                                       interreduction_percentage, verbose);
    if(!success)
    {
      cerr<<"ERROR: could not perform required IP-algorithm,\n"
        "no GROEBNER file created"<<endl;
      return 1;
    }

    // Groebner basis successfully computed
    return 0;
  }


/////////////////////// provide help text ///////////////////////////////////

  // handle the case of "missing" arguments

  if((argc==2) && !strcmp(argv[1],"--help"))
    // help required
  {
    system("less toric_ideal.hlp");
    return 0;
  }

  else
    // invalid arguments
    cerr<<"USAGE: toric_ideal [options] matrix_file"<<endl
        <<"Type `toric_ideal --help' for help on options."<<endl;

  return 1;

}
#endif  // TORIC_IDEAL_CC
