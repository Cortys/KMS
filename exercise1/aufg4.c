#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/wait.h>

static const int SIZES[] = { 1, 10, 100, 1000 };
static const int SIZESLENGTH = sizeof(SIZES) / sizeof(*SIZES);
static char* DIRECTORY = "test";

int numLength(int num) {
	if(num < 10)
		return 1;

	return 1 + numLength((num - num % 10) / 10);
}

char *calculateFilename(int size, int id) {
	if(size == 0)
		size = SIZES[id % SIZESLENGTH];

	char *filename = malloc(8 + strlen(DIRECTORY) + numLength(size) + numLength(id));

	sprintf(filename, "%s/test-%d-%d", DIRECTORY, size, id);

	return filename;
}

FILE *openFile(char *filename) {
	return fopen(filename, "r");
}

unsigned char calculateXOR(FILE *fd) {
	unsigned char result = 0;
	unsigned char tmp[1];

	while(fread(tmp, sizeof(unsigned char), 1, fd))
		result ^= tmp[0];

	return result;
}

char *charToBinary(unsigned char c) {
	char *result = malloc(9);

	for (int i = 0; i < 8; i++)
		result[7 - i] = (c >> i) & 1 ? '1' : '0';

	result[8] = '\0';

	return result;
}

void handleFile(int size, int id) {
    char *filename = calculateFilename(size, id);

	FILE *fd = openFile(filename);

	if(fd == NULL) {
		fprintf(stderr, "Could not open file '%s'.\n", filename);
		free(filename);
		return;
	}

	unsigned char result = calculateXOR(fd);

	char *binResult = charToBinary(result);

	printf("File %s\t has XOR = %s = %hhu\n", filename, binResult, result);

	fclose(fd);
	free(filename);
	free(binResult);
}

void sequential(int size) {
	for (int i = 1; i <= 10; i++)
		handleFile(size, i);
}

void multiprocess(int size) {
	for (int i = 1; i <= 10; i++)
		if(!fork()) {
			handleFile(size, i);
			exit(0);
		}

	while(wait(NULL) > 0);
}

typedef struct {
	int size;
	int id;
} threadcall_t;

void *threadcall(void *vargs) {
    threadcall_t *args = (threadcall_t*) vargs;

	handleFile(args->size, args->id);

	free(args);

	return NULL;
}

void multithreaded(int size) {
	pthread_t thread[10];

    for (int i = 1; i <= 10; i++) {
		threadcall_t *args = malloc(sizeof(threadcall_t));
		args->size = size;
		args->id = i;

		pthread_create(&thread[i - 1], NULL, threadcall, (void*) args);
	}

	for (int i = 0; i < 10; i++)
		pthread_join(thread[i], NULL);
}

int main(int argc, char *args[]) {
	char mode = 's';
	char allSizes = 0;

	if(argc > 1) {
		if(args[1][0] == 'a') {
			allSizes = 1;

			if(strlen(args[1]) > 1)
				mode = args[1][1];
		}
		else {
			mode = args[1][0];

			if(strlen(args[1]) > 1 && args[1][1] == 'a')
				allSizes = 1;
		}

        if(argc > 2)
            DIRECTORY = args[2];
	}

	void (*function)(int);

	switch (mode) {
		case 's':
			puts("Sequential mode:\n");
			function = &sequential;
			break;
		case 'p':
			puts("Multiprocess mode:\n");
			function = &multiprocess;
			break;
		case 't':
			puts("Multithreaded mode:\n");
			function = &multithreaded;
			break;
		default:
			fprintf(stderr, "Invalid flag '%c'.\n\nUse\t's' = sequential (default),\n\t'p' = multiprocess\nor\t't' = multithreaded.\n\nUse\t'a' to XOR all 40 testfiles instead of just 10.\n\n", mode);

			return 1;
	}

	if(allSizes)
		for(int i = 0; i < SIZESLENGTH; i++)
			(*function)(SIZES[i]);
	else
		(*function)(0);

	return 0;
}
