/**
 * @file   mlpredict.h
 * @brief  Function definitions for using python to do machine learning in Singular
 *
 * @author Murray Heymann
 * @date   August 2019
 */
#ifndef MLPREDICT_H
#define MLPREDICT_H
extern "C" {

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
 * Finalize the python interpreter
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
 * @param[in]  buffer_size The maximum length of the prediction string.
 * @param[out] prediction_buffer The buffer into which the prediction
 * filename is copied.  
 * @param[out] pred_len A pointer to an integer, at which the string length
 * of the prediction filename is set. 
 *
 * @return 1 if successful, 0 if some error occurs.
 */
int ml_make_prediction(char *filename,
					   int buffer_size, 
					   char *prediction_buffer,
					   int *pred_len);

}
#endif
