/**
 * @file   mlpredict.c
 * @brief  Functions for using python to do machine learning in Singular
 *
 * @author Murray Heymann
 * @date   August 2019
 */
#include <stdio.h>
#include <Python.h>

/* Locally defined macros */
#define LOOKUPTABLE "common.lookuptable"
#define KEYWORD_VECTOR "common.keyword_vector"
#define PRD_RUNNER "predictor_runner"
#define IS_LOOKUP_INITIALISED "is_lookup_initialised"
#define INIT_TABLE_ON_SYSTEM "init_table_on_system"
#define GET_PREDICTION "get_prediction"
#define READ_DICTIONARY "read_dictionary"
#define CREATE_TABLE "create_table"

/**** Local Function Decleartions ****************************************/

PyObject *_call_python_function(char *module, char *func);
PyObject *_call_python_function_args(char *module, char *func, PyObject *pArgs);
int _get_dictionary();
int _get_vectors_file_list();

/**** Static Variables ***************************************************/

static PyObject *pDictionary = NULL;
static PyObject *pVectors = NULL;
static PyObject *pFile_list = NULL;

/**** Public Functions ***************************************************/

/**
 * Check whether the helpfiles have been downloaded and the relevant
 * vectors have been calculated and saved. Furthermore, the local static
 * variables must have been set with pointers to the relevant data
 * structures in the python instance. 
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
		pValue = _call_python_function(LOOKUPTABLE, IS_LOOKUP_INITIALISED);
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

	retvalue = retvalue && pDictionary;
	retvalue = retvalue && pVectors;
	retvalue = retvalue && pFile_list;
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
	int retvalue = 1;
	PyObject *pValue = NULL;

	if (!Py_IsInitialized()) {
		Py_Initialize();
	}
	pValue = _call_python_function(LOOKUPTABLE, INIT_TABLE_ON_SYSTEM);
	if (pValue != NULL) {
		Py_DECREF(pValue);
	} else {
		retvalue = 0;
	}

	retvalue = retvalue && _get_dictionary();
	retvalue = retvalue && _get_vectors_file_list();

	return retvalue;
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

	Py_XDECREF(pDictionary);
	Py_XDECREF(pVectors);
	Py_XDECREF(pFile_list);

	if (Py_IsInitialized()) {
		Py_Finalize();
		retvalue = 1;
	} else {
		retvalue = 0;
	}

	return retvalue;
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
	PyObject *pFName = NULL, *pArgs = NULL;
	PyObject *pValue = NULL; PyObject *pString = NULL;
	int retvalue = 1;
	int ret_string_len = 0;
	
	pFName = PyString_FromString(filename);
	if (!pFName) {
		fprintf(stderr, "This is weird\n");

		retvalue = 0;
	} else {
		pArgs = PyTuple_New(4);
		if (!pArgs) {
			fprintf(stderr, "This is also weird\n");
			retvalue = 0;
			Py_DECREF(pFName);
		} else {
			/* pFName is handed over to the tuple, so not DECREF later */
			PyTuple_SetItem(pArgs, 0, pFName);
			/* Since each of the following is handed over, we need to
			 * increase the reference, otherwise our static variable
			 * pointers might be freed by the python interpreter. */
			PyTuple_SetItem(pArgs, 1, pDictionary);
			Py_INCREF(pDictionary);
			PyTuple_SetItem(pArgs, 2, pVectors);
			Py_INCREF(pVectors);
			PyTuple_SetItem(pArgs, 3, pFile_list);
			Py_INCREF(pFile_list);
		}

	}
	if (pArgs) {
		pValue = _call_python_function_args(PRD_RUNNER,
											GET_PREDICTION,
											pArgs);
		Py_DECREF(pArgs);
		if (!pValue) {
			retvalue = 0;
		} else {
			pString = PyObject_Str(pValue);
			strncpy(prediction_buffer,
					PyString_AsString(pString),
					buffer_size - 1);
			ret_string_len = strlen(PyString_AsString(pString));
			if (ret_string_len >= buffer_size - 1) {
				prediction_buffer[buffer_size - 1] = '\0';
				*pred_len = buffer_size - 1;
			} else {
				*pred_len = ret_string_len;
			}

			Py_DECREF(pString);
			Py_DECREF(pValue);
		}
	}

	return retvalue;
}

