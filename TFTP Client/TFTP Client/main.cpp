//AB Khalid
//DT Parker

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <io.h>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

const int DEFAULT_BUFLEN = 512;				//max size of packet
const char * DEFAULT_PORT = "69";				//set port to find on server-side to be port 69

const string MODE = "octet";
const byte OP_RRQ = 1;
const byte OP_ACK = 4;
const byte OP_ERROR = 5;

int establishSocket(string, SOCKET &);
char * createRequest(const byte, const string, const string);
BYTE * createAcknowledgment(byte blockNumber[]);
void getUserInput(string &, string &, string &);
int tftpSend(SOCKET,const char *,int);



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
	//getUserInput(remoteHost, localFile, remoteFile);
	remoteHost = "192.168.1.75";
	remoteFile = "hello.txt";

	//establish socket
	SOCKET UDPSocket = INVALID_SOCKET;
	iResult = establishSocket(remoteHost, UDPSocket);
	if (iResult != 0) {//check for errors establishing socket
		return 1;
	}


	//	int recvbuflen = DEFAULT_BUFLEN;
	const char *sendbuf = createRequest(OP_RRQ, remoteFile, MODE);//generate a buffer here;

	string filename = "hello.txt";
	string opcode = "\x1";
	char spacer = '\x0';
	string mode = "octet";

	string temp = spacer + opcode + filename + spacer + mode + spacer;
	char * ctemp = (char*) temp.c_str();
	ctemp[0] = '\x0';

	//	char recvbuf[DEFAULT_BUFLEN];
	//	system("pause");
	for (int i = 0; i < 1; i++) {
		iResult = tftpSend(UDPSocket, ctemp,temp.size());
		if (iResult != 0) {
			return 1;
		}
		int usleep(250);
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

char * createRequest(const byte opCode, const string fileName, const string mode)
{
	byte zeroByte = 0;
	int rrqCharLength = 2 + fileName.length() + 1 + mode.length() + 1;
	char rrqCharArray[512] = { 0 };

	int position = 0;
	rrqCharArray[position] = char(zeroByte);
	position++;
	rrqCharArray[position] = char(opCode);
	position++;
	for (int i = 0; i < fileName.length(); i++)
	{
		rrqCharArray[position] = fileName[i];
		position++;
	}
	rrqCharArray[position] = char(zeroByte);
	position++;
	for (int i = 0; i < mode.length(); i++)
	{
		rrqCharArray[position] = mode[i];
		position++;
	}
	rrqCharArray[position] = char(zeroByte);
	position++;

	return rrqCharArray;

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

int tftpSend(SOCKET sendSocket,const char * sendData,int datalen) {
	int iResult;
	iResult = send(sendSocket, sendData, datalen, 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed : %d\n", WSAGetLastError());
		closesocket(sendSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}