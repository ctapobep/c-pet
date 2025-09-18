#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/io_uring.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
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

void output_buf(const char *buf, size_t len) {
	while (len--)
		fputc(*buf++, stdout);
}
long min(long a, long b) {
	return a < b ? a : b;
}
int read_and_print_file_readv(char *filename) {
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Couldn't open the file %s\n", filename);
		return 1;
	}
	off_t filesize = get_file_size(fd);
	if (filesize <= 0)
		return -1;
#define BLKSIZE 4096
	int block_cnt = (int)(filesize / BLKSIZE) + (filesize % BLKSIZE > 0);

	struct iovec iovecs[block_cnt];
	// Now let's create the iovecs
	for (ssize_t i = 0, bytes_remaining = filesize; i < block_cnt; i++, bytes_remaining -= BLKSIZE) {
		size_t blksize = min(BLKSIZE, bytes_remaining);
		void *buf = malloc(blksize);
		iovecs[i].iov_base = buf;
		iovecs[i].iov_len = blksize;
	}
	int ret = 0;
	ssize_t error = 0;
	if ((error = readv(fd, iovecs, block_cnt)) < 0) {
		fprintf(stderr, "Couldn't readv(): %ld\n", error);
		ret = 1;
		goto free;
	}
	for (int i = 0; i < block_cnt; i++)
		output_buf(iovecs[i].iov_base, iovecs[i].iov_len);

free:
	for (int i = 0; i <block_cnt; i++)
		free(iovecs[i].iov_base);
	return ret;
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
	read_and_print_file_readv(filename);
	close(fd);
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
