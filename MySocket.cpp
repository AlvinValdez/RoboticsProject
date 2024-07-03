#include "MySocket.h"
MySocket::MySocket(SocketType socket, std::string IP, unsigned int port, ConnectionType connection, unsigned int size) {
	mySocket = socket;
	IPAddr = IP;
	Port = port;
	connectionType = connection;
	bConnect = false;
	
	if (size <0) {
		MaxSize = DEFAULT_SIZE;
	}
	else {
		MaxSize = size;
	}
	
	Buffer = new char[MaxSize];
}

MySocket::~MySocket() {
	delete[] Buffer;
	Buffer = nullptr;
}

bool MySocket::ConnectTCP() {
	int nToLog = 0;
	if (connectionType != TCP) {
		//std::cout <<"Not a TCP" <<std::endl;
		return false;
	}
	if (connectionType == TCP) {
		StartWSA();
		//std::cout << "Started WSA" << std::endl;
		if (mySocket == SERVER) {
			//std::cout << "Line 27" << std::endl;
			WelcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (WelcomeSocket == INVALID_SOCKET) {
				//	std::cout << "Invalid Socket" << std::endl;
				WSACleanup();
				return false;
			}
			SvrAddr.sin_family = AF_INET; //Address family type internet
			SvrAddr.sin_port = htons(Port); //port (host to network conversion)
			SvrAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str()); //IP address
			std::cout << "Socket Initialized" << std::endl;
			if ((bind(WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
				closesocket(WelcomeSocket);
				WSACleanup();
				std::cout << "Could not bind to the socket" << std::endl;
				bConnect = false;
				return false;
			}
			else {
				std::cout << "Socket binded" << std::endl;
				if (listen(WelcomeSocket, 1) == SOCKET_ERROR) {
					closesocket(WelcomeSocket);
					WSACleanup();
					std::cout << "Could not listen to the provided socket" << std::endl;
					bConnect = false;
					return false;
				}
				else {
					std::cout << "Waiting for client connection" << std::endl;

					if ((ConnectionSocket = accept(WelcomeSocket, nullptr, nullptr)) == SOCKET_ERROR) {
						closesocket(WelcomeSocket);
						WSACleanup();
						std::cout << "Could not accept incoming connection from client" << std::endl;
						bConnect = false;
						return false;
					}
					else {

						std::cout << "Connection Accepted" << std::endl;
						bConnect = true;
						return true;
					}
				}

			}


		}
		else if (mySocket == CLIENT) {
			SvrAddr.sin_family = AF_INET; //Address family type internet
			SvrAddr.sin_port = htons(Port); //port (host to network conversion)
			SvrAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str()); //IP address


			ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if ((connect(ConnectionSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
				std::cout << "Can't connect" << std::endl;
				return false;
			}
			else {
				std::cout << "Connection Established" << std::endl;
				bConnect = true;
				return true;
			}
		}
	}
}


bool MySocket::DisconnectTCP() {
	if (connectionType != TCP) {
		return false;
	}

	closesocket(ConnectionSocket);
	closesocket(WelcomeSocket);
	WSACleanup();
	
	bConnect = false;
	return true;
}

bool MySocket::SetupUDP() {
	if(connectionType != UDP) {
	//	std::cout << "Not a UDP Connection" << std::endl;
		bConnect = false;
		return false;
	}

	
	StartWSA();
	
	if (mySocket == SERVER) {
		ConnectionSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		SvrAddr.sin_family = AF_INET; //Address family type internet
		SvrAddr.sin_port = htons(Port); //port (host to network conversion)
		SvrAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str()); //IP address

		

		if ((bind(ConnectionSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
			closesocket(ConnectionSocket);
			WSACleanup();
		//	std::cout << "Could not bind to the socket" << std::endl;
			return false;
		}
		else {
			bConnect = true;
			return true;
		}
		
	}
	else if(mySocket == CLIENT){
		ConnectionSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		SvrAddr.sin_family = AF_INET; //Address family type internet
		SvrAddr.sin_port = htons(Port); //port (host to network conversion)
		SvrAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str()); //IP address
		bConnect = true;
		return true;
	}
	else {
		
		return false;
	}
	

}

bool MySocket::TerminateUDP() {
	if (connectionType != UDP) {
		return false;
	}
	closesocket(ConnectionSocket);
	WSACleanup();
	
	bConnect = false;
	return true;

}

int MySocket::SendData(const char* TxBuffer, int size) {
	int bytes = 0;

	if (connectionType == UDP) {
		if (mySocket == SERVER) {
			bytes = sendto(ConnectionSocket, TxBuffer, size, 0, (struct sockaddr *)&RespAddr, sizeof(RespAddr));
		}
		else if (mySocket == CLIENT) {
			bytes = sendto(ConnectionSocket, TxBuffer, size, 0, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr));
		}
		
	}
	else if (connectionType == TCP) {
		bytes = send(ConnectionSocket, TxBuffer, size, 0);
	}
	return bytes;
}

bool MySocket::StartWSA() {
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
	//	std::cout << "ERROR:  Failed to start WSA (My Error)" << std::endl;
		return false;
	}
	else {
	//	std::cout << "WSA Started" << std::endl;
		return true;
	}
}

//Carl
int MySocket::GetData(char * buffer)
{
	
	
	memset(Buffer, 0, MaxSize);
	int bytes = 0;

	if (connectionType == UDP) {
		int addrLen = sizeof(RespAddr);
		bytes = recvfrom(ConnectionSocket, Buffer, MaxSize, 0, (struct sockaddr *)&RespAddr, &addrLen);
	}
	else if (connectionType == TCP)
		bytes = recv(ConnectionSocket, Buffer, MaxSize, 0);

	if (bytes < 0) {
		bytes= 0;
	}
	else {
		memcpy(buffer, Buffer, bytes);
		return bytes;
	}
}

std::string MySocket::GetIPAddr()
{
	return IPAddr;
}

bool MySocket::SetIPAddr(std::string ipAddress)
{
	bool flag = false;

	if (bConnect == false) {
		IPAddr = ipAddress;
		flag = true;
	}
	
	return flag;
}

bool MySocket::SetPortNum(int portNum)
{
	bool flag = false;

	if (bConnect == false) {
		Port = portNum;
		flag = true;
	}

	return flag;
}

int MySocket::GetPort()
{
	return Port;
}

SocketType MySocket::GetType()
{
	return mySocket;
}

bool MySocket::SetType(SocketType sType)
{
	bool flag = false;

	if (bConnect == false) {
		mySocket = sType;
		flag = true;
	}

	return flag;
}