/**** Local Functions ****************************************************/

/**
 * Local helper function to call a function that takes no arguments.
 * @param[in] module A string of the module name where the function is
 * found
 * @param[in] func A string giving the name of the function to call.
 *
 * @return the returned PyObject.
 */
PyObject *_call_python_function(char *module, char *func) {
	PyObject *pArgs = PyTuple_New(0);
	PyObject *retvalue = _call_python_function_args(module, func, pArgs);

	Py_DECREF(pArgs);
	return retvalue;
}


/**
 * Local helper function to call a function that takes arguments.
 * @param[in] module A string of the module name where the function is
 * found
 * @param[in] func A string giving the name of the function to call.
 * @param[in] pArgs The arguments to be parsed to the funcion being called.
 *
 * @return the returned PyObject.
 */
PyObject *_call_python_function_args(char *module, char *func, PyObject *pArgs)
{
	PyObject *pName = NULL, *pModule = NULL, *pFunc = NULL;
	PyObject *pValue = NULL;
		
	if (!Py_IsInitialized()) {
		Py_Initialize();
		printf("I don't like this\n");
	}

	/* import the module */
	pName = PyString_FromString(module);
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if (pModule != NULL){
		/* Get the init function we want to call */
		pFunc = PyObject_GetAttrString(pModule, func);
		if (pFunc && PyCallable_Check(pFunc)) {
			/* Callable function. Good. Call with the args supplied,
			 * assuming the arguments are correct for the function */
			pValue = PyObject_CallObject(pFunc, pArgs);
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
 * Get the PyObject of the list of keywords called dictionary and set it to
 * the local static variable.  If already set, simply return without any
 * action.
 *
 * @return 1 if successful, 0 if something goes wrong.
 */
int _get_dictionary()
{
	int retvalue = 1;
	PyObject *pValue = NULL;

	if (!pDictionary) {
		pValue = _call_python_function(KEYWORD_VECTOR, READ_DICTIONARY);
		if (!pValue) {
			retvalue = 0;
		} else {
			pDictionary = pValue;
		}
	}
	return retvalue;
}

/**
 * Get the PyObject of the list of vectors called corresponding to each
 * helper file and the list of helper files that could be in a prediction
 * and set it to the local static variable. If already set, simply return
 * without any action.
 *
 * @return 1 if successful, 0 if something goes wrong.
 */
int _get_vectors_file_list()
{
	int retvalue = 1;
	PyObject *pValue = NULL;
	PyObject *pVal1 = NULL, *pVal2 = NULL;

	if (!pVectors || !pFile_list) {
		/* Ensure *both* are free */
		Py_XDECREF(pVectors);
		Py_XDECREF(pFile_list);
		pVectors = NULL;
		pFile_list = NULL;
		pValue = _call_python_function(LOOKUPTABLE, CREATE_TABLE);
		if (!pValue) {
			retvalue = 0;
		} else {
			pVal1 = PyTuple_GetItem(pValue, 0);
			pVal2 = PyTuple_GetItem(pValue, 1);
			/* Decreasing the reference to the tuple causes the content to
			 * be freed.  To prevent that (leading to a segfault), we have
			 * to manually increase the reference to the contents */
			Py_INCREF(pVal1);
			Py_INCREF(pVal2);
			Py_DECREF(pValue);
			pValue = NULL;
			if (pVal1 && pVal2) {
				pVectors = pVal1;
				pFile_list = pVal2;
			} else {
				Py_XDECREF(pVal1);
				Py_XDECREF(pVal2);
				retvalue = 0;
			}
		}
	}
	
	return retvalue;
}
