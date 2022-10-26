#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>

#define CSNIP_SHORT_NAMES
#include <csnip/x.h>
#include <csnip/x_unistd.h>
#include <csnip/util.h>

typedef x_ssize_t (*readv_funcptr)(int, const struct x_iovec*, int);

int runtest(const char* rfun_name, readv_funcptr rfun)
{
	printf("Checking %s: ", rfun_name);
	fflush(stdout);

	/* Load file back, using readv() */
	int fd = open("tmpfile.txt", O_RDONLY, 0);
	if (fd == -1) {
		perror("open");
		return 0;
	}

	char buf[26 + 1];
	struct x_iovec iov[] = {
		{ .iov_base = buf + 7, .iov_len = 13},
		{ .iov_base = buf + 0, .iov_len = 7},
		{ .iov_base = buf + 20, .iov_len = 7},
	};
	const char check[] = "NOPQRSTABCDEFGHIJKLMUVWXYZ";
	x_ssize_t n = (*rfun)(fd, iov, Static_len(iov));
	if (n == -1) {
		perror("readv");
		return 0;
	}
	if (close(fd) == -1) {
		perror("close");
		return 0;
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

	/* Report */
	puts(success ? "pass" : "FAIL");

	return success;
}

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

	/* Run checks */
	int success = runtest("x_readv", x_readv);
	success = runtest("x_readv_imp", x_readv_imp) && success;

	/* Delete */
	if (unlink("tmpfile.txt") == -1) {
		perror("unlink");
		return 1;
	}

	return success ? 0 : 1;
}
