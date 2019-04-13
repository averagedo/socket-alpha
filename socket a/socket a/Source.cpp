#include<iostream>
#include<stdio.h>
#include<WinSock2.h>
using namespace std;

#pragma comment (lib,"ws2_32.lib")

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


int main()
{
	WSADATA SData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &SData);
	if (iResult != 0)
	{
		cout << "Khong the khoi dong winsock";
		return 1;
	}

	//khoi tao sock ket
	SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;	//IP cua mink
	addr.sin_port = htons(1234);

	//cho doi
	int TBind = bind(server, (sockaddr*)&addr, sizeof(addr));
	if (TBind == -1)
	{
		cout << "Loi thiet lap IP va Port." << endl;
		WSAGetLastError();
		closesocket(server);
		return 1;
	}

	//lang nghe
	int TListen = listen(server, 5);
	if (TListen == -1)
	{
		cout << "Loi lang nghe." << endl;
		WSAGetLastError();
		closesocket(server);
		return 1;
	}

	SOCKET CliPro;

	sockaddr_in IP_CliPro;	//Luu dia chi va port cua server
	char *BoDy;

	while (1)
	{

		char request_browser[1000];
		char Host[300] = { 0 };

		char header[5000] = { 0 };
		char body[5000] = { 0 };
		SOCKET ProSer = socket(AF_INET, SOCK_STREAM, 0);

		//Chap nhap ket noi
		int ip_CliPro;
		ip_CliPro = sizeof(IP_CliPro);
		CliPro = accept(server, (sockaddr*)&IP_CliPro, &ip_CliPro);

		if (CliPro == -1)
		{
			cout << "Loi ket noi" << endl;
			return 1;
		}
		else
			cout << "Ket noi thanh cong" << endl << endl;

		cout << "Nhan yeu cau:" << endl;

		memset(request_browser, 0, sizeof(request_browser));	//set lai request
		recv(CliPro, request_browser, sizeof(request_browser), 0);
		cout << request_browser << endl << endl;

		int i = CompareHost(request_browser);
		if (i != 0)
		{
			int j = 0;
			while (request_browser[i] != '\r' && request_browser[i] != ':')
			{
				Host[j] = request_browser[i + 1];
				j++;
				i++;
			}
			Host[j - 1] = '\0';
			cout << Host << endl;
		}

		//phan giai ten mien
		sockaddr_in ConnectIP;
		hostent *ConnectPC = NULL;
		ConnectPC = gethostbyname(Host);  // Lay PC theo Tên Domain

		if (!ConnectPC) {
			cout << "DNS khong the phan giai duoc ten mien nay ...\n";
			continue;
		}

		ConnectIP.sin_family = AF_INET;
		ConnectIP.sin_port = htons(80);

		ConnectIP.sin_addr.s_addr = (*(DWORD*)ConnectPC->h_addr_list[0]); // Lay IP

		cout << "May chu:";
		cout << ConnectPC->h_name << "\n";
		cout << "IP: " << inet_ntoa(ConnectIP.sin_addr) << "\n\n";

		//gui len server
		//tao 1 socket khac
		int TConnect = connect(ProSer, (sockaddr*)&ConnectIP, sizeof(ConnectIP));
		if (TConnect == -1)
		{
			cout << "Loi ket noi proxy server" << endl;
			WSAGetLastError();
			//continue;
			system("pause");
			return 1;
		}

		for (int i = 0; request_browser[i] != '\0'; i++)
		{
			if (request_browser[i] == 'I' && request_browser[i + 1] == 'f')
			{
				int j;
				for (j = 0; request_browser[i + j] != '\n'; j++)
				{
				}
				int k = 0;
				for (k = 0; request_browser[i + k + j + 1] != '\0'; k++)
				{
					request_browser[i + k] = request_browser[i + k + j + 1];
				}
				request_browser[i + k] = '\0';
				i--;
			}
		}

		//Gui du lieu
		send(ProSer, request_browser, sizeof(request_browser), 0);

		//Nhan du lieu
		//Header
		memset(header, 0, sizeof(header));
		recv(ProSer, header, sizeof(header), 0);



		cout << header << endl;
		char* BoDy;
		//body
		int m = 1, n = 0;

		send(CliPro, header, sizeof(header), 0);

		do
		{
			m = recv(ProSer, body, sizeof(body), 0);
			if (m == -1)
			{
				cout << "Nhan du lieu loi." << endl;
				break;
			}
			BoDy = new char[m];
			memmove(BoDy, body, m);
			send(CliPro, BoDy, m, 0);
			delete[] BoDy;
		} while (m > 0);

		closesocket(CliPro);
		//closesocket(ProSer);

	}

	WSACleanup();
	system("pause");


	return 0;

}