#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

const int DEFAULT_BUFLEN = 512;				//max size of packet
const char * DEFAULT_PORT = "69";				//set port to find on server-side to be port 69

const string MODE = "octet";
const byte OP_RRQ = 1;
const byte OP_ACK = 4;
const byte OP_ERROR = 5;

int establishSocket(string, SOCKET &);
BYTE * createRequest(const byte, const string, const string);
BYTE * createAcknowledgment(byte blockNumber[]);
void getUserInput(string &, string &, string &);
int tftpSend(SOCKET, char *);



int main() {
	//initialize winsock2
	WSADATA wsdata;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsdata);
	if (iResult != 0) {
		printf("WSAStartup failed : %d", iResult);
		return 1;
	}

	//get input from user
	string remoteHost, localFile, remoteFile;
	getUserInput(remoteHost, localFile, remoteFile);

	//establish socket
	SOCKET UDPSocket = INVALID_SOCKET;
	iResult = establishSocket(remoteHost, UDPSocket);
	if (iResult != 0) {//check for errors establishing socket
		return 1;
	}


	///-----------------receiving/sending from here onward
//	int recvbuflen = DEFAULT_BUFLEN;
	char *sendbuf = "test";//generate a buffer here;
//	char recvbuf[DEFAULT_BUFLEN];

	iResult = tftpSend(UDPSocket, sendbuf);
	if (iResult != 0) {
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

BYTE * createRequest(const byte opCode, const string fileName, const string mode)
{
	byte zeroByte = 0;
	int rrqByteLength = 2 + fileName.length() + 1 + mode.length() + 1;
	byte *rrqByteArray = new byte(rrqByteLength);

	int position = 0;
	rrqByteArray[position] = zeroByte;
	position++;
	rrqByteArray[position] = opCode;
	position++;
	for (int i = 0; i < fileName.length(); i++)
	{
		rrqByteArray[position] = (byte)fileName[i];
		position++;
	}
	rrqByteArray[position] = zeroByte;
	position++;
	for (int i = 0; i < mode.length(); i++)
	{
		rrqByteArray[position] = (byte)mode[i];
		position++;
	}
	rrqByteArray[position] = zeroByte;

	return rrqByteArray;

}

BYTE * createAcknowledgment(byte blockNumber[])
{
	byte ACK[] = { 0,OP_ACK,blockNumber[0],blockNumber[1] };
	return ACK;
}

int establishSocket(string remoteHost, SOCKET & sock) {//returns 0 if no error and socket is set up, else returns 1
	int iResult;

	//set up the socket
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			//ipv4
	hints.ai_socktype = SOCK_DGRAM;		//use datagrams
	hints.ai_protocol = IPPROTO_UDP;	//use UDP protocol

										//resolve server addr and port
	iResult = getaddrinfo(remoteHost.c_str(), DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	sock = INVALID_SOCKET;
	//----------- should try this block over until we get a valid address for ptr by setting ptr=ptr->ai_next while not null
	//we now connect to the first address returned by the call to getaddrinfo
	ptr = result;

	// create a SOCKET for sending to the server
	sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	//check to see if we managed to create the socket, if so exit
	if (sock == INVALID_SOCKET) {
		printf("Error at setting up a socket : %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	//connect to the server
	iResult = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(sock);
		sock = INVALID_SOCKET;
	}
	//-----------------------------------------------------------------------

	//free resources up and exit if we do not manage to find a valid socket after trying all returned sockets
	freeaddrinfo(result);
	if (sock == INVALID_SOCKET) {
		printf("unable to connect to server! \n");
		WSACleanup();
		return 1;
	}
	return 0;
}

int tftpSend(SOCKET sendSocket, char * sendData) {

	int iResult;
	iResult = send(sendSocket, sendData, (int)strlen(sendData), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed : %d\n", WSAGetLastError());
		closesocket(sendSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}