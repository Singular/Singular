#include <stdio.h>
#include <Python.h>
#include "mlpredict.h"

int main(int argc, char *argv[])
{
	int i = 0;
	if (argc != 1) {
		printf("Usage: %s\n", argv[0]);
		return 1;
	}

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: %d\n", i);
	Py_Initialize();

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: %d\n", i);

	i = ml_initialise();
	printf("Returnvalue for ml_initialise: %d\n", i);

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: %d\n", i);

	i = ml_finalise();
	printf("Returnvalue for ml_finalise: %d\n", i);

	return 0;
}
