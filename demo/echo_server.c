#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>


#define SERVER_PORT 666

int main(void) {

	int sock;
	struct sockaddr_in server_addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	// 清空标签，写上地址和端口号
	bzero(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET; // 选择协议族IPV4
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 监听本地所有IP地址
	server_addr.sin_port = htons(SERVER_PORT); //绑定端口号

	bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

	listen(sock, 128);

	printf("等待客户端的连接\n");

	int done = 1;

	while(done) {
		struct sockaddr_in client;
		int client_sock, len, i;
		char client_ip[64];
		char buf[256];

		socklen_t client_addr_len;
		client_addr_len = sizeof(client);
		client_sock = accept(sock, (struct sockaddr *)&client, &client_addr_len);

		printf("client ip:%s\t port:%d\n", inet_ntop(
				AF_INET, 
				&client.sin_addr.s_addr,
				client_ip, 
				sizeof(client_ip)
			),
			ntohs(client.sin_port)
		);

		// 读取客户端发送的数据
		len = read(client_sock, buf, sizeof(buf) - 1);
		buf[len] = '\0';
		printf("receive[%d]: %s\n", len, buf);

		// 转换成大写
		for(i = 0; i < len; i++) {
			buf[i] = toupper(buf[i]);
		}

		len = write(client_sock, buf, len);

		printf("finished. len: %d\n", len);
		close(client_sock);
	}
	close(sock);
	return 0;
}