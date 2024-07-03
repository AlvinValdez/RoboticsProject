#include <windows.networking.sockets.h>
#include <string>
#include <winsock.h>
#include <string>
#include <memory>
#include <iostream>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
enum SocketType { CLIENT, SERVER };
enum ConnectionType { TCP, UDP };
const int DEFAULT_SIZE = 128;
class MySocket
{
private:
	char* Buffer;
	SOCKET WelcomeSocket;
	SOCKET ConnectionSocket;
	struct sockaddr_in SvrAddr;
	struct sockaddr_in RespAddr;
	SocketType mySocket;
	std::string IPAddr;
	int Port;
	ConnectionType connectionType;
	bool bConnect;
	int MaxSize;
	WSADATA wsaData;
	
	

public:
	//Alvin
	MySocket(SocketType, std::string, unsigned int, ConnectionType, unsigned int);
	~MySocket();
	bool ConnectTCP();
	bool DisconnectTCP();
	bool SetupUDP();
	bool TerminateUDP();
	int SendData(const char*, int);
	bool StartWSA();




	//Carl
	int GetData(char*);
	std::string GetIPAddr();
	bool SetIPAddr(std::string);
	bool SetPortNum(int);
	int GetPort();
	SocketType GetType();
	bool SetType(SocketType);
};