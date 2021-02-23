#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <winuser.h>
#include <Windows.h>
#include <conio.h>
#include <iomanip>
#include "User.h"

#pragma comment(lib, "ws2_32.lib") //Winsock Library
#pragma comment(lib, "user32.lib")

int initConnection(SOCKET&);
void menu();
void menu2();
void login();
void registerUser();
void sendPrintR();

//Utility functions
std::string splitOn(std::string, int);
bool is_digits(const std::string&);

std::string IP = "127.0.0.1";
SOCKET sock;
User returningUser;

int main() { //NetPrint
	//Connect to server
	/*
	std::cout << "Initializing Connection";
	while (initConnection(sock)) { //Keep retrying till we establish a connection
		std::cout << "." << std::flush;
		Sleep(500);
		std::cout << "." << std::flush;
		Sleep(500);
		std::cout << "." << std::flush;
		Sleep(500);
		std::cout << "\b\b\b   \b\b\b" << std::flush;
	}
	std::cout << "Connected Successfully.\n";*/

	//Launch NetPrint Menu
	menu();
	return 0;
}

void menu() {

	std::string logo = R"(
 /$$   /$$             /$$     /$$$$$$$           /$$             /$$    
| $$$ | $$            | $$    | $$__  $$         |__/            | $$    
| $$$$| $$  /$$$$$$  /$$$$$$  | $$  \ $$ /$$$$$$  /$$ /$$$$$$$  /$$$$$$  
| $$ $$ $$ /$$__  $$|_  $$_/  | $$$$$$$//$$__  $$| $$| $$__  $$|_  $$_/  
| $$  $$$$| $$$$$$$$  | $$    | $$____/| $$  \__/| $$| $$  \ $$  | $$    
| $$\  $$$| $$_____/  | $$ /$$| $$     | $$      | $$| $$  | $$  | $$ /$$
| $$ \  $$|  $$$$$$$  |  $$$$/| $$     | $$      | $$| $$  | $$  |  $$$$/
|__/  \__/ \_______/   \___/  |__/     |__/      |__/|__/  |__/   \___/  
                                                                         
                                                                         
                                                                                                              
)";

	std::cout << logo << std::endl;

	char key='0';
	do {
		std::cout << "Options:\n"
			<< std::setw(12) << "- Login\n"
			<< std::setw(11) << "- Quit\n"
			<< "\nChoose an option > ";
		std::cin >> key;

		system("cls");

		//key = _getch();
		if (toupper(key) == 'L') {
			std::cout << "[+] Initializing Connection";
			while (initConnection(sock)) { //Keep retrying till we establish a connection
				std::cout << "." << std::flush;
				Sleep(500);
				std::cout << "." << std::flush;
				Sleep(500);
				std::cout << "." << std::flush;
				Sleep(500);
				std::cout << "\b\b\b   \b\b\b" << std::flush;
			}
			std::cout << "\n[+] Connected Successfully.\n";
			Sleep(1500);
			login();
			break;
		}
		if (toupper(key) == 'Q') {
			break;
		}
		
		//std::cout << "111111111111111111111111111111111\n";
	} while (toupper(key) != 'L' && toupper(key) != 'Q');
}

void login() {
	system("cls");

	std::string tmp_id;
	std::string tmp_pass;

	std::cout << "Login\n\n";
	
	do {
		std::cout << "ID (numeric) > ";
		std::cin >> tmp_id;

		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');
	} while (!is_digits(tmp_id));
	returningUser.setId(std::stoi(tmp_id));
	
	do {
		std::cout << "Password (Between 8 and 16 characters) > ";
		std::cin >> tmp_pass;

		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');
	} while (tmp_pass.length() < 8 || tmp_pass.length() > 16); //If not between 8 and 16 characters
	returningUser.setPassword(tmp_pass);
	
	//Send login request
	std::string tmp_str = "LGIN-" + tmp_id + '-' + tmp_pass;
	char* packet = new char[tmp_str.size() + 1];
	std::copy(tmp_str.begin(), tmp_str.end(), packet);
	packet[tmp_str.size()] = '\0';

	send(sock, packet, tmp_str.size() + 1, 0);
	//std::cout << "Sent Packet" << tmp_str << std::endl;

	delete[] packet;

	//Wait for request
	//std::cout << "Waiting for reply" << std::endl;
	char reply[2000];
	int recv_size = recv(sock, reply, sizeof(reply), 0);

	//Check if login is successful
	//AKIN-[Success Code]-[Privilege Code]
	std::string repStr = std::string(reply);

	//std::cout << repStr << std::endl;
	//std::cout << splitOn(repStr, 0) << std::endl;
	//std::cout << splitOn(repStr, 1) << std::endl;
	//std::cout << splitOn(repStr, 2) << std::endl;
	if (splitOn(repStr, 0) == "AKIN") {
		if (splitOn(repStr, 1) == "0") {
			//Set privilege
			returningUser.setPrivLevel(std::stoi(splitOn(repStr, 2)));
			//Launch menu2
			menu2();
		}
		else {
			std::cout << "\n[-] Error Logging in" << std::endl;
			Sleep(1000);
			login();
		}
	}
}


