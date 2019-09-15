/**
 * @file   mlpredict.c
 * @brief  Functions for using python to do machine learning in Singular
 *
 * @author Murray Heymann
 * @date   August 2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Python.h>

#include "mlpredict.h"

/* Locally defined macros */
#define LOOKUPTABLE		"common.lookuptable"
#define KEYWORD_VECTOR		"common.keyword_vector"
#define PRD_RUNNER		"predictor_runner"
#define IS_LOOKUP_INITIALISED	"is_lookup_initialised"
#define INIT_TABLE_ON_SYSTEM	"init_table_on_system"
#define GET_PREDICTION		"get_prediction"
#define READ_DICTIONARY		"read_dictionary"
#define CREATE_TABLE		"create_table"
#define PYTPATH(B)		sprintf(B, "%s/ml_python", DATA_PATH)
#define SING_BIN(B)		sprintf(B, "%s/Singular", BIN_PATH)
#define SING_EXT_SCRIPT(B)	sprintf(B, "%s/ml_singular/extract.lib", \
					DATA_PATH)

/**** Local Function Declarations ****************************************/

PyObject *_call_python_function(char *module, char *func);
PyObject *_call_python_function_args(char *module, char *func, PyObject *pArgs);
ml_internal *_get_internals();
int _set_dictionary();
int _set_vectors_file_list();

/**** Static Variables ***************************************************/

//static PyObject *pDictionary = NULL;
//static PyObject *pVectors = NULL;
//static PyObject *pFile_list = NULL;
static ml_internal internal_obs = {NULL,NULL,NULL};

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
	int t_value = 0;
	PyObject *pValue = NULL;

	if (!Py_IsInitialized()) return 0;

	pValue = _call_python_function(LOOKUPTABLE, IS_LOOKUP_INITIALISED);

	if (pValue == NULL)	return 0;
	if (!PyBool_Check(pValue)) {
		/* Not a boolean type */
		Py_DECREF(pValue);
		return 0;
	}

	t_value = PyObject_IsTrue(pValue);
	Py_DECREF(pValue);
	if (t_value == -1) {
		/* errors */
		PyErr_Print();
		Py_DECREF(pValue);
		return 0;
	}

	if (!t_value)		return 0;
	if (!internal_obs.pDictionary)	return 0;
	if (!internal_obs.pVectors)		return 0;
	if (!internal_obs.pFile_list)	return 0;

	return 1;
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
	char buffer[100];
	char *spath = NULL;
	PyObject *pString = NULL;
	PyObject *pValue = NULL;
	PyObject *pPath = NULL;
	PyObject *pMyPath = NULL;

	PyObject *pName = NULL;
	PyObject *pModule = NULL;
	PyObject *pArgs = NULL;
	//PyObject *pTemp = NULL;

	if (!Py_IsInitialized()) {
		Py_Initialize();
	}

	pName = PyString_FromString("sys");
	//pName = PyUnicode_FromString("sys");
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	if (pModule == NULL){
		PyErr_Print();
		fprintf(stderr, "Failed to load \"%s\"\n", "sys");
		return 0;
	}

	/* Get the path list */
	pPath = PyObject_GetAttrString(pModule, "path");

	if (!pPath) {
		fprintf(stderr, "Failed to get python path list\n");
		Py_DECREF(pModule);
		return 0;
	}
	Py_DECREF(pModule);

	/* get a string representation of the path list for comparison */
	pString = PyObject_Str(pPath);
	spath = PyString_AsString(pString);
	//pTemp = PyUnicode_AsASCIIString(pString);
	//spath = PyBytes_AsString(pTemp);
	/* get the path to be set */
	PYTPATH(buffer);
	if (!strstr(spath, buffer)) {
		pMyPath = PyString_FromString(buffer);
		//pMyPath = PyUnicode_FromString(buffer);
		/* pPath set to tuple, so no decref needed later */
		PyList_Append(pPath, pMyPath);
		Py_DECREF(pMyPath);
	}
	Py_DECREF(pString);
	//Py_XDECREF(pTemp);
	Py_DECREF(pPath);


	/* Setup arguments */
	pArgs = PyTuple_New(2);
	SING_EXT_SCRIPT(buffer);
	pString = PyString_FromString(buffer);
	PyTuple_SetItem(pArgs, 0, pString);

	SING_BIN(buffer);
	pString = PyString_FromString(buffer);
	PyTuple_SetItem(pArgs, 1, pString);

	pValue = _call_python_function_args(LOOKUPTABLE,
					    INIT_TABLE_ON_SYSTEM,
					    pArgs);
	Py_DECREF(pArgs);

	if (pValue == NULL)		return 0;
	Py_XDECREF(pValue);
	if (!_set_dictionary())		return 0;
	if (!_set_vectors_file_list())	return 0;

	return 1;
}

