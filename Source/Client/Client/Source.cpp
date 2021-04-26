#include"CClient.h"
#include<fstream>
#include<string>
#include<stdlib.h>
void errexit(const char*, ...);

char pos[5] = ""; // Đây để lưu thông tin là Socket ở vị trí thứ mấy đang kết nối tới Server
char fileNameDownload[20]; //Tên file dùng để lưu file cần download để Server khỏi thông báo lại là cần download tên file đỡ mất công
char fileNameUpload[20]; // Tên file dùng để muốn báo là định upload file gì lưu ở toàn cục để sử dụng luôn
bool busy = false; // Busy là biến để biết là có Client nào đang upload hay không để Client khác biết mà được upload

void _exit()
{
	exit(0);
}

void _register(char tmp[], SOCKET& socket_descriptor)
{

	char message[100];
	char userName[20];
	char passWord[20];
	char registerReport[20] = "register";

	strcpy_s(message, tmp);
	// Nhập thông tin đăng kí 
	cout << message << endl;
	cout << "Please register\n";
	cout << "Username: ";
	cin >> userName;
	cout << "Password: ";
	cin >> passWord;
	//Gửi các thông tin đến server để check xem là có hợp lệ hay không
	send(socket_descriptor, registerReport, sizeof(registerReport), 0);
	Sleep(10);
	send(socket_descriptor, userName, sizeof(userName), 0);
	Sleep(10);
	send(socket_descriptor, passWord, sizeof(passWord), 0);
	strcpy_s(message, "");
}

void _registerAgain(char tmp[], SOCKET& socket_descriptor)
{
	//Nếu tài khoản không hợp lệ thì Server gửi tới thông báo đăng kí lại 
	char message[100];
	char userName[20];
	char passWord[20];
	char registerReport[20] = "register";

	strcpy_s(message, tmp);

	cout << message << endl;
	cout << "Please register again\n";
	cout << "Username: ";
	cin >> userName;
	cout << "Password: ";
	cin >> passWord;
	send(socket_descriptor, registerReport, sizeof(registerReport), 0);
	Sleep(10);
	send(socket_descriptor, userName, sizeof(userName), 0);
	Sleep(10);
	send(socket_descriptor, passWord, sizeof(passWord), 0);
	strcpy_s(message, "");
}

void _registerSuccess(char tmp[], SOCKET& socket_descriptor)
{
	//Hiển thị thông báo đăng kí đã thành công
	string message = tmp;
	pos[0] = message[message.size() - 1];
	message.erase(message.end() - 1);
	cout << message << endl;
	return;
}

void _welcome(char tmp[], SOCKET& socket_descriptor)
{
	//Hiển thị thông báo chào mừng một Client
	string message = tmp;
	cout << message << endl;
	return;
}

void _logOff(char tmp[], SOCKET& socket_descriptor)
{
	// Hiển thị thông báo một Client nào log off
	string message = tmp;
	cout << endl << message << endl;
	return;
}

void upload(char tmp[], SOCKET& socket_descriptor)
{
	string fileNameUpl = tmp;
	ifstream fileUpload;
	fileUpload.open(fileNameUpload, ios::binary);
	string content_;

	string content = "Upload:" + fileNameUpl + "~";
	content.append(content_);
	content[content.size()] = '\0';

	// Gửi tên file cần upload cho server
	send(socket_descriptor, content.c_str(), content.size() + 1, 0);

	Sleep(10);

	//Gửi thông tin của người upload
	send(socket_descriptor, pos, sizeof(pos), 0);

	fileUpload.seekg(0, ios::end);
	int sizeFile = fileUpload.tellg();
	fileUpload.seekg(0, ios::beg);

	string tmz = to_string(sizeFile);

	Sleep(10);

	// Gửi size file cần upload cho server 
	send(socket_descriptor, tmz.c_str(), tmz.size() + 1, 0);

	char* buffer = new char[sizeFile];
	fileUpload.read(buffer, sizeFile);


	//Bắt đầu gửi cho tới khi sizeFile = 0
	while (sizeFile > 0)
	{
		int sentLength = send(socket_descriptor, buffer, sizeFile, 0);
		if (sentLength < 1)
		{
			cout << "Error" << endl;
			return;
		}
		sizeFile -= sentLength;
		buffer += sentLength;
	}

	cout << "\nUpload successful!!\n";

	return;
}

void _displayListFile(char tmp[], SOCKET& socket_descriptor)
{
	//Hiển thị các File có ở Server
	string listFile = tmp;
	listFile.erase(listFile.begin(), listFile.begin() + 9);
	cout << endl << listFile << endl;
	return;
}




