#include <stdlib.h>

typedef struct {
	int rows;
	int columns;
	int** data;
} Matrix;

int matrixCreate(Matrix** matrix, int rows, int columns);
int matrixDestroy(Matrix* matrix);

typedef void (matrixCellFunction)(int* cell, void* meta);
typedef void (matrixIteratorFunction)(Matrix* matrix, void* meta, matrixCellFunction* function);

void matrixIterateByRows(Matrix* matrix, void* meta, matrixCellFunction* function);
void matrixIterateByColumns(Matrix* matrix, void* meta, matrixCellFunction* function);

void matrixPrint(Matrix* matrix, int cellWidth);
