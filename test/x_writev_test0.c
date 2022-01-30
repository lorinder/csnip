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

	/* Write gather array */
	char s1[] = "Second pAr7.";
	char s2[] = "First part.";
	char s3[] = "Part #3!";
	struct x_iovec iov[] = {
		{ .iov_base = s2, .iov_len = sizeof(s2) - 1 },
		{ .iov_base = s1, .iov_len = sizeof(s1) - 1},
		{ .iov_base = s3, .iov_len = sizeof(s3) - 1},
	};
	if (x_writev(fd, iov, Static_len(iov)) == -1) {
		perror("writev");
		return 1;
	}
	if (close(fd) == -1) {
		perror("close");
		return 1;
	}

	/* Load file back */
	char buf[128];

	fd = open("tmpfile.txt", O_RDONLY, 0);
	if (fd == -1) {
		perror("open");
		return 1;
	}
	x_ssize_t n = read(fd, buf, sizeof(buf) - 1);
	if (n == -1) {
		perror("read");
		return 1;
	}
	buf[n] = '\0';
	if (close(fd) == -1) {
		perror("close");
		return 1;
	}

	/* Check */
	const char check[] = "First part.Second pAr7.Part #3!";
	int success = 1;
	if (strcmp(buf, check) != 0) {
		fprintf(stderr, "Error:  Mismatch on read.\n");
		success = 0;
	}

	/* Delete */
	if (unlink("tmpfile.txt") == -1) {
		perror("unlink");
		return 1;
	}

	return success ? 0 : 1;
}
