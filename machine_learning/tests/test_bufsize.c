#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../mlpredict.h"

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

	i = ml_initialise();
	if (i != 1)				return 1;

	i = system("echo \"ring\" > test.txt");
	if (i != 0)				return 1;

	i = ml_make_prediction("test.txt", 10, buffer10, &length);
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	printf("String for buffer size 10: \t\t%s\n", buffer10);
	if (i != 1)				return 1;
	if ((int)strlen(buffer10) != length)		return 1;
	if (strcmp(buffer10, "sing_29.h"))	return 1;
	if (length != 9)			return 1;

	i = ml_make_prediction("test.txt", 11, buffer11, &length);
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	printf("String for buffer size 11: \t\t%s\n", buffer11);
	if (i != 1)				return 1;
	if ((int)strlen(buffer11) != length)		return 1;
	if (strcmp(buffer11, "sing_29.ht"))	return 1;
	if (length != 10)			return 1;

	i = ml_make_prediction("test.txt", 12, buffer12, &length);
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	printf("String for buffer size 12: \t\t%s\n", buffer12);
	if (i != 1)				return 1;
	if ((int)strlen(buffer12) != length)		return 1;
	if (strcmp(buffer12, "sing_29.htm"))	return 1;
	if (length != 11)			return 1;

	i = ml_make_prediction("test.txt", 20, buffer20, &length);
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	printf("String for buffer size 20: \t\t%s\n", buffer20);
	if (i != 1)				return 1;
	if ((int)strlen(buffer20) != length)		return 1;
	if (strcmp(buffer20, "sing_29.htm"))	return 1;
	if (length != 11)			return 1;

	i = ml_finalise();
	if (i != 1)				return 1;

	return 0;
}
