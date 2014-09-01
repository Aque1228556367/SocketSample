#include <winsock.h>
#include <stdio.h>

#define PORT       5150			// 服务器的监听端口，所有想要与之连接的客户端都必须指定这个服务器端口
#define MSGSIZE    1024

#pragma comment(lib, "ws2_32.lib")

int    g_iTotalConn = 0;
//SOCKET g_CliSocketArr[FD_SETSIZE];
SOCKET g_CliSocketArr[2];

DWORD WINAPI WorkerThread(LPVOID lpParameter);

int main()
{
	printf("======= The Server which use Select =======\n");
	// 服务端的 bind() 相当于客户端的 connect()，都是用一个socket对象关联一个 sockaddr 地址
	// 服务端 bind() 是用“监听socket对象”关联一个“本地sockaddr对象local”
	// 客户端 connect() 是用“客户端socket对象”关联一个“指定要连接的服务器的sockaddr对象server”

	WSADATA     wsaData;
	SOCKET      sListen, sClient;
	SOCKADDR_IN local, client;
	int         iaddrSize = sizeof(SOCKADDR_IN);
	DWORD       dwThreadId;

	// Initialize Windows socket library
	WSAStartup(0x0202, &wsaData);

	// Create listening socket
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Bind
	local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT);
	bind( sListen, (struct sockaddr *)&local, sizeof(SOCKADDR_IN) );
	printf( "The Server Address :  %s:%d", inet_ntoa(local.sin_addr), ntohs(local.sin_port) );

	// Listen
	listen(sListen, 3);

	// Create worker thread
	CreateThread(NULL, 0, WorkerThread, NULL, 0, &dwThreadId);  

	while (TRUE)
	{
		// Accept a connection
		sClient = accept(sListen, (struct sockaddr *)&client, &iaddrSize);
		printf("Accepted client:%s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

		// Add socket to g_CliSocketArr
		// 注：如果接收到的客户端连接超过 g_CliSocketArr 的大小，socket 连接还是可以建立的
		// 只是超过数组大小，线程不做处理。客户端发过去的消息收不回来
		g_CliSocketArr[g_iTotalConn++] = sClient;
	}

	return 0;
}

DWORD WINAPI WorkerThread(LPVOID lpParam)
{
	int            i;
	fd_set       fdread;			// 指向一组等待可读性检查的套接口
	int            ret;
	struct timeval tv = {1, 0};
	char           szMessage[MSGSIZE];

	while (TRUE)
	{
		FD_ZERO(&fdread);
		for (i = 0; i < g_iTotalConn; i++)
		{
			FD_SET(g_CliSocketArr[i], &fdread);
		}

		// We only care read event
		ret = select( 0, &fdread, NULL, NULL, &tv );

		if (ret == 0)
		{
			// Time expired
			continue;
		}

		for (i = 0; i < g_iTotalConn; i++)
		{
			if (FD_ISSET(g_CliSocketArr[i], &fdread))
			{
				// A read event happened on g_CliSocketArr
				ret = recv( g_CliSocketArr[i], szMessage, MSGSIZE, 0 );
				if (ret == 0 || (ret == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
				{
					// Client socket closed
					printf("Client socket %d closed.\n", g_CliSocketArr[i]);
					closesocket(g_CliSocketArr[i]);
					if (i < g_iTotalConn - 1)
					{            
						g_CliSocketArr[i--] = g_CliSocketArr[--g_iTotalConn];
					}
				}
				else
				{
					// We received a message from client
					szMessage[ret] = '\0';
					send(g_CliSocketArr[i], szMessage, strlen(szMessage), 0);
				}
			}
		}
	}

	return 0;
}