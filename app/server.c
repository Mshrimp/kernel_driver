#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define BUF_SIZE			1024

#define	CLNT_MAX_CNT		5

void ser_error(char *message)
{
	printf("\033[1;31m%s\033[0m\n", message);
}

int main(int argc, char *argv[])
{
	int ret = -1;
	int data = 0;

	int serv_sockfd;
	int clnt_sockfd;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;

	socklen_t clnt_addr_size;
	int str_len = 0;
	int i = 0;

	char message[BUF_SIZE] = {0};

	if (argc != 2) {
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	srand((unsigned)time(NULL));

	serv_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sockfd == -1) {
		ser_error("socket() error");
		exit(2);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	ret = bind(serv_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (ret == -1) {
		ser_error("bind() error");
		exit(6);
	}

	ret = listen(serv_sockfd, 5);
	if (ret == -1) {
		ser_error("listen() failed");
		exit(3);
	}

	clnt_addr_size = sizeof(clnt_addr);

	memcpy(message, "Hello world!", 20);

	for (i = 0; i < CLNT_MAX_CNT; i++) {
		clnt_sockfd = accept(serv_sockfd, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
		if (clnt_sockfd == -1) {
			ser_error("accept() failed");
			exit(4);
		} else {
			printf("Client %d connected\n", i + 1);
		}

		data = rand() % 0xFFFF;
		printf("data = 0x%X\n", data);

		ret = write(clnt_sockfd, &data, sizeof(data));
		if (ret == -1) {
			ser_error("write() error");
			exit(5);
		}

/*
 *        str_len = read(clnt_sockfd, message, BUF_SIZE);
 *        if (str_len > 0) {
 *            printf("Server: read message: %s\n", message);
 *
 *            ret = write(clnt_sockfd, message, str_len);
 *            if (ret == -1) {
 *                ser_error("write() error");
 *                exit(5);
 *            }
 *        } else {
 *            ser_error("read() error");
 *            exit(6);
 *        }
 */

		close(clnt_sockfd);
	}

	close(serv_sockfd);

	return 0;
}

