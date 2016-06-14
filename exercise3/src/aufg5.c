#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <getopt.h>

#include "Matrix.h"

typedef unsigned long long matrixSum_t;

void initCell(int* cell, void* meta) {
	*cell = rand();
}

void initByRows(Matrix* matrix) {
	matrixIterateByRows(matrix, NULL, initCell);
}

void initByColumns(Matrix* matrix) {
	matrixIterateByColumns(matrix, NULL, initCell);
}

void sumCell(int* cell, void* target) {
	*((unsigned long long *) target) += *cell;
}

matrixSum_t sum(Matrix* matrix, matrixIteratorFunction* iterator) {
	matrixSum_t result = 0;

	iterator(matrix, &result, sumCell);

	return result;
}

matrixSum_t sumByRows(Matrix* matrix) {
	return sum(matrix, matrixIterateByRows);
}

matrixSum_t sumByColumns(Matrix* matrix) {
	return sum(matrix, matrixIterateByColumns);
}

void quit(char* argv[]) {
	fprintf(stderr, "Usage: %s -r [matrix rows] -c [matrix columns] -i [r (init by row), c (init by column)] -s [n (no sum), r (sum by row), c (sum by column)] -p\n", argv[0]);
	exit(1);
}

int main(int argc, char *argv[]) {
	// Parse arguments:
	int opt;
	int rows = 0;
	int columns = 0;
	unsigned char initMode = 'r';
	unsigned char calculateSum = 'n';
	unsigned char print = 0;

	while((opt = getopt(argc, argv, "r:c:i:s:p")) != -1)
		switch (opt) {
			case 'r':
				rows = atoi(optarg);
				break;
			case 'c':
				columns = atoi(optarg);
				break;
			case 'i':
				initMode = optarg[0];
				break;
			case 's':
				calculateSum = optarg[0];
				break;
			case 'p':
				print = 1;
				break;
			default:
				quit(argv);
		}

	if(rows <= 0 || columns <= 0
		|| (initMode != 'r' && initMode != 'c')
		|| (calculateSum != 'n' && calculateSum != 'r' && calculateSum != 'c'))
		quit(argv);

	// Create matrix:
	srand(time(NULL));

	Matrix* matrix;

	if(matrixCreate(&matrix, rows, columns)) {
		perror("Could not create matrix.");
		exit(1);
	}

	printf("Created %d x %d matrix.\n", matrix->rows, matrix->columns);

	// Initialize matrix:
	char* type;

	if(initMode == 'r') {
		initByRows(matrix);
		type = "row-wise";
	}
	else {
		initByColumns(matrix);
		type = "column-wise";
	}

	printf("Initialized matrix %s with random values.\n", type);

	// Optionally print matrix:
	if(print && columns <= 20)
		matrixPrint(matrix, 10);

	// Optionally calculate matrix sum:
	if(calculateSum != 'n') {
		matrixSum_t sum;
		char* type;

		if(calculateSum == 'r') {
			sum = sumByRows(matrix);
			type = "Row-wise";
		}
		else {
			sum = sumByColumns(matrix);
			type = "Column-wise";
		}

		printf("%s matrix sum: %llu.\n", type, sum);
	}

	// Cleanup:
	matrixDestroy(matrix);

	return 0;
}
