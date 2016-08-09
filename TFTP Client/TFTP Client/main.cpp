#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

void getUserInput(string &, string &, string &);

int main() {
	string remoteHost, localFile, remoteFile;
	getUserInput(remoteHost, localFile, remoteFile);
	//initialize winsock2
	WSADATA wsdata;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsdata);
	if (iResult != 0) {
		printf("WSAStartup failed : %d", iResult);
		return 1;
	}

	//set up the socket
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			//ipv4
	hints.ai_socktype = SOCK_DGRAM;		//use datagrams
	hints.ai_protocol = IPPROTO_UDP;	//use UDP protocol

#define DEFAULT_PORT "69"				//set port to find on server-side to be port 69

										//resolve server addr and port
	iResult = getaddrinfo(remoteHost.c_str(), DEFAULT_PORT, &hints, &result);//argv[1] is the host address (remote host we want to connect to)
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	return 0;
}

void getUserInput(string & remoteHost, string & localFileName, string & remoteFileName) {
	cout << "enter the remote IP" << endl;
	cin >> remoteHost;
	cout << "enter the name of the file to download" << endl;
	cin >> remoteFileName;
	cout << "enter the name of the file you wish to save it as" << endl;
	cin >> localFileName;
};