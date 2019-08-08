#include <stdio.h>
#include <time.h>
#include "mlpredict.h"

int main(int argc, char *argv[])
{
	int i = 0;
	int length = 0;
	char buffer10[10];
	char buffer11[11];
	char buffer12[12];
	char buffer20[20];
	clock_t t;
	double time_taken;
	if (argc != 1) {
		printf("Usage: %s\n", argv[0]);
		return 1;
	}

	t = clock();
	i = ml_is_initialised();
	t = clock() - t;
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	time_taken = ((double)t)/CLOCKS_PER_SEC; /* in seconds */
	printf("%f seconds\n\n", time_taken);

	t = clock();
	i = ml_initialise();
	t = clock() - t;
	printf("Returnvalue for ml_initialise: \t\t%d\n", i);
	time_taken = ((double)t)/CLOCKS_PER_SEC; /* in seconds */
	printf("%f seconds\n\n", time_taken);

	t = clock();
	i = ml_is_initialised();
	t = clock() - t;
	printf("Returnvalue for ml_is_initialised: \t%d\n", i);
	time_taken = ((double)t)/CLOCKS_PER_SEC; /* in seconds */
	printf("%f seconds\n\n", time_taken);

	t = clock();
	i = ml_make_prediction("test.txt", 10, buffer10, &length);
	t = clock() - t;
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	printf("String for buffer size 10: \t\t%s\n", buffer10);
	time_taken = ((double)t)/CLOCKS_PER_SEC; /* in seconds */
	printf("%f seconds\n\n", time_taken);

	t = clock();
	i = ml_make_prediction("test.txt", 11, buffer11, &length);
	t = clock() - t;
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	printf("String for buffer size 11: \t\t%s\n", buffer11);
	time_taken = ((double)t)/CLOCKS_PER_SEC; /* in seconds */
	printf("%f seconds\n\n", time_taken);

	t = clock();
	i = ml_make_prediction("test.txt", 12, buffer12, &length);
	t = clock() - t;
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	printf("String for buffer size 12: \t\t%s\n", buffer12);
	time_taken = ((double)t)/CLOCKS_PER_SEC; /* in seconds */
	printf("%f seconds\n\n", time_taken);

	t = clock();
	i = ml_make_prediction("test.txt", 20, buffer20, &length);
	t = clock() - t;
	printf("Returnvalue for ml_make_prediciton: \t%d\n", i);
	printf("String for buffer size 20: \t\t%s\n", buffer20);
	time_taken = ((double)t)/CLOCKS_PER_SEC; /* in seconds */
	printf("%f seconds\n\n", time_taken);

	t = clock();
	i = ml_finalise();
	t = clock() - t;
	printf("Returnvalue for ml_finalise: \t\t%d\n", i);
	time_taken = ((double)t)/CLOCKS_PER_SEC; /* in seconds */
	printf("%f seconds\n\n", time_taken);

	return 0;
}
