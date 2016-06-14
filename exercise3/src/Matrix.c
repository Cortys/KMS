#include <stdio.h>

#include "Matrix.h"

int matrixCreate(Matrix** matrix, int rows, int columns) {
	*matrix = (Matrix*) malloc(sizeof(Matrix));

	if(*matrix == NULL)
		return -1;

	(*matrix)->rows = rows;
	(*matrix)->columns = columns;
	int** data = (*matrix)->data = malloc(sizeof(int*[rows]));

	if(data == NULL)
		return -1;

	for(int i = 0; i < rows; i++) {
		data[i] = malloc(sizeof(int[columns]));

		if(data[i] == NULL)
			return -1;
	}

	return 0;
}

int matrixDestroy(Matrix* matrix) {
	if(matrix == NULL)
		return -1;

	int rows = matrix->rows;
	int** data = matrix->data;

	for(int i = 0; i < rows; i++)
		free(data[i]);

	free(data);
	free(matrix);

	return 0;
}

void matrixIterateByRows(Matrix* matrix, void* meta, matrixCellFunction* function) {
	int rows = matrix->rows;
	int columns = matrix->columns;
	int** data = matrix->data;

	for(int row = 0; row < rows; row++)
		for(int column = 0; column < columns; column++)
			function(&data[row][column], meta);
}

void matrixIterateByColumns(Matrix* matrix, void* meta, matrixCellFunction* function) {
	int rows = matrix->rows;
	int columns = matrix->columns;
	int** data = matrix->data;

	for(int column = 0; column < columns; column++)
		for(int row = 0; row < rows; row++)
			function(&data[row][column], meta);
}

typedef struct {
	int current;
	int max;
	int cellWidth;
} printState_t;

static void printCell(int* cell, void* meta) {
	printState_t* ps = (printState_t*) meta;

	printf("%*d", ps->cellWidth, *cell);

	if(++ps->current >= ps->max) {
		ps->current = 0;
		printf("\n");
	}
	else
		printf("  ");
}

void matrixPrint(Matrix* matrix, int cellWidth) {
	printState_t* ps = malloc(sizeof(printState_t));

	ps->current = 0;
	ps->max = matrix->columns;
	ps->cellWidth = cellWidth;

	matrixIterateByRows(matrix, ps, printCell);

	free(ps);
}
