#include <stdio.h>

#include "kernel/mod2.h"
#if defined(HAVE_READLINE) && defined(HAVE_READLINE_READLINE_H)
#include <readline/history.h>
#include "Singular/ipid.h"
#include "Singular/mod_lib.h"
#include "kernel/oswrapper/feread.h"

#ifdef HAVE_PYTHON

#include "mlpredict.h"

static BOOLEAN predictHelp(leftv result, leftv arg)
{
        char *buffer[5];
        int lengths[5];
        char *filename = getenv("SINGULARHIST");
        if (filename==NULL) filename=SINGULARHIST_FILE;
        int i;
        lists L = (lists)omAllocBin(slists_bin);

        buffer[0] = NULL;
        buffer[1] = NULL;
        buffer[2] = NULL;
        buffer[3] = NULL;
        buffer[4] = NULL;

        if (write_history(filename))
        {
                printf("Failed to write history\n");
                return TRUE;
        }

        if (!ml_initialise())
        {
                WerrorS("Initialise of ml failed.");
                /* Notify singular that an error occured */
                return TRUE;
        }
        #ifdef HAVE_OMALLOC
        ml_make_prediction(filename, buffer, lengths, _omStrDup);
        #else /*xalloc*/
        ml_make_prediction(filename, buffer, lengths, omStrDup);
        #endif

        L->Init(5);

        for (i = 0; i < 5; i++) {
                //printf("prediction %d: %s\n", i, buffer[i]);
                L->m[i].rtyp = STRING_CMD;
                L->m[i].data = buffer[i];
        }

        // pass the resultant list to the res datastructure
        result->rtyp=LIST_CMD;
        result->data=(void *)L;

        ml_finalise();
        return FALSE;

        //result->rtyp=NONE; // set the result type
}

// initialisation of the module
extern "C" int SI_MOD_INIT(machinelearning)(SModulFunctions* psModulFunctions)
{
        char *filename = getenv("SINGULARHIST");
        if (filename==NULL)
        {
          WarnS("SINGULARHIST is not set - no history available, using ~/.singularhist");
        }
        // this is the initialization routine of the module
        // adding the routine predictHelp:
        psModulFunctions->iiAddCproc(
                        (currPack->libname? currPack->libname: ""),
                        "predictHelp",// for the singular interpreter
                        FALSE, // should enter the global name space
                        predictHelp); // the C/C++ routine
        return MAX_TOK;
}
#endif
#endif
