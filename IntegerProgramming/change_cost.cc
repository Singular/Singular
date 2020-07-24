// change_cost.cc

// This file provides an interface for changing the cost vector in IP-problems
// and for recomputing Groebner basis of toric ideals.

#ifndef CHANGE_COST_CC
#define CHANGE_COST_CC

#include "IP_algorithms.h"

int main(int argc, char *argv[])
{

  // initialize arguments for the IP-algorithms (Buchberger)
  // with default settings
  BOOLEAN verbose=FALSE;
  short version=1;
  short S_pair_criteria=11;
  double interreduction_percentage=12.0;


///////////////////////// parse options /////////////////////////////////////


  if(argc>=3)
    // argc has to be at least 3 (except when help is required, see below):
    // program name, GROEBNER file, NEW_COST file
  {

    for(short i=1;i<argc-2;i++)
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
            <<"Type `change_cost --help' for help on options."<<endl;
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
              <<"Type `change_cost --help' for help on options."<<endl;
          return 1;
        }

        continue;
      }

      // When reaching this line, the argument is no legal option.
      cerr<<"ERROR: invalid option "<<argv[i]<<endl
          <<"Type `change_cost --help' for help on options."<<endl;
      return 1;
    }


///////////////////// call IP-algorithms ////////////////////////////////////

    // open first input stream (GROEBNER file)
    ifstream first_input(argv[argc-2]);

    if(!first_input)
    {
      cerr<<"ERROR: cannot open first input file, possibly not found"<<endl;
      return 1;
    }

    // open second input stream (NEW_COST file)
    ifstream second_input(argv[argc-1]);

    if(!second_input)
    {
      cerr<<"ERROR: cannot open second input file, possibly not found"<<endl;
      return 1;
    }

    // change term ordering
    int success=change_cost(argv[argc-2], argv[argc-1], version,
                            S_pair_criteria, interreduction_percentage,
                            verbose);
    if(!success)
    {
      cerr<<"ERROR: could not change cost vector,\n"
        "no new GROEBNER file created"<<endl;
      return 1;
    }

    // Groebner basis successfully changed
    return 0;
  }

  // handle the case of "missing" arguments

    // invalid arguments
    cerr<<"USAGE: change_cost [options] groebner_file new_cost_file"<<endl
        <<"Type `change_cost --help' for help on options."<<endl;

  return 1;

}
#endif  // CHANGE_COST_CC
