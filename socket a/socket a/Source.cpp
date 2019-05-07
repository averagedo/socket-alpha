

#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>

#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_BUFLEN 1024

int toInt(std::string a)
{
	int tong = 0;
	for (int i = 0; i < a.length(); i++)
	{
		tong = tong * 10 + (int)(a[i] - 48);
	}
	return tong;
}

int CompareHost(char * a)
{
	char host[] = "Host: ";
	int j = 0, term = 0;
	char test[7];
	for (int i = 0; i < strlen(a); i++)
	{
		if (term == 1)
		{
			test[j] = a[i];
			if (j == 5)
			{
				test[j + 1] = '\0';
				if (strcmp(test, host) == 0)
				{
					return i;
				}
				j = -1;
				term = 0;
			}
			j++;
		}
		if (a[i] == '\n')
			term = 1;
	}
	return 0;
}

int BlockConnect(char a[])
{
	for (int i = 0; i < 4; i++)
	{
		if (a[i] == 'C' && a[i + 1] == 'O' && a[i + 2] == 'N')
			return 1;
	}
	return 0;
}

int process(SOCKET ClientSocket)
{
	char recvbuf[DEFAULT_BUFLEN] = { 0 };
	int recvbuflen = DEFAULT_BUFLEN;
	char Host[300] = { 0 };
	char Header[1000] = { 0 };
	char Body[5000] = { 0 };

	int iResult;

	addrinfo *result, hints;

	memset(recvbuf, 0, recvbuflen);
	int byteRecv;
	byteRecv = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (byteRecv > 0)
	{
		printf("Bytes received: %d\n", byteRecv);
		std::cout << recvbuf << std::endl;
	}
	else if (byteRecv == 0)
	{
		printf("Connection closing...\n");
		return 1;
	}
	else {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		system("pause");
		return 1;
	}

	int i = CompareHost(recvbuf);
	if (i != 0)
	{
		int j = 0;
		while (recvbuf[i] != '\r' && recvbuf[i] != ':')
		{
			Host[j] = recvbuf[i + 1];
			j++;
			i++;
		}
		Host[j - 1] = '\0';
		std::cout << "Host: " << Host << std::endl << std::endl;
	}

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_RECEIVE);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		system("pause");
		WSACleanup();
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(Host, "80", &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		system("pause");
		return 1;
	}

	SOCKET ConnectSocket;
	ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		system("pause");
		return 1;
	}

	// Connect to server.
	iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		//WSACleanup();
		//system("pause");
		return 1;
	}

	/*for (int i = 0; recvbuf[i] != '\0'; i++)
	{
		if (recvbuf[i] == 'I' && recvbuf[i + 1] == 'f')
		{
			int j;
			for (j = 0; recvbuf[i + j] != '\n'; j++)
			{
			}
			int k = 0;
			for (k = 0; recvbuf[i + k + j + 1] != '\0'; k++)
			{
				recvbuf[i + k] = recvbuf[i + k + j + 1];
			}
			recvbuf[i + k] = '\0';
			i--;
		}
	}*/

	iResult = send(ConnectSocket, recvbuf, byteRecv, 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		system("pause");
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		system("pause");
		return 1;
	}

	char c;
	std::string tem;
	int dem = 0;
	int tong = 0;
	int z = 0;
	memset(Header, 0, sizeof(Header));
	int r;
	do
	{
		r = recv(ConnectSocket, &c, 1, 0);
		if (r < 0)
		{
			std::cout << "Loiiii !!!" << std::endl;
			return 1;
		}

		Header[z] = c;
		std::cout << c;
		tem.push_back(c);
		tong++;
		z++;
		if (c == '\n' || c == '\r')
			dem++;
		else
			dem = 0;
		if (dem == 4)
			break;
	} while (r > 0);

	iResult = send(ClientSocket, Header, z, 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		system("pause");
		WSACleanup();
		return 1;
	}

	std::cout << "So byte co header:";
	std::cout << tong << std::endl;

	std::string Cont = "Content-Length: ";
	std::size_t found = tem.find(Cont);
	std::string so;
	int j = 0;
	int conLength = 0;
	if (found != tem.npos)
	{
		while (tem[found + Cont.length() + j] != '\r')
		{
			so.push_back(tem[found + Cont.length() + j]);
			j++;
		}
		conLength = toInt(so);
	}

	int m = 1, l;
	tong = 0;
	char *BoDy;
	do
	{
		m = recv(ConnectSocket, Body, sizeof(Body), 0);
		if (m == SOCKET_ERROR) {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			system("pause");
			WSACleanup();
			return 1;
		}
		tong += m;
		BoDy = new char[m];
		memmove(BoDy, Body, m);
		l = send(ClientSocket, BoDy, m, 0);
		if (l == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			system("pause");
			WSACleanup();
			return 1;
		}
		delete[] BoDy;
		if (tong == conLength)
			break;
		/*if (m < 5000)
			break;*/
	} while (m > 0);

	std::cout << "So byte co body: ";
	std::cout << tong << std::endl << std::endl;

	closesocket(ClientSocket);
	closesocket(ConnectSocket);
}

int main()
{
	WSADATA wsaDATA;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	addrinfo *result = NULL;
	addrinfo hints;

	int iResult;
	//Khoi tao winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaDATA);
	if (iResult != 0)
	{
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, "8888", &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	while (1)
	{
		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			system("pause");
			return 1;
		}

		process(ClientSocket);
		//std::thread Thr(process, ClientSocket);
		//Thr.detach();

		//Sleep(10);
	}
	// cleanup
	system("pause");

	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}