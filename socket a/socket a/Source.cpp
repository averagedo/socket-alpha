#include<iostream>
#include<stdio.h>
#include<string>
#include<WinSock2.h>
#include<thread>

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

int toInt(std::string a)
{
	int tong = 0;
	for (int i = 0; i < a.length(); i++)
	{
		tong = tong * 10 + (int)(a[i] - 48);
	}
	return tong;
}

void Process(SOCKET CliPro)
{
	char request_browser[1000] = { 0 };
	char Host[300] = { 0 };

	char header[3000] = { 0 };
	char body[5000] = { 0 };
	SOCKET ProSer = socket(AF_INET, SOCK_STREAM, 0);

	std::cout << "Nhan yeu cau:" << std::endl;

	memset(request_browser, 0, sizeof(request_browser));	//set lai request
	int v = recv(CliPro, request_browser, sizeof(request_browser), 0);
	if (v >= 1000)
	{
		std::cout << "Thieuuuuuuuuuuuuuuuuuuuuuu?" << std::endl;
		system("pause");
		return;
	}
	std::cout << request_browser << std::endl << std::endl;

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
		std::cout << Host << std::endl;
	}

	//phan giai ten mien
	sockaddr_in ConnectIP;
	hostent *ConnectPC = NULL;
	ConnectPC = gethostbyname(Host);  // Lay PC theo Tên Domain

	if (!ConnectPC) {
		std::cout << "DNS khong the phan giai duoc ten mien nay ...\n";
		//continue;
		return;
	}

	ConnectIP.sin_family = AF_INET;
	ConnectIP.sin_port = htons(80);

	ConnectIP.sin_addr.s_addr = (*(DWORD*)ConnectPC->h_addr_list[0]); // Lay IP

	std::cout << "May chu:";
	std::cout << ConnectPC->h_name << "\n";
	std::cout << "IP: " << inet_ntoa(ConnectIP.sin_addr) << "\n\n";

	//gui len server
	//tao 1 socket khac
	int TConnect = connect(ProSer, (sockaddr*)&ConnectIP, sizeof(ConnectIP));
	if (TConnect == -1)
	{
		std::cout << "Loi ket noi proxy server" << std::endl << std::endl;
		WSAGetLastError();
		//continue;
		//system("pause");
		return;
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
	send(ProSer, request_browser, v, 0);

	//Nhan du lieu
	//Header
	//memset(header, 0, sizeof(header));
	char c;
	std::string tem;
	int dem = 0;
	int tong = 0;
	int z = 0;
	while (1)
	{
		int r = recv(ProSer, &c, 1, 0);
		if (r < 0)
		{
			std::cout << "Loiiii !!!" << std::endl;
			return;
		}
		header[z] = c;
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
	}

	send(CliPro, header, z, 0);
	std::cout << "So byte co header:";
	std::cout << tong << std::endl;

	std::string Cont = "Content-Length: ";
	std::size_t found = tem.find(Cont);
	std::string so;
	int j = 0;
	while (tem[found + Cont.length() + j] != '\r')
	{
		so.push_back(tem[found + Cont.length() + j]);
		j++;
	}
	int conLength = toInt(so);

	int m = 1;
	tong = 0;
	char *BoDy;
	do
	{
		m = recv(ProSer, body, sizeof(body), 0);
		if (m == -1)
		{
			std::cout << "Nhan du lieu loi." << std::endl;
			return;
		}
		tong += m;
		BoDy = new char[m];
		memmove(BoDy, body, m);
		send(CliPro, BoDy, m, 0);
		delete[] BoDy;
		if (tong == conLength)
			break;
	} while (m > 0);
	std::cout << "So byte co body: ";
	std::cout << tong << std::endl << std::endl;

	closesocket(ProSer);
	closesocket(CliPro);
}

void main()
{
	WSADATA SData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &SData);
	if (iResult != 0)
	{
		std::cout << "Khong the khoi dong winsock";
		return;
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
		std::cout << "Loi thiet lap IP va Port." << std::endl;
		WSAGetLastError();
		closesocket(server);
		return;
	}

	//lang nghe
	int TListen = listen(server, 5);
	if (TListen == -1)
	{
		std::cout << "Loi lang nghe." << std::endl;
		WSAGetLastError();
		closesocket(server);
		return;
	}

	while (1)
	{
		SOCKET CliPro;
		sockaddr_in IP_CliPro;	//Luu dia chi va port cua server

		//Chap nhap ket noi
		int ip_CliPro;
		ip_CliPro = sizeof(IP_CliPro);
		CliPro = accept(server, (sockaddr*)&IP_CliPro, &ip_CliPro);

		if (CliPro == -1)
		{
			std::cout << "Loi ket noi" << std::endl;
			return;
		}
		else
			std::cout << "Ket noi thanh cong" << std::endl << std::endl;

		std::thread Thr1(Process, CliPro);
		Thr1.detach();

	}

	WSACleanup();
	system("pause");

	return;
}