/**
 * @file   mlpredict.h
 * @brief  Function definitions for using python to do machine learning in Singular
 *
 * @author Murray Heymann
 * @date   August 2019
 */
#ifndef MLPREDICT_H
#define MLPREDICT_H

#ifdef HAVE_PYTHON
#ifdef __cplusplus
extern "C" {
#endif /* ifdef cpp */

#include <Python.h>
typedef struct ml_internal {
	PyObject *pDictionary;
	PyObject *pVectors;
	PyObject *pFile_list;
} ml_internal;

/**
 * Check whether the helpfiles have been downloaded and the relevant
 * vectors have been calculated and saved.
 *
 * @return An integer:  1 if it has been intialised, 0 otherwise
 */
int ml_is_initialised();

/**
 * Initialise the machine learning system by starting the python
 * interpreter,  downloading the helpfiles if
 * not present, and calculating the bag of words vectors for the helpfiles,
 * saving this info on the local system.
 *
 * @return An integer: 1 if successful, 0 if some error were to occur.
 */
int ml_initialise();

/**
 * Tell python to decrement the global variables, checking whether it is
 * necessary in the first place.
 *
 * @return An integer: 1 if successful, 0 if not.
 */
int ml_finalise();

/**
 * Take a filename as string, pass it to the machine learning system, and
 * return a helpfile name as string.
 *
 * @param[in]  filename A String indicating the for which the prediction
 * must be made
 * @param[out] prediction_buffers The buffers into which the prediction
 * filename is copied.
 * @param[out] pred_len A pointer to an integer, at which the string length
 * of the prediction filename is set.
 *
 * @return 1 if successful, 0 if some error occurs.
 */
int ml_make_prediction(char *filename,
		       char *prediction_buffers[],
		       int *pred_len,
		       char *(*custom_strdup)(const char *));

#ifdef __cplusplus
}
#endif /* ifdef cpp */

#endif /* HAVE_PYTHON */
#endif