void menu2() {
	short privLevel = returningUser.getPrivLevel();
	char key;
	do{
		switch (privLevel) {
		case 1:
			system("cls");
			std::cout << "[+] Limited User: Priv-Level 1\n\n"
				<< "Options:\n"
				<< std::setw(13) << "- Logout\n" << std::endl;
			break;
		case 2:
			system("cls");
			std::cout << "[+] Normal User: Priv-Level 2\n\n"
				<< "Options:\n"
				<< std::setw(12) << "- Print\n"
				<< std::setw(13) << "- Logout\n";
			break;
		case 3:
			system("cls");
			std::cout << "[+] Admin: Priv-Level 3\n\n"
				<< "Options: \n"
				<< std::setw(12) << "- Print\n"
				<< std::setw(15) << "- Register\n"
				<< std::setw(13) << "- Logout\n";
			break;
		}

		std::cout << "\nChoose an option > ";
		std::cin >> key;
		
		key = toupper(key);
		
	} while (key != 'P' && key != 'R' && key != 'L');

	system("cls");
	if (key == 'P' && (privLevel == 2 || privLevel == 3)) {
		//print
		sendPrintR();
		Sleep(2000);
		system("cls");
		menu2();
	}
	if (key == 'R' && privLevel == 3) {
		//register
		registerUser();
		Sleep(2000);
		system("cls");
		menu2();
	}
	if (key == 'L') {
		//logout
		std::string tmp_str = "LGOT-";
		char* packet = new char[tmp_str.size() + 1];
		std::copy(tmp_str.begin(), tmp_str.end(), packet);
		packet[tmp_str.size()] = '\0';
		send(sock, packet, tmp_str.size() + 1, 0);
		std::cout << "[+] Logging out..." << std::endl;
		delete[] packet;
		closesocket(sock);
		WSACleanup();
		returningUser = User();
		Sleep(1000);
		system("cls");
		main();
	}
	

}

void sendPrintR() {
	std::cout << "Print\n\n";

	std::string toPrint;
	std::cout << "Enter data to print > ";
	//std::cin >> toPrint;
	std::getline(std::cin>>std::ws, toPrint);
	std::string tmp_str = "PRNT-"+toPrint;
	char* packet = new char[tmp_str.size() + 1];
	std::copy(tmp_str.begin(), tmp_str.end(), packet);
	packet[tmp_str.size()] = '\0';

	send(sock, packet, tmp_str.size() + 1, 0);
	std::cout << "\n[+] Sending..." << std::endl;

	delete[] packet;

	//Wait for request
	std::cout << "[+] Please wait..." << std::endl;
	char reply[2000];
	int recv_size = recv(sock, reply, sizeof(reply), 0);

	//Check if Print is successful
	//AKPT-[Success Code]
	std::string repStr = std::string(reply);
	if (splitOn(repStr, 0) == "AKPT") {
		if (splitOn(repStr, 1) == "1") {
			std::cout << "[+] Print request sent successfully" << std::endl;
		}
		else {
			std::cout << "[-] Print request failed" << std::endl;
		}
	}

}

void registerUser() {
	//Send register request
	std::string tmp_str = "NWID";
	char* packet = new char[tmp_str.size() + 1];
	std::copy(tmp_str.begin(), tmp_str.end(), packet);
	packet[tmp_str.size()] = '\0';

	send(sock, packet, tmp_str.size() + 1, 0);
	//std::cout << "Sent Packet" << tmp_str << std::endl;

	delete[] packet;

	//Wait for DEFS to receive ID
	std::cout << "[+] Waiting for server..." << std::endl;
	char reply[2000];
	int recv_size = recv(sock, reply, sizeof(reply), 0);

	//When ID is received
	//DEFS-[ID]
	std::string repStr = std::string(reply);

	//std::cout << repStr << std::endl;
	//std::cout << splitOn(repStr, 0) << std::endl;
	//std::cout << splitOn(repStr, 1) << std::endl;

	int tmp_id = 0;
	if (splitOn(repStr, 0) == "DEFS") {
		tmp_id = std::stoi(splitOn(repStr, 1));
		std::cout << "[+] ID has been received" << std::endl;

		//Get Priv-Level, Pass, and Priority
		std::string tmp_pass;
		short tmp_priority;
		short tmp_privLevel;

		Sleep(2000);

		do {
			system("cls");

			std::cout << "Register User\n\n";

			std::cout << "Password (Between 8 and 16 characters) > ";
			std::cin >> tmp_pass;

			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
		} while (tmp_pass.length() < 8 || tmp_pass.length() > 16); //If not between 8 and 16 characters


		std::cout << "Priority (1, 2, 3) > ";
		std::cin >> tmp_priority;

		std::cout << "PrivLevel (1, 2 ,3) > ";
		std::cin >> tmp_privLevel;

		//Send RGST request
		//RGST-[ID]-[Pass]--[Priority]-[PrivLevel]
		//std::cout << "Sending RGST reply.." << std::endl;
		tmp_str = "RGST-" + std::to_string(tmp_id) + "-" + tmp_pass + "-" + std::to_string(tmp_priority) + "-" + std::to_string(tmp_privLevel);

		//Reply with AKIN
		packet = new char[tmp_str.size() + 1];
		std::copy(tmp_str.begin(), tmp_str.end(), packet);
		packet[tmp_str.size()] = '\0';

		send(sock, packet, tmp_str.size() + 1, 0);
		delete[] packet;
		std::cout << "\n[+] Registration successful" << std::endl;
	}
	else {
		std::cout << "[-] Error has occured" << std::endl;
	}
}

int initConnection(SOCKET& sock) {
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return WSAGetLastError();

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		return WSAGetLastError();

	if (inet_addr(IP.c_str()) == INADDR_NONE)
		return -420;

	server.sin_addr.s_addr = inet_addr(IP.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(31466);

	if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0)
		return WSAGetLastError();

	sock = s;
	return 0;
}

std::string splitOn(std::string s, int index) {
	std::string delimiter = "-";

	size_t pos = 0;
	std::string token;
	int tmp_index = 0;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		if (tmp_index == index)
			return token;
		s.erase(0, pos + delimiter.length());
		tmp_index++;
	}
	return s;
}

bool is_digits(const std::string& str) {
	return str.find_first_not_of("0123456789") == std::string::npos;
}