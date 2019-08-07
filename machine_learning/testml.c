#include <stdio.h>
#include <Python.h>
#include "mlpredict.h"

int main(int argc, char *argv[])
{
	int i = 0;
	int length = 0;
	char buffer10[10];
	char buffer11[11];
	char buffer12[12];
	char buffer20[20];
	if (argc != 1) {
		printf("Usage: %s\n", argv[0]);
		return 1;
	}

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	Py_Initialize();

	i = ml_initialise();
	printf("Returnvalue for ml_initialise: \t\t%d\n", i);

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);

	i = ml_make_prediction("test.txt", 10, buffer10, &length);
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	printf("String for buffer size 10: \t%s\n", buffer10);

	i = ml_make_prediction("test.txt", 11, buffer11, &length);
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	printf("String for buffer size 11: \t%s\n", buffer11);

	i = ml_make_prediction("test.txt", 12, buffer12, &length);
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	printf("String for buffer size 12: \t%s\n", buffer12);

	i = ml_make_prediction("test.txt", 20, buffer20, &length);
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	printf("String for buffer size 20: \t%s\n", buffer20);

	i = ml_finalise();
	printf("Returnvalue for ml_finalise: \t\t%d\n", i);

	return 0;
}
