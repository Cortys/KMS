#include "shared.h"

#include <getopt.h>

#define SPEED 9900

void readData(Buffer* buffer) {
	printf("[%s] Starting to consume data.\n", getTime());

	while(1) {
		printf("[%s][%d/%d] Read '%d' from buffer.\n", getTime(), buffer->size, buffer->maxSize, bufferRead(buffer));
		usleep(SPEED);
	}
}

void quit(char* argv[]) {
	fprintf(stderr, "Usage: %s -p producerPID -s shmID\n", argv[0]);
	exit(1);
}

int main(int argc, char *argv[]) {

	int opt;
	int pid = 0;
	int shmId = 0;

	while((opt = getopt(argc, argv, "p:s:")) != -1)
		switch (opt) {
			case 'p':
				pid = atoi(optarg);
				break;
			case 's':
				shmId = atoi(optarg);
				break;
			default:
				quit(argv);
		}

	if(pid <= 0 || shmId <= 0)
		quit(argv);

	Buffer* buffer;

	if(sharedBufferAttach(shmId, &buffer)) {
		perror("Could not attach to buffer");
		return 1;
	}

	kill(pid, SIGUSR1);

	printf("[%s] Consumer is ready. Sent SIGUSR1 to PID=%d.\n", getTime(), pid);

	readData(buffer);

	return 0;
}
