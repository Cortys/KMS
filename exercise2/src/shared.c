#include "shared.h"

size_t bufferSize(size_t size) {
	return sizeof(Buffer) + size;
}

int bufferInit(Buffer* buffer, int maxSize, int shared) {
	buffer->size = 0;
	buffer->maxSize = maxSize;
	buffer->position = 0;

	sem_init(&buffer->mutex, shared, 1);
	sem_init(&buffer->semRead, shared, 0);
	sem_init(&buffer->semWrite, shared, buffer->maxSize);

	for(int i = 0; i < buffer->maxSize; i++)
		buffer->values[i] = 0;

	return 0;
}

int sharedBufferAttach(int shmId, Buffer** buffer) {
	(*buffer) = shmat(shmId, NULL, SHM_RND);

	if(*buffer == (Buffer*) -1)
		return -1;

	return 0;
}

int sharedBufferInit(int shmId, Buffer** buffer, int maxSize) {
	if(sharedBufferAttach(shmId, buffer))
		return -1;

	return bufferInit(*buffer, maxSize, 1);
}

int bufferDestroy(Buffer* buffer) {
	return sem_destroy(&buffer->mutex)
		| sem_destroy(&buffer->semRead)
		| sem_destroy(&buffer->semWrite);
}

char bufferRead(Buffer* buffer) {
	sem_wait(&buffer->semRead);
	sem_wait(&buffer->mutex);

	char out = buffer->values[buffer->position];

	buffer->position = (buffer->position + 1) % buffer->maxSize;
	buffer->size--;

	sem_post(&buffer->semWrite);
	sem_post(&buffer->mutex);

	return out;
}

void bufferWrite(Buffer* buffer, char in) {
	sem_wait(&buffer->semWrite);
	sem_wait(&buffer->mutex);

	buffer->values[(buffer->position + buffer->size++) % buffer->maxSize] = in;

	sem_post(&buffer->semRead);
	sem_post(&buffer->mutex);
}

char* getTime() {
	time_t rawtime;

	time(&rawtime);

	char* res = asctime(localtime(&rawtime));

	res[strlen(res) - 1] = 0;

	return res;
}
