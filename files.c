#include <stdio.h>
#include <string.h>
#include <linux/io_uring.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/mman.h>

#ifdef __linux__
	#include <sys/syscall.h>
	#include <linux/fs.h>
#endif
#ifdef __APPLE__ // just so that we don't see red highlights in IDE
	#include <unistd_linux.h>
	#include <sys/syscall_linux.h>
#endif


// #ifndef __NR_io_uring_setup
// #define __NR_io_uring_setup 425
// #endif

#define BUFFER_SIZE 1023
#define BLKSIZE 4096
#include "src/util/signals.c"

#define read_barrier()  __asm__ __volatile__("":::"memory")
#define write_barrier() __asm__ __volatile__("":::"memory")

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
		fprintf(stdout, "%c", *buf++);
}
unsigned long min(unsigned long a, unsigned long b) {
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

struct app_io_sq_ring {
	unsigned *head;
	unsigned *tail;
	unsigned *ring_mask;
	unsigned *flags;
	unsigned *array;
};
struct app_io_cq_ring {
	unsigned *head;
	unsigned *tail;
	unsigned *ring_mask;
	struct io_uring_cqe *cqes;
};
struct submitter {
	int ring_fd;
	struct app_io_sq_ring sq_ring;
	struct io_uring_sqe *sqes;
	struct app_io_cq_ring cq_ring;
};
struct iovec_array {
	struct iovec *array;
	size_t len;
};

// https://unixism.net/loti/low_level.html
int io_uring_setup(unsigned entries, struct io_uring_params *params) {
	return syscall(SYS_io_uring_setup, entries, params);
}
int io_uring_enter(int ring_fd, unsigned int to_submit, unsigned int min_complete, unsigned int flags){
	return (int) syscall(SYS_io_uring_enter, ring_fd, to_submit, min_complete, flags, NULL, 0);
}

bool print_err(bool t, char *str) {
	if (t) {
		fputs(str, stderr);
		printf("\n");
	}
	return t;
}

int prepare_io_uring(struct submitter *result) {
	int ret = 0;
	struct io_uring_params params = {};
	int fd = 0;
	int entries = 1;
	if ((fd = io_uring_setup(entries, &params)) < 0) {
		fprintf(stderr, "Couldn't set up io_uring: %d", fd);
		return 1;
	}
	void *sqring, *sqentries, *cqring;
	sqring = mmap(NULL, params.sq_off.array + entries * sizeof(__uint32_t),
	                    PROT_READ | PROT_WRITE, MAP_SHARED |MAP_POPULATE, fd, IORING_OFF_SQ_RING);
	if (print_err(sqring == NULL, "Couldn't map submission queue"))
		return 2;
	sqentries = mmap(NULL, entries * sizeof(struct io_uring_sqe),
	                       PROT_READ | PROT_WRITE, MAP_SHARED |MAP_POPULATE, fd, IORING_OFF_SQES);
	if (print_err(sqentries == NULL, "Couldn't map submission queue entries"))
		return 3;
	cqring = mmap(NULL, params.cq_off.cqes + params.cq_entries * sizeof(struct io_uring_cqe),
						PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, IORING_OFF_CQ_RING);
	if (print_err(cqring == NULL, "Couldn't map completion queue"))
		return 4;

	result->ring_fd = fd;
	result->sqes = sqentries;

	result->sq_ring.head = sqring + params.sq_off.head;
	result->sq_ring.tail = sqring + params.sq_off.tail;
	result->sq_ring.array = sqring + params.sq_off.array;
	result->sq_ring.flags = sqring + params.sq_off.flags;
	result->sq_ring.ring_mask = sqring + params.sq_off.ring_mask;

	result->cq_ring.head = cqring + params.cq_off.head;
	result->cq_ring.tail = cqring + params.cq_off.tail;
	result->cq_ring.cqes = cqring + params.cq_off.cqes;
	result->cq_ring.ring_mask = cqring + params.cq_off.ring_mask;
	return ret;
}
struct file_info {
	off_t file_sz;
	struct iovec iovecs[];      /* Referred by readv/writev */
};
void submitToSq(int fd, struct submitter * submitter) {
	struct file_info fi;
	off_t filesize = get_file_size(fd);
	// 1. Calculate the number of iovecs we need
	// 2. Init every iovec with the len and buffer
	// 3. add it to the sqe
	size_t block_cnt = filesize / BLKSIZE + (filesize % BLKSIZE != 0);
	printf("[SUBMIT] Filesize: %lu, block count: %lu\n", filesize, block_cnt);
	struct iovec *iovecs = malloc(block_cnt * sizeof(struct iovec));
	size_t bytesLeft = filesize;
	for (int i = 0; i < block_cnt; i++, bytesLeft -= BLKSIZE) {
		iovecs[i].iov_len = min(BLKSIZE, bytesLeft);
		iovecs[i].iov_base = malloc(iovecs[i].iov_len);
	}
	struct iovec_array *iovec_array = malloc(sizeof(struct iovec_array));
	iovec_array->array = iovecs;
	iovec_array->len = block_cnt;

	unsigned tail = *submitter->sq_ring.tail;
	read_barrier();
	fprintf(stdout, "[SUBMIT] tail=%u (%p), head=%u (%p)\n",
		tail, submitter->sq_ring.tail,
		*submitter->sq_ring.head, submitter->sq_ring.head);
	unsigned index = tail & *submitter->sq_ring.ring_mask;
	struct io_uring_sqe *sqe = &submitter->sqes[index];
	sqe->fd = fd;
	sqe->opcode = IORING_OP_READV;
	sqe->addr = (__uint64_t) iovecs;
	sqe->len = block_cnt;
	sqe->user_data = (__uint64_t) iovec_array;
	sqe->off = 0;
	fprintf(stdout, "[SUBMIT] Set sqe[%d].user_data=%lld (%p), submitted %lu iovecs\n", index, sqe->user_data, iovecs, block_cnt);

	submitter->sq_ring.array[index] = index;
	(*submitter->sq_ring.tail)++;
	write_barrier();// if Kernel is set up to continuously poll w/o a syscall, we'd need the barrier before updating tail
	fprintf(stdout, "[SUBMIT] New SQ tail=%u (%p)\n", *submitter->sq_ring.tail, submitter->sq_ring.tail);

	int ret = 0;
	if ((ret = io_uring_enter(submitter->ring_fd, 1, 1, IORING_ENTER_GETEVENTS)) < 0) {
		fprintf(stderr, "io_uring_enter() returned %d\n", ret);
	}
	fprintf(stdout, "[SUBMIT] Finished submitting %d\n", ret);
}
void readFromCq(struct submitter *s) {
	struct app_io_cq_ring *cqring = &s->cq_ring;
	do {
		read_barrier();
		while (*cqring->tail == *cqring->head)
			;
		fprintf(stdout, "[READ  ] cqe.tail=%u %p, head=%u %p\n", *cqring->tail, cqring->tail, *cqring->head, cqring->head);
		struct io_uring_cqe *cqe = &cqring->cqes[*cqring->head & *cqring->ring_mask];
		fprintf(stdout, "[READ  ] cqe user_data=%lld, result=%d, flags=%d\n", cqe->user_data, cqe->res, cqe->flags);
		if (cqe->res < 0) {
			fprintf(stderr, "[READ  ] The Completion Queue Entry (CQE) result: %d", cqe->res);
			return;
		}

		struct iovec_array *iovec_array = (struct iovec_array*)cqe->user_data;
		fprintf(stdout, "[READ  ] Writing out %lu blocks:\n", iovec_array->len);
		for (int i = 0; i < iovec_array->len; i++) {
			output_buf(iovec_array->array[i].iov_base, iovec_array->array[i].iov_len);
			free(iovec_array->array[i].iov_base);
		}
		fprintf(stdout, "\n");
		free(iovec_array);

		(*cqring->head)++;
		write_barrier();
	} while (true);
}

// TODO:
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
		puts(buffer);
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
		puts(buffer);
		fprintf(stdout, "*** END OF BUFFER: %lu", n);
	}
	close(fd);
	return 0;
}

int main(void) {
	register_signals();
	int errorCode;

	char *filename = "somefile.txt";
	int fd = open(filename, O_RDWR);
	if (fd < 0)
		printf("Couldn't open the file: %d\n", fd);
	printf("Bytes in the file: %ld\n", get_file_size(fd));
	// read_and_print_file_readv(filename);

	struct submitter s = {};
	if (prepare_io_uring(&s))
		return 5;
	submitToSq(fd, &s);
	readFromCq(&s);
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
