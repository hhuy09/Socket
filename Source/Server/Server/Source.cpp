#include"CServer.h"
#include <cstring>
void errexit(const char* format, ...);
#include"Windows.h"
#include<stdlib.h>

struct informationClient
{
	char userName[20];
	char passWord[20];
	int pos;
};

vector <SOCKET>  new_Socket;  // Lưu những socket liên lạc với các Clients
vector<informationClient> infC; // Lưu thông tin tên và mật khẩu đã đăng kí thành công của các Client
vector<string> fileName; // Lưu những file có ở server

// hàm kết thúc chương trình
void _exit()
{
	exit(0);
}

void updateServer()
{
	//Thông báo các Client đã kết nối đến Server
	cout << "Now the server has " << new_Socket.size() << " connected" << endl;
	for (int i = 0; i < new_Socket.size(); i++)
	{
		cout << "Client " << infC[i].pos + 1 << ": " << infC[i].userName << endl;
	}
	//Thông báo các File đang có trên Server
	if (fileName.size() < 2)
	{
		cout << "Now the server has " << fileName.size() << " file" << endl;
		for (int i = 0; i < fileName.size(); i++)
		{
			cout << "File " << i + 1 << " : " << fileName[i] << endl;
		}
	}
	else
	{
		cout << "Now the server has " << fileName.size() << " files" << endl;
		for (int i = 0; i < fileName.size(); i++)
		{
			cout << "File " << i + 1 << " : " << fileName[i] << endl;
		}
	}

	return;
}

void _displayListFile(int pos)
{
	// Thông báo các file hiện có ở server khi mới đăng kí thành công
	string listFile = "*Listfile";
	listFile.append("Now the server has ");
	if (fileName.size() == 1)
	{
		listFile.append("1 file:\n");
	}
	else if (fileName.size() == 0)
	{
		listFile.append("0 file:\n");
	}
	else
	{
		listFile.append(to_string(fileName.size()));
		listFile.append(" files:\n");
	}
	for (int i = 0; i < fileName.size(); i++)
	{
		string tmp = fileName[i] + "\n";
		listFile.append(tmp);
	}
	char* x = new char[listFile.size() + 1];
	strcpy_s(x, listFile.size() + 1, listFile.c_str());
	send(new_Socket[pos], x, listFile.size() + 1, 0);

}

void _busyUpload()
{
	//Gửi đến các Client khác để nhận biết là có một Client đang upload nên không thể upload được
	char busy[10] = "*Busy";
	for (int i = 0; i < new_Socket.size(); i++)
	{
		send(new_Socket[i], busy, sizeof(busy), 0);
	}
	return;
}

void _freeUpload()
{
	//Gửi đến các Client khác đã upload xong và các clients còn lại có thể upload
	char free[10] = "*Free";
	for (int i = 0; i < new_Socket.size(); i++)
	{
		send(new_Socket[i], free, sizeof(free), 0);
	}
	return;
}

bool checkAccount(string userName, string passWord, string fileName)
{
	// Kiểm tra tài khoản được Client gửi lên đã trùng hay chưa
	fstream accountClients;
	accountClients.open(fileName, ios::in | ios::out | ios_base::app);
	while (accountClients.eof() == false)
	{
		string a, x;
		getline(accountClients, a);
		if (a == "") break;
		for (int i = 0; i < a.size(); i++)
		{
			if (a[i] == ' ')
			{
				x = a.substr(0, i);
				break;
			}
		}
		if (userName == x) return false;
	}
	accountClients.close();
	return true;
}

void _register(SOCKET& socket_connect, char tmp[], int pos)
{
	// Đăng kí tài khoản 
	string fileName = "accountClients.txt";
	char message[50];
	char username[20], password[20];
	char reportRegisterAgian[50] = "Register Failed";
	char reportRegisterSuccess[50] = "Register Successfull";
	char reportAllWelcome[50] = "Welcome:";

	// Nhận Username được đăng kí từ Client
	recv(socket_connect, username, sizeof(username), 0);
	// Nhận Password được đăng kí từ Client
	recv(socket_connect, password, sizeof(password), 0);

	// Bắt đầu đưa vào hàm kiểm tra tài khoản đăng kí có hợp lệ không
	if (checkAccount(username, password, fileName) == true)
	{
		fstream accountClients;
		accountClients.open(fileName, ios::in | ios::out | ios_base::app);
		// Khi hợp lệ thì ta ghi Tài Khoản vừa được tạo vào File lưu trữ các tài khoản đã được đăng kí
		accountClients << username << " " << password << endl;
		accountClients.close();

		//Lưu trữ thông tin tài khoản
		informationClient k;
		strcpy_s(k.userName, username);
		strcpy_s(k.passWord, password);
		k.pos = new_Socket.size(); // Vị trí thứ tự của nó được tạo ra của các Client

		reportRegisterSuccess[strlen(reportRegisterSuccess)] = 48 + k.pos;
		//Gửi thông báo tài khoản đã đăng kí thành công
		send(socket_connect, reportRegisterSuccess, sizeof(reportRegisterSuccess), 0);

		new_Socket.push_back(socket_connect); // Thêm socket với Client tương ứng vào vector để tí nữa gửi tất cả thì lấy ra sử dụng
		infC.push_back(k); // Lưu thông tin tương ứng của Client vào vector

		_displayListFile(k.pos); //Gửi đến Client vừa đăng kí thành công các File hiện tại đang có 

		char tmp[20];
		char pos1[50];

		cout << endl << username << " log in\n";

		strcat_s(reportAllWelcome, k.userName);

		Sleep(50);

		// Gửi đến tất cả các Client là có vừa Client đăng kí thành công
		for (int i = 0; i < new_Socket.size(); i++)
		{
			send(new_Socket[i], reportAllWelcome, sizeof(reportAllWelcome), 0);
		}
		updateServer();

	}
	else
	{
		//Nếu đăng kí không thành công thì gửi thông báo đăng kí lại
		send(socket_connect, reportRegisterAgian, sizeof(reportRegisterAgian), 0);
	}
}



