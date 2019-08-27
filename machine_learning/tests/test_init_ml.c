#include <stdio.h>
#include <stdlib.h>
#include "../mlpredict.h"

int main(int argc, char *argv[])
{
	int i = 0;

	if (argc != 1) {
		printf("Usage: %s\n", argv[0]);
		return 1;
	}

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 0)	return 1;

	i = ml_initialise();
	printf("Returnvalue for ml_initialise: \t\t%d\n", i);
	if (i != 1)	return 1;

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 1)	return 1;

	i = ml_finalise();
	printf("Returnvalue for ml_finalise: \t%d\n", i);
	if (i != 1)	return 1;

	i = ml_is_initialised();
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	if (i != 0)	return 1;

	return 0;
}
