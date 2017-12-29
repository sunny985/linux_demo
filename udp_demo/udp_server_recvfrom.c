#include <stdio.h>
#include <errno.h>       /* errno */
#include <string.h>      /* strerror */

#include <sys/types.h>   /* socket */
#include <sys/socket.h>
#include <netinet/in.h>  /* inet_addr */
#include <arpa/inet.h>   /* htons */

#define UDP_TEST_PORT 7777

int main(int argc, char **argv)
{
	struct sockaddr_in addr;
	struct sockaddr_in client_addr;
	char buffer[256];
	int sock;
	int addr_len = sizeof(struct sockaddr_in);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == -1) {
		printf("Create socket fail: %s\n", strerror(errno));
		return 0;
	}
	
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(UDP_TEST_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);// 所有网卡地址
	
	if(bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
		printf("Bind socket fail: %s\n", strerror(errno));
		return 0;
	}

	while(1) {
		int rec_len;
		rec_len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
		printf("*******************\n");
		printf("\treceive buffer size : %d\n", rec_len);
		printf("\tbuffer : %s\n", buffer);
		printf("\tclient ip : 0x%x\n", ntohl(client_addr.sin_addr.s_addr));
		printf("\tclient port : %d\n", ntohs(client_addr.sin_port));
		sendto(sock, "OK\n", 3, 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
	}
               
}