void _logOff(SOCKET& socket_connect, char tmp[])
{
	char message[50];
	strcpy_s(message, tmp);
	char reportAllLogoff[50] = "";
	int a = message[3] - 48;
	cout << endl << infC[a].userName << " log out" << endl;
	for (int i = 0; i < new_Socket.size(); i++)
	{
		// Tìm kiếm Client log out để xóa ra khỏi các vector lưu trữ thông tin và socket kết nối
		if (infC[i].pos == a)
		{
			strcpy_s(reportAllLogoff, infC[i].userName);
			reportAllLogoff[strlen(reportAllLogoff)] = '\0';

			strcat_s(reportAllLogoff, " log off");

			new_Socket.erase(new_Socket.begin() + i);
			infC.erase(infC.begin() + i);
			break;
		}
	}
	for (int i = 0; i < new_Socket.size(); i++)
	{
		//Gửi đến tất cả các Client khác là có Client vừa log out
		send(new_Socket[i], reportAllLogoff, sizeof(reportAllLogoff), 0);
		Sleep(10);
	}
	// Khi Client thoát thì cập nhật lại các Client đang kết nối
	updateServer();
}




void _download(SOCKET& socket_connect, char tmp[])
{
	char add[5];
	//Nhận thông tin của client download
	recv(socket_connect, add, sizeof(add), 0);
	int _add = stoi(add);
	string userDownload = infC[_add].userName;

	send(socket_connect, "Do", 2, 0);
	string fileName = tmp;
	fileName.erase(fileName.begin(), fileName.begin() + fileName.find(':') + 1);

	ifstream fileNameDownload;
	fileNameDownload.open(fileName, ios::binary);

	fileNameDownload.seekg(0, ios::end);
	int sizeFile = fileNameDownload.tellg();
	fileNameDownload.seekg(0, ios::beg);

	string tmz = to_string(sizeFile);

	Sleep(10);

	send(socket_connect, tmz.c_str(), tmz.size() + 1, 0); // Gửi size mà client cần download

	char* buffer = new char[sizeFile];
	fileNameDownload.read(buffer, sizeFile);

	while (sizeFile > 0)
	{
		int sentLength = send(socket_connect, buffer, sizeFile, 0);
		if (sentLength < 1)
		{
			cout << "Error" << endl;
			return;
		}
		sizeFile -= sentLength;
		buffer += sentLength;
	}

	cout << endl << "Client " << userDownload << " download " << fileName << " successfull" << endl << endl;


	return;
}


void _displayListFileAll()
{
	//Thông báo các file hiện có đến tất cả Clients
	string listFile = "*Listfile";
	listFile.append("Now the server has ");
	if (fileName.size() == 1)
	{
		listFile.append("1 file:\n");
	}
	if (fileName.size() == 0)
	{
		listFile.append("0 file:\n");
	}
	else
	{
		listFile.append(to_string(fileName.size()));
		listFile.append(" files:\n");
	}
	for (int i = 0; i < fileName.size(); i++)
	{
		string tmp = fileName[i] + "\n";
		listFile.append(tmp);
	}

	char* x = new char[listFile.size() + 1];
	strcpy_s(x, listFile.size() + 1, listFile.c_str());

	for (int i = 0; i < new_Socket.size(); i++)
	{
		send(new_Socket[i], x, listFile.size() + 1, 0);
	}
}

