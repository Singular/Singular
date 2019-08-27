#include <stdio.h>
#include <stdlib.h>
#include <Python.h>
#include "../mlpredict.h"

/* this is found as an internal function in mlpredict.c, for use in testing */
ml_internal *_get_internals();

int test_ml_is_initialised();
void remove_dictionary();
void remove_vectors();
void remove_file_list();

void remove_dictionary()
{
	ml_internal *internal_obs = NULL;
	/* get internal datastructures */
	internal_obs = _get_internals();

	printf("Removing pDictionary\n");
	Py_DECREF(internal_obs->pDictionary);
	internal_obs->pDictionary = NULL;

	return;
}

void remove_vectors()
{
	ml_internal *internal_obs = NULL;
	/* get internal datastructures */
	internal_obs = _get_internals();

	printf("Removing pVectors\n");
	Py_DECREF(internal_obs->pVectors);
	internal_obs->pVectors = NULL;
}

void remove_file_list()
{
	ml_internal *internal_obs = NULL;
	/* get internal datastructures */
	internal_obs = _get_internals();

	printf("Removing pFile_list\n");
	Py_DECREF(internal_obs->pFile_list);
	internal_obs->pFile_list = NULL;
}

int test_ml_is_initialised()
{
	int i;

	ml_initialise();

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 1)	return 1;

	remove_dictionary();

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 0)	return 1;

	i = ml_initialise();
	printf("Returnvalue for ml_initialise: \t\t%d\n", i);
	if (i != 1)	return 1;

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 1)	return 1;

	remove_vectors();

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 0)	return 1;

	i = ml_initialise();
	printf("Returnvalue for ml_initialise: \t\t%d\n", i);
	if (i != 1)	return 1;

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 1)	return 1;

	remove_file_list();

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 0)	return 1;

	i = ml_initialise();
	printf("Returnvalue for ml_initialise: \t\t%d\n", i);
	if (i != 1)	return 1;

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 1)	return 1;

	return 0;
}


int main(int argc, char *argv[])
{
	int i = 0;

	if (argc != 1) {
		printf("Usage: %s\n", argv[0]);
		return 1;
	}

	/* initially should not be initialised */
	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 0)	return 1;

	/* check that python script says it's not initialised */
	Py_Initialize();
	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 0)	return 1;

	i = ml_initialise();
	printf("Returnvalue for ml_initialise: \t\t%d\n", i);
	if (i != 1)	return 1;

	/* Should now be initialised */
	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 1)	return 1;

	if (test_ml_is_initialised())	return 1;

	i = ml_finalise();
	printf("Returnvalue for ml_finalise: \t%d\n", i);
	if (i != 1)	return 1;

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 0)	return 1;

	return 0;
}
