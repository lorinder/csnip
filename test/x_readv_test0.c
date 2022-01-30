#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>

#define CSNIP_SHORT_NAMES
#include <csnip/x.h>
#include <csnip/util.h>

int main(int argc, char** argv)
{
	/* Open file */
	int fd = open("tmpfile.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
	if (fd == -1) {
		perror("open");
		return 1;
	}

	/* Write stuff to it */
	const char orig[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (write(fd, orig, Static_len(orig)) == -1) {
		perror("write");
		return 1;
	}
	if (close(fd) == -1) {
		perror("close");
		return 1;
	}

	/* Load file back, using readv() */
	fd = open("tmpfile.txt", O_RDONLY, 0);
	if (fd == -1) {
		perror("open");
		return 1;
	}

	char buf[26 + 1];
	struct x_iovec iov[] = {
		{ .iov_base = buf + 7, .iov_len = 13},
		{ .iov_base = buf + 0, .iov_len = 7},
		{ .iov_base = buf + 20, .iov_len = 7},
	};
	const char check[] = "NOPQRSTABCDEFGHIJKLMUVWXYZ";
	x_ssize_t n = x_readv(fd, iov, Static_len(iov));
	if (n == -1) {
		perror("readv");
		return 1;
	}
	if (close(fd) == -1) {
		perror("close");
		return 1;
	}

	/* Check */
	int success = 1;
	if (strcmp(buf, check) != 0) {
		fprintf(stderr, "Error:  No match.\n"
				"        Got:      \"%s\"\n"
				"        Expected: \"%s\"\n",
			buf, check);
		success = 0;
	}

	/* Delete */
	if (unlink("tmpfile.txt") == -1) {
		perror("unlink");
		return 1;
	}

	return success ? 0 : 1;
}
