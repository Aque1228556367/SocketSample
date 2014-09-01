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
	// �ͻ��˵� connect() �൱�ڷ���˵� bind()��������һ��socket�������һ�� sockaddr ��ַ
	// ����� bind() ���á�����socket���󡱹���һ��������sockaddr����local��
	// �ͻ��� connect() ���á��ͻ���socket���󡱹���һ����ָ��Ҫ���ӵķ�������sockaddr����server��

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

	// ˵����
	// �ͻ���Ҫ��û����bind��bind�˿�����Ϊ0
	//	��connectʱ��ϵͳ�Զ�ѡ��һ��û��ռ�õ���ʱ�˿�
	// ���󶨿ͻ��˶˿ڵ��ѱ�ռ�ã����������һ���µĶ˿�

	// Ϊ�����顰�ͻ���Ҳһ�����԰�socket��
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