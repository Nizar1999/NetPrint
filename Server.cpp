#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <winuser.h>
#include <Windows.h>
#include <map>
#include "User.h"
#include <thread>
#include <vector>
#include <list>

#pragma comment(lib, "ws2_32.lib") //Winsock Library
#pragma comment(lib, "user32.lib")

void LGIN(int, std::string, SOCKET);
int initServer();
void handleConnections(SOCKET);
void handleRequests(int id, SOCKET sock);
void printA();

//Utility Function
std::string splitOn(std::string, int);

std::map <int, User> Users; //map containing all Users
std::map <int, SOCKET> connections;
std::map <short, std::vector<std::string> > printQ = {
	{1, {}},
	{2, {}},
	{3, {}}
};
int ncon = 0;
int semaphore = 0;
WSADATA wsa;
SOCKET s;
struct sockaddr_in server, client;
int c;
int newId = 2;
//char data[2000];

int main() {
	//Admin User
	User admin(0,"niz12345", 3, 3);
	Users[admin.getId()] = admin;
	User pat(1, "pat12345", 2, 2);
	Users[pat.getId()] = pat;

	//Initialize server and listen for connections
	std::string logo = R"(
 /$$   /$$             /$$     /$$$$$$$           /$$             /$$                          /$$$$$$ 
| $$$ | $$            | $$    | $$__  $$         |__/            | $$                         /$$__  $$
| $$$$| $$  /$$$$$$  /$$$$$$  | $$  \ $$ /$$$$$$  /$$ /$$$$$$$  /$$$$$$                      | $$  \__/
| $$ $$ $$ /$$__  $$|_  $$_/  | $$$$$$$//$$__  $$| $$| $$__  $$|_  $$_/         /$$$$$$      |  $$$$$$ 
| $$  $$$$| $$$$$$$$  | $$    | $$____/| $$  \__/| $$| $$  \ $$  | $$          |______/       \____  $$
| $$\  $$$| $$_____/  | $$ /$$| $$     | $$      | $$| $$  | $$  | $$ /$$                     /$$  \ $$
| $$ \  $$|  $$$$$$$  |  $$$$/| $$     | $$      | $$| $$  | $$  |  $$$$/                    |  $$$$$$/
|__/  \__/ \_______/   \___/  |__/     |__/      |__/|__/  |__/   \___/                       \______/ 
                                                                                                       
                                                                                                       
                                                                                                       
                                                                                                        
)";

	std::cout << logo << std::endl;

	std::cout << "[+] Server Running..\n" << std::endl;
	while (initServer()) { //Keep retrying till we establish a connection
		std::cout << "." << std::flush;
		Sleep(500);
		std::cout << "." << std::flush;
		Sleep(500);
		std::cout << "." << std::flush;
		Sleep(500);
		std::cout << "\b\b\b   \b\b\b" << std::flush;
	}
	
	return 0;
}

void LGIN(int id, std::string password, SOCKET sock) {

	std::cout << "[+] Sending AKIN reply..\n" << std::endl;

	//Prepare AKIN
	//AKIN-[Success Code]-[Privilege Code]
	std::string reply = "AKIN-";

	//Check if creds are valid
	if (Users[id].getPassword() == password) {
		reply += "0-" + std::to_string(Users[id].getPrivLevel());
	}
	else {
		reply += "1";
	}

	//Reply with AKIN
	char* packet = new char[reply.size() + 1];
	std::copy(reply.begin(), reply.end(), packet);
	packet[reply.size()] = '\0';

	send(sock, packet, reply.size() + 1, 0);
	delete[] packet;

	//Wait for additional requests
	if (Users[id].getPassword() == password) {
		handleRequests(id, sock);
	}
	else {
		handleConnections(sock);
	}
}

