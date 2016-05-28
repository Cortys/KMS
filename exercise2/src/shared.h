#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <signal.h>

typedef struct {
	unsigned int size;
	unsigned int maxSize;
	unsigned int position;
	sem_t mutex;
	sem_t semRead;
	sem_t semWrite;
	char values[];
} Buffer;

size_t bufferSize(size_t size);
int bufferInit(Buffer* buffer, int maxSize, int shared);

char bufferRead(Buffer* buffer);
void bufferWrite(Buffer* buffer, char in);

int sharedBufferAttach(int shmId, Buffer** buffer);
int sharedBufferInit(int shmId, Buffer** buffer, int maxSize);

char* getTime();
int getRandom(int min, int max);
