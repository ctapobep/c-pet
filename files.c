#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/io_uring.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/fs.h>


#define BUFFER_SIZE 1023

off_t get_file_size(int fd) {
	struct stat st;
	memset(&st, 0, sizeof(struct stat));
	int error = 0;
	if ((error = fstat(fd, &st)) < 0) {
		fprintf(stderr, "Couldn't stat the file, got error: %d\n", error);
		return error;
	}
	if (S_ISBLK(st.st_mode)) {
		off_t size;
		if (ioctl(fd, BLKGETSIZE64, &size)) {
			fprintf(stderr, "Couldn't ioctl the block device");
			return -2;
		}
		return size;
	} else if (S_ISREG(st.st_mode))
		return st.st_size;
	return -1;
}

// TODO:
// 1. Use io_uring
// 2. Use epoll
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
		printf("Couldn't open %s for reading\n", filename);
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
		printf("Couldn't open %s for reading\n", filename);
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

	char *filename = "somefile.txt";
	int fd = open(filename, O_RDWR);
	if (fd < 0)
		printf("Couldn't open the file: %d\n", fd);
	printf("Bytes in the file: %llu\n", get_file_size(fd));
	// if ((errorCode = writesync_hi()))
		// return errorCode;
	// if ((errorCode = readsync_lo()))
		// return errorCode;
	// if ((errorCode = writeasync_lo()))
		// return errorCode;
	// if ((errorCode = readasync_lo()))
		// return errorCode;
	// if ((errorCode = write_iouring()))
		// return errorCode;
}
