#include <stdio.h>
#include "mlpredict.h"

int main(int argc, char *argv[])
{
	int i = 0;
	if (argc != 1) {
		printf("Usage: %s\n", argv[0]);
		return 1;
	}
	i = ml_initialise();
	printf("Returnvalue for initialise: %d\n", i);

	i = ml_finalise();
	printf("Returnvalue for finalise: %d\n", i);

	return 0;
}