void handleRequests(int id, SOCKET sock) {
	char request[2000];
	int recv_size = recv(sock, request, sizeof(request), 0);
	if (recv_size == SOCKET_ERROR) {
		std::cout << "\n[-] UserID "<<id<<" has disconnected"<< std::endl;
		Sleep(2000);
		//exit(1);
		//system("Pause");
	}
	else {
		//Identify request
		std::string reqStr = std::string(request);
		std::cout << "[+] Received request: " << reqStr << std::endl;
		std::string reqCode = splitOn(reqStr, 0);
		if (reqCode == "NWID") {
			//reply with id
			//get new ID
			//int newID = Users.size();
			int newID = newId;
			newId++;
			//send new ID
			std::string tmp_str = "DEFS-" + std::to_string(newID);
			char* packet = new char[tmp_str.size() + 1];
			std::copy(tmp_str.begin(), tmp_str.end(), packet);
			packet[tmp_str.size()] = '\0';

			send(sock, packet, tmp_str.size() + 1, 0);
			std::cout << "[+] Sending new ID: " << tmp_str << std::endl;
			delete[] packet;

			//receive RGST request
			memset(&request[0], 0, sizeof(request)); //clear request array
			recv_size = recv(sock, request, sizeof(request), 0);

			//Identify request
			std::string reqStr = std::string(request);
			std::cout << "[+] Received request: " << reqStr << std::endl;
			std::string reqCode = splitOn(reqStr, 0);
			if (reqCode == "RGST" && newID == std::stoi(splitOn(reqStr, 1))) {
				std::string newPass = splitOn(reqStr, 2);				//Pass
				short newPriority = (short)std::stoi(splitOn(reqStr, 3));	//Priority
				short newPrivLevel = (short)std::stoi(splitOn(reqStr, 4));	//PrivLevel

				//Register new user
				User newUser(newID, newPass, newPriority, newPrivLevel);
				Users[newID] = newUser;
			}
			handleRequests(id, sock);
		}
		if (reqCode == "PRNT") {
			//std::cout << "entered print" << std::endl;
			std::string toPrint = splitOn(reqStr, 1);
			while (semaphore < 0) {
				std::cout << "[-] Waiting.." << std::endl;
				Sleep(100);
			}
			semaphore--;
			//handle the print request
			//critical part
			std::cout << "[+] Entered critical section" << std::endl;
			printQ[Users[id].getPriority()].push_back(toPrint);
			semaphore++;

			//send AKPT-[0/1] to user
			std::string tmp_str = "AKPT-1";
			char* packet = new char[tmp_str.size() + 1];
			std::copy(tmp_str.begin(), tmp_str.end(), packet);
			packet[tmp_str.size()] = '\0';

			send(sock, packet, tmp_str.size() + 1, 0);
			std::cout << "[+] Sent AKPT reply: " << tmp_str << std::endl << std::endl;
			delete[] packet;
			handleRequests(id, sock);
		}
		if (reqCode == "LGOT") {
			std::cout << "[-] UserID :" << id << " has logged out" << std::endl;
			closesocket(sock);
			//WSACleanup();
		}
		//std::cout << "??????????????????"<< std::endl;
	}

}
void printA(){
	while (true) {
		//std::cout << "entered printA" << std::endl;
		Sleep(5000);
		while (semaphore < 0) {
			std::cout << "[-] Waiting.." << std::endl;
			Sleep(500);
		}
		semaphore--;
		//std::cout << "entered printA crit" << std::endl;
		//handle the printing
		for (int i = printQ.size(); i >= 1; i--) {
			if (!printQ[i].empty()) {
				for (int z = 0; z < printQ[i].size(); z++) {
					std::cout << "[|] Printed: " << printQ[i][z] << " (Priority " << i << ")" << std::endl;
				}
				printQ[i].clear();
			}
		}
		semaphore++;
	}
}
int initServer() {
	/*WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server, client;
	int c;
	char data[2000];*/

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return WSAGetLastError();

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		return WSAGetLastError();

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(31466);

	//Bind
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
		return WSAGetLastError();

	listen(s, 0);

	c = sizeof(struct sockaddr_in);
	//start printing 
	std::thread pr(printA);
	pr.detach();

	while (true) {
		SOCKET new_socket = accept(s, (struct sockaddr*)&client, &c);
		ncon++;
		connections[ncon] = new_socket;
		if (new_socket == INVALID_SOCKET) {
			std::cout << "[-] Error has occured\n[-]Could not accept connection" << std::endl;
			return WSAGetLastError();
		}
		std::thread thread_obj(handleConnections, connections[ncon]);
		//thread_obj.join();
		thread_obj.detach();
	}
	return 0;
}

void handleConnections(SOCKET sock) {
	char request[2000];
	int recv_size = recv(sock, request, sizeof(request), 0);

	//Identify request
	std::string reqStr = std::string(request);

	std::cout << "[+] Received Login request: " << reqStr << std::endl;

	std::string reqCode = splitOn(reqStr, 0);

	if (reqCode == "LGIN") {
		LGIN(std::stoi(splitOn(request, 1)), splitOn(request, 2), sock);
	}
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