#include <stdio.h>
#include <errno.h>       /* errno */
#include <string.h>      /* strerror */
#include <stdlib.h>      /* exit */
#include <pthread.h>     /* pthread_create */

#include <sys/types.h>   /* socket */
#include <sys/socket.h>
#include <netinet/in.h>  /* inet_addr */
#include <arpa/inet.h>   /* htons */

#define TCP_TEST_PORT 8888

void process_thread(void *arg)
{
    int conn = *(int *)arg;
    int recv_len;
	char recv_msg[256];

	memset(recv_msg, 0, sizeof(recv_msg));
	/* tcp 编程必须先通过client端的connect和server端的accept建立连接，然后才能接收消息，直接接收会导致失败 */
	recv_len = recv(conn, recv_msg, sizeof(recv_msg), MSG_WAITALL);//阻塞模式接收
	if (recv_len == -1) {
		printf("receive message error : %s\n", strerror(errno));
		exit(0);
	}
	printf("*******************\n");
	printf("\treceive buffer size : %d\n", recv_len);
	printf("\tbuffer : %s\n", recv_msg);
		
	/* 不能使用sendto() */
	
	//char s[10];
	//scanf("%s", s);
	send(conn, "OK\n", 3, 0);
	printf("send OK\n");
}

int main(int argc, char **argv)
{
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	char buffer[256];
	int sock;
	int conn;
	int addr_len = sizeof(struct sockaddr_in);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Create socket fail: %s\n", strerror(errno));
		return 0;
	}
	
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(TCP_TEST_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);// 所有网卡地址
	if(bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
		printf("Bind socket fail: %s\n", strerror(errno));
		return 0;
	}
	/**
	 * socket创建后，默认是要去进行链接的，listen()会使socket进入监听状态
	 * 参数2的意义：设置监听队列的大小，每当有一个connect()请求，就会进入监听队列，每当一个请求被accpet()接收，
	 * 请求就被移除，当请求队列满时，客户端的connet请求就会返回服务器繁忙的提示
	 */
	listen(sock, 10);
	/* 返回新的套接字，这个套接字是具有面向连接的属性，之后使用这个套接字进行通讯 
	 * 猜测这个套接字中同时保存着客户端和服务端的ip和port
	 * 只有tcp才能使用accept()，这也是tcp的核心实现，UDP是不能使用accept的
	 */
	pthread_t tid;
	while(1)
	{
		conn = accept(sock, (struct sockaddr *)&client_addr, &addr_len);
		if (conn < 0) {
			printf("Accept fail : %s\n", strerror(errno));
			exit(0);
		}
		printf("\tclient ip : 0x%x\n", ntohl(client_addr.sin_addr.s_addr));
		printf("\tclient port : %d\n", ntohs(client_addr.sin_port));
		pthread_create(&tid, NULL, (void *)process_thread, &conn); 
	}
}


