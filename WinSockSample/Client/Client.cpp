#include <WINSOCK2.H>
#include <stdio.h>

//#define SERVER_ADDRESS "137.117.2.148"
#define SERVER_ADDRESS "192.168.2.236"
#define PORT           5150
#define MSGSIZE        1024

#pragma comment(lib, "ws2_32.lib")

int main()
{
	printf("======= The Client =======\n");
	// 客户端的 connect() 相当于服务端的 bind()，都是用一个socket对象关联一个 sockaddr 地址
	// 服务端 bind() 是用“监听socket对象”关联一个“本地sockaddr对象local”
	// 客户端 connect() 是用“客户端socket对象”关联一个“指定要连接的服务器的sockaddr对象server”

	WSADATA     wsaData;
	SOCKET      sClient;
	SOCKADDR_IN server;
	char        szMessage[MSGSIZE];
	int         ret;

	WORD wVersionRequested = MAKEWORD( 2, 2 );
	// Initialize Windows socket library
	//WSAStartup(0x0202, &wsaData);
	WSAStartup(0x0202, &wsaData);

	// Create client socket
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Connect to server
	memset( &server, 0, sizeof(SOCKADDR_IN) );
	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDRESS);
	server.sin_port = htons(PORT);

	// 说明：
	// 客户端要是没调用bind或bind端口设置为0
	//	则connect时，系统自动选择一个没有占用的临时端口
	// 若绑定客户端端口的已被占用，则随机分配一个新的端口

	// 为了试验“客户端也一样可以绑定socket”
	SOCKADDR_IN local;
	memset( &local, 0, sizeof(SOCKADDR_IN) );
	local.sin_family = AF_INET;
	local.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDRESS);
	local.sin_port = htons(10086);
	ret = bind( sClient, (struct sockaddr *)&local, sizeof(SOCKADDR_IN) );
	if ( ret != 0 )
	{
		DWORD tLastError = WSAGetLastError();
	}

	printf("Connect To Server : %s ......\n", SERVER_ADDRESS);
	ret = connect( sClient, (struct sockaddr *)&server, sizeof(SOCKADDR_IN) );
	if ( ret != 0 )
	{
		DWORD tLastError = GetLastError();
		printf("Connect Server Failed ! The Error Code is : %d\n\n", tLastError);
		printf("The Failed Reason : \n");
		switch( tLastError )
		{
		case WSAETIMEDOUT:
			{
				printf("The Connection is time out .");
				break;
			}
		case WSAECONNREFUSED:
			{
				printf("The Connection is refused .");
				break;
			}
		default:
		    break;
		}
		printf("\n\n");
		return 0;
	}

	while (TRUE)
	{
		printf("Send:");
		gets(szMessage);

		// Send message
		send(sClient, szMessage, strlen(szMessage), 0);

		// Receive message
		ret = recv(sClient, szMessage, MSGSIZE, 0);
		szMessage[ret] = '\0';

		printf("Received [%d bytes]: '%s'\n", ret, szMessage);
	}

	// Clean up
	closesocket(sClient);
	WSACleanup();
	return 0;
}