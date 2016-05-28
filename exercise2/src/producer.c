#include "shared.h"

#define BUFFERSIZE 10
#define SPEEDMIN 5000
#define SPEEDMAX 25000

int createSharedMemory() {
	key_t key = ftok("producer", 'x');

	return shmget(key, bufferSize(BUFFERSIZE), IPC_CREAT | 0666);
}

int destroySharedMemory(int shmId) {
	return shmctl(shmId, IPC_RMID, NULL);
}

void generateData(Buffer* buffer) {
	char i = 1;

	printf("[%s] Starting to produce data.\n", getTime());

	while(1) {
		bufferWrite(buffer, i);
		printf("[%s][%d/%d] Wrote '%d' to buffer.\n", getTime(), buffer->size, buffer->maxSize, i);
		i++;
		usleep(getRandom(SPEEDMIN, SPEEDMAX));
	}
}

int main() {
	int shmId = createSharedMemory();

	if(shmId == -1) {
		perror("Could not create shared memory");
		return 1;
	}

	Buffer* buffer;

	if(sharedBufferInit(shmId, &buffer, BUFFERSIZE)) {
		perror("Could not initialize buffer");
		return 1;
	}

	fprintf(stderr, "%d", shmId);

	sigset_t sigset;
	int sig;

	sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	printf("[%s] Producer is ready. Waiting for SIGUSR1 to PID=%d.\n", getTime(), getpid());

	sigwait(&sigset, &sig);

	generateData(buffer);

	destroySharedMemory(shmId);

	return 0;
}
