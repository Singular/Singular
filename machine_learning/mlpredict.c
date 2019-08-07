/**
 * @file   mlpredict.c
 * @brief  Functions for using python to do machine learning in Singular
 *
 * @author Murray Heymann
 * @date   August 2019
 */
#include <stdio.h>
#include <Python.h>

#include "mlpredict.h"

PyObject *call_python_function(char *module, char *func);

/**
 * Check whether the helpfiles have been downloaded and the relevant
 * vectors have been calculated and saved.
 *
 * @return An integer:  1 if it has been intialised, 0 otherwise
 */
int ml_is_initialised()
{
	int retvalue = 1;
	int t_value = 0;
	PyObject *pValue = NULL;

	if (!Py_IsInitialized()) {
		retvalue = 0;
	} else {
		/* python system is initialised */
		pValue = call_python_function(LOOKUPTABLE, IS_LOOKUP_INITIALISED);
		/* is this a boolean? */
		if (pValue != NULL && PyBool_Check(pValue)) {
			t_value = PyObject_IsTrue(pValue);
			/* errors? */
			if (t_value == -1) {
				PyErr_Print();
				retvalue = 0;
			} else {
				/* no errors */
				retvalue = t_value;
			}
			Py_DECREF(pValue);
		} else {
			retvalue = 0;
		}
	}
	return retvalue;
}


/**
 * Initialise the machine learning system by starting the python
 * interpreter,  downloading the helpfiles if
 * not present, and calculating the bag of words vectors for the helpfiles,
 * saving this info on the local system.
 *
 * @return An integer: 1 if successful, 0 if some error were to occur.
 */
int ml_initialise()
{
	PyObject *pValue = NULL;

	if (!Py_IsInitialized()) {
		Py_Initialize();
	}
	pValue = call_python_function(LOOKUPTABLE, INIT_TABLE_ON_SYSTEM);
	if (pValue != NULL) {
		Py_DECREF(pValue);
		return 1;
	} else {
		return 0;
	}

}

/**
 * A wrapper for Py_Finalize, checking whether it is necessary in the first
 * place.
 *
 * @return An integer: 1 if successful, 0 if not.
 */
int ml_finalise()
{
	int retvalue = 1;
	if (Py_IsInitialized()) {
		Py_Finalize();
		retvalue = 1;
	} else {
		retvalue = 0;
	}
	return retvalue;
}



/**
 * Local helper function to call a function that takes no arguments.
 * @param[in] module A string of the module name where the function is
 * found
 * @param[in] func A string giving the name of the function to call.
 *
 * @return the returned PyObject.
 */
PyObject *call_python_function(char *module, char *func)
{
	PyObject *pName = NULL, *pModule = NULL, *pFunc = NULL;
	PyObject *pValue = NULL;
		
	if (!Py_IsInitialized()) {
		Py_Initialize();
	}

	/* import the module */
	pName = PyString_FromString(module);
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if (pModule != NULL){
		/* Get the init function we want to call */
		pFunc = PyObject_GetAttrString(pModule, func);
		if (pFunc && PyCallable_Check(pFunc)) {
			/* Callable function. Good. In this case, doesn't take any
			 * arguments*/
			pValue = PyObject_CallObject(pFunc, NULL);
			if (pValue == NULL) {
				printf("No return for function\n");
				PyErr_Print();
			}
		} else {
			/* Somehow not executable. Clean up! */
			if(PyErr_Occurred()) {
				PyErr_Print();
			}
			fprintf(stderr,
					"Cannot find function \"%s\"\n",
					func);
		}
		Py_XDECREF(pFunc);
		Py_DECREF(pModule);
	} else {
		PyErr_Print();
		fprintf(stderr, "Failed to load \"%s\"\n", module);
	}

	return pValue;
}

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
					   int *pred_len)
{



	return 1;
}