void _upload(SOCKET& socket_connect, char tmp[])
{

	char temp[4096];
	char add[5];
	string content = tmp;
	content.erase(content.begin(), content.begin() + 7);
	string fileUploadName = content.substr(0, content.find('~'));
	content.erase(content.begin(), content.begin() + 1 + content.find('~'));

	recv(socket_connect, add, sizeof(add), 0);
	int _add = stoi(add);
	string UserUpload = infC[_add].userName;

	ofstream uploadFile;
	uploadFile.open(fileUploadName, ios::binary);

	recv(socket_connect, temp, 4096, 0);		//Nhận size của dữ liệu định upload
	string fileLengthStr = temp;

	int fileLength = stoi(fileLengthStr);

	char* buffer = new char[fileLength];

	while (fileLength > 0)
	{
		int recvLength = recv(socket_connect, buffer, fileLength, 0);
		if (recvLength < 1)
		{
			break;
		}
		uploadFile.write(buffer, recvLength);
		fileLength -= recvLength;
		buffer += recvLength;
	}

	fileName.push_back(fileUploadName);
	cout << "\n" << "Client " << UserUpload << " uploaded file " << fileUploadName << " successfull!" << endl << endl;

	fstream _fileNamee;
	_fileNamee.open("listFile.txt", ios::in | ios::out | ios_base::app);
	_fileNamee << endl << fileUploadName;

	// Khi upload xong thì update lại server có những File mới nào
	updateServer();

	//Các client khác bắt đầu được upload file
	_freeUpload();

	Sleep(100);
	//Hiển thị file mới đến các Client
	_displayListFileAll();
	uploadFile.close();

}


void Exit()
{
	while (1)
	{
		//Người quản lí server có thể đóng server bằng việc nhấn phím "SHIFT"
		if (GetKeyState(VK_SHIFT) & 0x8000)
		{
			char __exit[10] = "Exit";
			for (int i = 0; i < new_Socket.size(); i++)
			{
				//Thông báo đến tất cả các Client là Server đã chết
				send(new_Socket[i], __exit, sizeof(__exit), 0);
				closesocket(new_Socket[i]);
			}
			cout << "\n---------------------------- SERVER DIED ----------------------------\n";
			_exit();
			return;
		}
		Sleep(100);
	}

}


void handleConnect(SOCKET socket_connect, int pos, string fileName)
{
	//Tạo một tiểu trình để bắt sự kiện nhấn shift để kết thúc server
	thread b(&Exit);
	b.detach();
	Sleep(10);
	//Đây là hàm xử lí chính cho từng Client với mỗi Client thì xử lí tương ứng với 1 thread được tạo ra
	cout << "\n-------Press 'Shift' To Close Server-------\n";
	char reportHello[50] = "Hello client,you connected to server";
	char reportRegisterSuccess[50] = "Register Successfull";
	char reportRegisterAgian[50] = "Register Failed";
	char reportAllLogoff[50] = "";
	char reportAllWelcome[50] = "Welcome:";
	char message[8192];
	send(socket_connect, reportHello, sizeof(reportHello), 0);
	while (1)
	{
		if (recv(socket_connect, message, sizeof(message), 0))
		{
			if (strcmp(message, "register") == 0)
			{
				_register(socket_connect, message, pos);

			}
			else if (message[0] == 'o' && message[1] == 'f' && message[2] == 'f')
			{
				_logOff(socket_connect, message);
			}
			else if (message[0] == 'D' && message[1] == 'o' && message[2] == 'w')
			{
				_download(socket_connect, message);
			}
			else if (message[0] == 'U' && message[1] == 'p' && message[2] == 'l')
			{
				_busyUpload();
				_upload(socket_connect, message);
			}
			strcpy_s(message, "");
		}

	}

}

int _tmain(int argc, TCHAR* argv[])
{

	fstream _fileNamee;
	_fileNamee.open("listFile.txt", ios::in | ios::out | ios_base::app);
	while (_fileNamee.eof() == false)
	{
		string tmp;
		getline(_fileNamee, tmp);
		fileName.push_back(tmp);
	}

	_fileNamee.close();

	string fileName_ = "accountClients.txt";


	printf("accept()\n");
	int count = 0;
	thread a[1000];
	int new_addrlen;
	CServer main; // Khi khởi tạo main thì các hàm tự khởi tạo ra socket và hiện tại nó đã đang lắng nghe các Clients
	SOCKET socket_descriptor = main.getMainSocket(); // Lấy socket chủ để bắt đầu chấp nhận các Client tới
	while (true)
	{
		SOCKET new_socket = socket(PF_INET, SOCK_STREAM, 0);
		struct sockaddr_in new_sin;
		struct informationClient inf;
		new_addrlen = sizeof(new_sin);
		new_socket = accept(socket_descriptor, (struct sockaddr*)&new_sin, &new_addrlen);
		if (new_socket == INVALID_SOCKET)
			errexit("Accept failed: %d\n", WSAGetLastError());
		else
		{
			// Khi kết nối được với một Client thì ta bắt đầu tạo thread đi vào hàm xử lí chính cho các Client
			a[count] = thread(&handleConnect, new_socket, count, fileName_);
			a[count].detach();
			count++;
			Sleep(300);

		}
	}

	return 0;
}

