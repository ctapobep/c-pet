#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1023

// TODO:
// 1. Implement io_uring
// 2. Implement MacOS async io

int writesync_hi(void) {
	char *filename = "somefile.txt";
	FILE *fd = fopen(filename, "a");
	if (fd == NULL) {
		printf("Couldn't open %s for writing", filename);
		return 1;
	}
	for (int i = 0; i < 10; i++) {
		fwrite(filename, sizeof(char), strlen(filename),fd);
		fwrite("\n", sizeof(char), 1, fd);
	}
	fclose(fd);
	return 0;
}
int writeasync_lo(void) {
	char *filename = "somefile.txt";
	int fd = open(filename, O_APPEND | O_NONBLOCK);
	if (fd == -1) {
		printf("Couldn't open %s for writing", filename);
		return 1;
	}
	size_t textLen = strlen(filename);
	for (int i = 0; i < 10; i++) {
		write(fd, filename, textLen);
		write(fd, "\n",1);
	}
	close(fd);
	return 0;
}

int readsync_lo() {
	char *filename = "somefile.txt";
	int fd = open(filename,  O_RDONLY);
	if (fd == -1) {
		printf("Couldn't open %s for reading", filename);
		return 2;
	}
	char buffer[BUFFER_SIZE+1];
	ssize_t n;
	while ((n = read(fd, buffer, BUFFER_SIZE)) > 0) {
		buffer[n] = '\0';
		fprintf(stdout, buffer);
		fprintf(stdout, "*** END OF BUFFER: %lu", n);
	}
	close(fd);
	return 0;
}
int readasync_lo() {
	char *filename = "somefile.txt";
	int fd = open(filename,  O_RDONLY | O_NONBLOCK);
	if (fd == -1) {
		printf("Couldn't open %s for reading", filename);
		return 2;
	}
	char buffer[BUFFER_SIZE+1];
	ssize_t n;
	while ((n = read(fd, buffer, BUFFER_SIZE)) > 0) {
		buffer[n] = '\0';
		fprintf(stdout, buffer);
		fprintf(stdout, "*** END OF BUFFER: %lu", n);
	}
	close(fd);
	return 0;
}
ssize_t readall(int fd, void * data, size_t count) {

	ssize_t bytesRead;

	char *dataPtr = data;
	size_t total = 0;
	while (count) {
		bytesRead = read(fd, dataPtr, count);
		/* we should check bytesRead for < 0 to return errors
		properly, but this is just sample code! */
		dataPtr += bytesRead;
		count -= bytesRead;
		total += bytesRead;
	}

	return total;
}

int main(void) {
	int errorCode;
	// if ((errorCode = writesync_hi()))
		// return errorCode;
	// if ((errorCode = readsync_lo()))
		// return errorCode;
	if ((errorCode = writeasync_lo()))
		return errorCode;
	if ((errorCode = readasync_lo()))
		return errorCode;
}
