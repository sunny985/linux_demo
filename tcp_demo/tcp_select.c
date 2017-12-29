#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TCP_TEST_PORT 8888

static int creat_socket(int port)
{
	int sock, ret;
	struct sockaddr_in addr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Create socket fail: %s\n", strerror(errno));
		return 0;
	}
	
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr));
	if(ret == -1) {
		printf("Bind socket fail: %s\n", strerror(errno));
		return 0;
	}

	return sock;
}

int main(int argc, char **argv)
{
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	char buffer[256];
	int sock,conn;
	int addr_len = sizeof(struct sockaddr_in);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Create socket fail: %s\n", strerror(errno));
		return 0;
	}
	
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
		printf("Bind socket fail: %s\n", strerror(errno));
		goto fail;
	}
	
	listen(sock, 10);

	if((conn = accept(sock, (struct sockaddr *)&client_addr, &addr_len)) == -1)
	{
		printf("accept error : %s\n", strerror(errno));
		return 0;
	}
	while(1) {
		int rec_len;
		char reply_msg[10];
		/* tcp 编程必须先通过client端的connect和server端的accept建立连接，然后才能接收消息，直接接收会导致失败 */
		rec_len = recv(conn, buffer, sizeof(buffer), 0);
		if (rec_len == -1) {
			printf("receive message error : %s\n", strerror(errno));
			return 0;
		}
		printf("*******************\n");
		printf("\treceive buffer size : %d\n", rec_len);
		printf("\tbuffer : %s\n", buffer);
		printf("\tclient ip : 0x%x\n", ntohl(client_addr.sin_addr.s_addr));
		printf("\tclient port : %d\n", ntohs(client_addr.sin_port));
		
		/* 不能使用sendto() */
		memset(reply_msg, 0, sizeof(reply_msg));
		printf("please input reply message:");
		scanf("%s", reply_msg);
		send(conn, reply_msg, sizeof(reply_msg), 0);
	}

fail:
	close(sock);
               
}



