#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdio>

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char *argv[]) {
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
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	return 0;
}