/**
 * Tell python to decrement the global variables, checking whether it is
 * necessary in the first place.
 *
 * @return An integer: 1 if successful, 0 if not.
 */
int ml_finalise()
{
	if (!Py_IsInitialized())
		return 0;

	Py_XDECREF(internal_obs.pDictionary);
	Py_XDECREF(internal_obs.pVectors);
	Py_XDECREF(internal_obs.pFile_list);
	internal_obs.pDictionary = NULL;
	internal_obs.pVectors = NULL;
	internal_obs.pFile_list = NULL;

	/* Note Py_Finalize should only be called when the program quits. not
	 * here.
	 */
	return 1;
}

/**
 * Take a filename as string, pass it to the machine learning system, and
 * return a helpfile name as string.
 *
 * @param[in]  filename A String indicating the for which the prediction
 * must be made
 * @param[out] prediction_buffers The buffer into which the prediction
 * filenames are copied. Must contain exactly 5 char *.
 * @param[out] pred_len A pointer to an integer, at which the string length
 * of the prediction filename is set.
 *
 * @return 1 if successful, 0 if some error occurs.
 */
int ml_make_prediction(char *filename,
		       char *prediction_buffers[],
		       int *pred_len)
{
	PyObject *pFName = NULL;
	PyObject *pArgs = NULL;
	PyObject *pValue = NULL;
	PyObject *pString = NULL;
	// PyObject *pTemp = NULL;
	int i = 0;

	pFName = PyString_FromString(filename);
	//pFName = PyUnicode_FromString(filename);
	if (!pFName) {
		fprintf(stderr, "This is weird\n");

		return 0;
	}
	pArgs = PyTuple_New(4);
	if (!pArgs) {
		fprintf(stderr, "This is also weird\n");
		Py_DECREF(pFName);
		return 0;
	}
	/* pFName is handed over to the tuple, so not DECREF later */
	PyTuple_SetItem(pArgs, 0, pFName);
	/* Since each of the following is handed over, we need to increase the
	 * reference, otherwise our static variable pointers might be freed by
	 * the python interpreter. */
	PyTuple_SetItem(pArgs, 1, internal_obs.pDictionary);
	Py_INCREF(internal_obs.pDictionary);
	PyTuple_SetItem(pArgs, 2, internal_obs.pVectors);
	Py_INCREF(internal_obs.pVectors);
	PyTuple_SetItem(pArgs, 3, internal_obs.pFile_list);
	Py_INCREF(internal_obs.pFile_list);

	pValue = _call_python_function_args(PRD_RUNNER,
					    GET_PREDICTION,
					    pArgs);
	Py_DECREF(pArgs);

	if (!pValue) {
		return 0;
	}
	if (!PyList_Check(pValue)) {
		printf("Expected a list for prediction.\n");
		Py_DECREF(pValue);
		return 0;
	}
	if (PyList_Size(pValue) != 5) {
		printf("List length is supposed to be five, but is %d.\n",
				PyList_Size(pValue));
		Py_DECREF(pValue);
		return 0;
	}
	// pString = PyObject_Str(pValue);
	for (i = 0; i < 5; i++) {
		pString = PyObject_Str(PyList_GetItem(pValue, i));
		prediction_buffers[i] = strdup(PyString_AsString(pString));
		//pTemp = PyUnicode_AsASCIIString(pString);
		//prediction_buffers[i] = strdup(PyBytes_AsString(pTemp));
		pred_len[i] = strlen(prediction_buffers[i]);
		Py_DECREF(pString);
		pString = NULL;
	}

	Py_DECREF(pValue);

	return 1;
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
PyObject *_call_python_function(char *module, char *func)
{
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
	//pName = PyUnicode_FromString(module);
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if (pModule == NULL){
		PyErr_Print();
		fprintf(stderr, "Failed to load \"%s\"\n", module);
		return NULL;
	}
	/* Get the init function we want to call */
	pFunc = PyObject_GetAttrString(pModule, func);
	if (!pFunc || !PyCallable_Check(pFunc)) {
		/* Somehow not executable. Clean up! */
		if(PyErr_Occurred()) {
			PyErr_Print();
		}
		fprintf(stderr,
				"Cannot find function \"%s\"\n",
				func);

		Py_XDECREF(pFunc);
		Py_DECREF(pModule);

		return NULL;
	}

	/* Callable function. Good. Call with the args supplied,
	 * assuming the arguments are correct for the function */
	pValue = PyObject_CallObject(pFunc, pArgs);

	Py_XDECREF(pFunc);
	Py_DECREF(pModule);

	if (pValue == NULL) {
		printf("No return for function\n");
		PyErr_Print();
		return NULL;
	}
	return pValue;
}

/**
 * This is intended to be used ONLY by the testing library.
 */
ml_internal *_get_internals()
{
	return &internal_obs;
}


/**
 * Get the PyObject of the list of keywords called dictionary and set it to
 * the local static variable.  If already set, simply return without any
 * action.
 *
 * @return 1 if successful, 0 if something goes wrong.
 */
int _set_dictionary()
{
	PyObject *pValue = NULL;

	if (internal_obs.pDictionary) {
		/* already set */
		return 1;
	}
	pValue = _call_python_function(KEYWORD_VECTOR, READ_DICTIONARY);
	if (!pValue) {
		return 0;
	}
	internal_obs.pDictionary = pValue;

	return 1;
}

/**
 * Get the PyObject of the list of vectors called corresponding to each
 * helper file and the list of helper files that could be in a prediction
 * and set it to the local static variable. If already set, simply return
 * without any action.
 *
 * @return 1 if successful, 0 if something goes wrong.
 */

int _set_vectors_file_list()
{
	PyObject *pValue = NULL;
	PyObject *pVal1 = NULL, *pVal2 = NULL;

	if (internal_obs.pVectors && internal_obs.pFile_list) {
		return 1;
	}

	/* Ensure *both* are free */
	Py_XDECREF(internal_obs.pVectors);
	Py_XDECREF(internal_obs.pFile_list);
	internal_obs.pVectors = NULL;
	internal_obs.pFile_list = NULL;

	pValue = _call_python_function(LOOKUPTABLE, CREATE_TABLE);
	if (!pValue) {
		return 0;
	}
	pVal1 = PyTuple_GetItem(pValue, 0);
	pVal2 = PyTuple_GetItem(pValue, 1);
	/* Decreasing the reference to the tuple causes the content to be freed.
	 * To prevent that (leading to a segfault), we have to manually increase
	 * the reference to the contents */
	Py_INCREF(pVal1);
	Py_INCREF(pVal2);

	Py_DECREF(pValue);
	pValue = NULL;

	if (!pVal1 || !pVal2) {
		Py_XDECREF(pVal1);
		Py_XDECREF(pVal2);
		return 0;
	}
	internal_obs.pVectors = pVal1;
	internal_obs.pFile_list = pVal2;

	return 1;
}
