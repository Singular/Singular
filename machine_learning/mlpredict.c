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
	/*
	 * TODO
	 */
	return 1;
}

/**
 * Initialise the machine learning system by downloading the helpfiles if
 * not present, and calculating the bag of words vectors for the helpfiles,
 * saving this info on the local system.  
 *
 * @return An integer: 1 if successful, 0 if some error were to occur.
 */

int ml_initialise()
{
	char lookuptable[] = "common.lookuptable";
	char init_table_on_system[] = "init_table_on_system";
	PyObject *pValue = NULL;

	pValue = call_python_function(lookuptable, init_table_on_system);
	if (pValue != NULL) {
		Py_DECREF(pValue);
		return 1;
	} else {
		return 0;
	}

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
	int retValue = 1;
		
	Py_Initialize();

	/* import the module */
	pName = PyUnicode_DecodeFSDefault(module);
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
		retValue = 0;
	}
	if (Py_FinalizeEx() < 0) {
		retValue = 0;
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
	Py_Initialize();



	if (Py_FinalizeEx() < 0) {
		return 0;
	} else {
		return 1;
	}
}


