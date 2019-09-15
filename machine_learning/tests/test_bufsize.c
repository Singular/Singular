#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../mlpredict.h"

int main(int argc, char *argv[])
{
	int i = 0;
	int k = 0;
	int length[5];
	char *buffer[5];
	if (argc != 1) {
		printf("Usage: %s\n", argv[0]);
		return 1;
	}

	i = ml_initialise();
	if (i != 1)				return 1;

	i = system("echo \"ring\" > test.txt");
	if (i != 0)				return 1;

	i = ml_make_prediction("test.txt", buffer, length);
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	for (k = 0; k < 5; k++) {
		printf("String %d in buffer: \t\t%s\n", k, buffer[k]);
	}
	if (i != 1)				return 1;
	for (k = 0; k < 5; k++) {
		if ((int)strlen(buffer[k]) != length[k])
			return 1;
	}
	if (strcmp(buffer[0], "sing_29.htm"))	return 1;
	if (length[0] != 11)			return 1;

	i = ml_finalise();
	if (i != 1)				return 1;

	return 0;
}