void handle(SOCKET socket_descriptor, char message1[])
{
	// Đây là hàm xử lí tổng bộ của Client được tạo ra chạy bằng các thread khác nhau để có thể chạy các update tình hình liên tục từ server và các client khác
	char message[8192];
	strcpy_s(message, message1);
	bool _continue = false;

	char logOffReport[20] = "off";

	if (strcmp(message, "Hello client,you connected to server") == 0)
	{
		_register(message, socket_descriptor);
		return;
	}

	else if (strcmp(message, "Register Failed") == 0)
	{
		_registerAgain(message, socket_descriptor);
		return;
	}

	else if (message[0] == 'R' && message[1] == 'e' && message[9] == 'S')
	{
		_registerSuccess(message, socket_descriptor);
		_continue = true;
	}

	else if (message[0] == 'W' && message[1] == 'e')
	{
		_welcome(message, socket_descriptor);
		return;
	}


	else if (message[strlen(message) - 1] == 'f' && message[strlen(message) - 2] == 'f' && message[strlen(message) - 3] == 'o')
	{
		_logOff(message, socket_descriptor);
		return;
	}



	else if (message[0] == '*' && message[1] == 'L' && message[2] == 'i')
	{
		_displayListFile(message, socket_descriptor);
		return;
	}


	else if (message[0] == '*' && message[1] == 'F' && message[2] == 'r')
	{
		busy = false;
		return;
	}


	else if (message[0] == '*' && message[1] == 'B' && message[2] == 'u')
	{
		busy = true;
		return;
	}



	strcpy_s(message, "");

	Sleep(2000);

	string keyPressed;
	while (true)
	{
		if (_continue == true)
		{
			_continue = false;
			cout << "\n----------------------------MENU----------------------------";
			cout << "\nType 1 ->Downloadfile\nType 2 ->Uploadfile\nType 3 ->Stop\n";
			cout << "Please type what you want\n";
			cin >> keyPressed;
			if (keyPressed == "1")
			{
				char download[50] = "Download:";
				cout << "type the name of file you want to download\n";
				cin >> fileNameDownload;
				strcat_s(download, fileNameDownload);
				send(socket_descriptor, download, sizeof(download), 0);
				Sleep(10);
				send(socket_descriptor, pos, sizeof(pos), 0);
				Sleep(10);
				_continue = true;
			}
			else if (keyPressed == "2")
			{
				if (busy)
				{
					cout << endl << "Now another client is uploading so can't upload together\n";
					_continue = true;
				}
				else
				{
					cout << "type the name of file you want to upload\n";
					cin >> fileNameUpload;
					if (busy)
					{
						cout << endl << "Now another client is uploading so can't upload together\n";
					}
					else
					{
						upload(fileNameUpload, socket_descriptor);
					}
					_continue = true;
				}
			}
			else if (keyPressed == "3")
			{
				cout << "Stop";
				strcat_s(logOffReport, pos);
				send(socket_descriptor, logOffReport, sizeof(logOffReport), 0);
				closesocket(socket_descriptor);
				_exit();
				return;
			}
			else
			{
				cout << "Select failed - Please select again\n";
				_continue = true;
			}
			strcpy_s(message, "");
		}
		Sleep(10);
	}

}



int _tmain(int argc, _TCHAR* argv[])
{
	int retcode;
	CClient main;
	if (main._connect() == true)
	{
		SOCKET socket_descriptor = main.getMainSocket();
		char message[8192];
		char userName[20];
		char pos[5] = "";
		char passWord[20];
		int count = 0;
		thread a[1000];
		while (true)
		{
			//Khi nhận thông tin từ Server thì nó bắt đầu xử lí
			if (recv(socket_descriptor, message, SO_RCVBUF, 0))
			{
				//Đây là quá trình download file từ server
				if (message[0] == 'D')
				{
					char temp[4096];
					ofstream downloadFile;
					downloadFile.open(fileNameDownload, ios::binary);

					recv(socket_descriptor, temp, 4096, 0);		//Nhận size file của dữ liệu cần download
					string fileLengthStr = temp;

					int fileLength = stoi(fileLengthStr);

					char* buffer = new char[fileLength];

					//Bắt đầu nhận dữ liệu cho đến khi dữ liệu đã nhận bằng 0
					while (fileLength > 0)
					{
						int recvLength = recv(socket_descriptor, buffer, fileLength, 0);
						if (recvLength < 1)
						{
							break;
						}
						downloadFile.write(buffer, recvLength);
						fileLength -= recvLength;
						buffer += recvLength;
					}

					cout << "\nDownload file " << fileNameDownload << " successfull!!" << endl;

				}
				else
				{
					// Đây là mấu chốt của chương trình mỗi khi có một thông báo từ Server thì nó bắt đầu tạo ra tiểu trình và thực thi chạy
					a[count] = thread(&handle, socket_descriptor, message);
					a[count].detach();
					count++;
					Sleep(10);
				}

				if (message[0] == 'E' && message[1] == 'x')
				{
					cout << "\nServer died ----  Stop connected\n";
					closesocket(socket_descriptor);
					return 0;
				}
			}
			strcpy_s(message, "");
		}
	}

	return 0;
}

