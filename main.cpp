#include "Pkt_Def.h"
#include "MySocket.h"
#include <iostream>
#include <vector>
#include <sstream> 
#include <thread>
#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
bool ExeComplete;

std::ofstream ofs("Client_Output.txt");
void TeleThread(std::string IP, unsigned int port) {
	MySocket TeleConnect(CLIENT, IP, port, TCP, 128);
	PktDef hacks;
	char buffer[100];
	if (TeleConnect.ConnectTCP() == true) {
		std::cout << "Connection Successful" << std::endl;
	}
	else {
		std::cout << "Connection Failed" << std::endl;
	}

	while (ExeComplete == false) {
		std::cout << std::dec;
		TeleConnect.GetData(buffer);

		char *ptr;
		ptr = buffer;
		int arm = 0;


		std::cout << "PktCount: " << (int)*ptr << std::endl;


		ptr += 4;

		std::bitset<8> x(*ptr);
		std::cout << "Header Sleep bit: " << x[0]  << std::endl;
		std::cout << "Header Status bit: " << x[1] << std::endl;
		std::cout << "Header Drive bit: " << x[2] << std::endl;
		std::cout << "Header Claw bit: " << x[3] << std::endl;
		std::cout << "Header Arm bit: " << x[4] << std::endl;
		std::cout << "Header Ack bit: " << x[5] << std::endl;
		


		ptr += 2;


		std::cout << "Sonar reading: " << (double)*ptr << std::endl;
		ptr += 2;
		memcpy(&arm, ptr, 2);
		std::cout << "Arm reading: " << arm << std::endl;
		ptr += 2;
		std::bitset<8> y(*ptr);


		if (y[0] == 0) {
			std::cout << "Drive flag bit is STOPPED(" << y[0] << ")" << std::endl;
		}
		else {
			std::cout << "Drive flag bit is DRIVING(" << y[0] << ")" << std::endl;
		}

		if (y[1] == 1) {
			std::cout << "Arm is Up, ";
		}
		else {
			std::cout << "Arm is Down, ";
		}

		if (y[3] == 1) {
			std::cout << "Claw is Open" << std::endl;
		}
		else {
			std::cout << "Claw is Closed" << std::endl;
		}

		ptr -= 10;
		std::cout << "Raw Packet Data: ";
		for (int counter = 0; counter < 12; counter++) {
			std::cout << std::hex << (int)*ptr << " ";
			ptr++;
		}
		std::cout << std::endl << std::endl;;

		
		ptr -= 12;

		
		int bits = 0;
		for (int counter = 0; counter < 11; counter++) {
			std::bitset<8> foo ((int)*ptr);
			ptr++;
			bits += foo.count();
		}
		
		if ((int)bits == (int)*ptr) {
			std::cout << "CRC is matched "<< bits <<" = "<<(int)*ptr << std::endl;
		}
		else {
			std::cout << "CRC is not matched ";
			std::cout << bits <<" != "<< (int)*ptr << std::endl;
		}

		



		if (ExeComplete == true) {
			TeleConnect.DisconnectTCP();
		}




	}



}
void CmdThread(std::string IP, unsigned int port) {
	//MySocket TeleConnect(CLIENT,IP,port,TCP, 128);
	std::string action;
	/*if (TeleConnect.ConnectTCP()==true) {
	std::cout << "Connection Successful" << std::endl;
	}
	else {
	std::cout << "Connection Failed" << std::endl;
	}
	*/
	int pktcount =0;
	PktDef CmdPacket;
	MySocket * command;
	command = new MySocket (CLIENT, IP, port, TCP, 128);
	command->ConnectTCP();
	bool success;
	/*char buffer[100];
	if (CmdConnect.ConnectTCP() == true) {
		std::cout << "Connection Successful" << std::endl;
	}
	else {
		std::cout << "Connection Failed" << std::endl;
	}
	*/

	

	/*****DRIVE COMMAND*****/
	while (ExeComplete == false) {
		std::cout << "Enter commands: " << std::endl;
		std::cin >> action;

		std::stringstream ss(action);
		std::string item;
		std::vector<std::string> tokens;
		while (getline(ss, item, ':')) {
			tokens.push_back(item);
		}
		success = true;
		if (tokens[0] == "DRIVE") {
			unsigned int direction = 0;
			unsigned int duration = 0;

			//Forward
			if (tokens[1] == "1")
				direction = 1;

			//Backward
			else if (tokens[1] == "2")
				direction = 2;

			//Left
			else if (tokens[1] == "3")
				direction = 3;

			//Right
			else if (tokens[1] == "4")
				direction = 4;

			else {
				std::cout << "Invalid direction input." << std::endl;
				success == false;
			}
				

			//Duration
			if (success  == true) {
			duration = std::stoul(tokens[2]);


			MotorBody body;
			body.Direction = direction;
			body.Duration = duration;

			CmdPacket.SetCmd(DRIVE);
			CmdPacket.SetBodyData((char *)&body, 2);
			CmdPacket.SetPktCount(pktcount);
			CmdPacket.CalcCRC();

			char * buffer = CmdPacket.GenPacket();
			command->SendData(buffer, CmdPacket.GetLength());
			command->GetData(buffer);
			}
			
		}

		/*****ARM COMMAND*****/
		else if (tokens[0] == "ARM") {
			unsigned int action = 0;

			//Up
			if (tokens[1] == "5")
				action = 5;

			//Down
			else if (tokens[1] == "6")
				action = 6;

			else {
				std::cout << "Invalid direction input." << std::endl;
				success == false;
			}
			if (success == true) {
		ActuatorBody body;
			body.Action = action;

			CmdPacket.SetCmd(ARM);
			CmdPacket.SetBodyData((char *)&body, 1);
			CmdPacket.SetPktCount(pktcount);
			CmdPacket.CalcCRC();

			char * buffer = CmdPacket.GenPacket();
			command->SendData(buffer, CmdPacket.GetLength());
			command->GetData(buffer);
			}
			

		}

		/*****CLAW COMMAND*****/
		else if (tokens[0] == "CLAW") {
			unsigned int action = 0;

			//Open
			if (tokens[1] == "7")
				action = 7;

			//Close
			else if (tokens[1] == "8")
				action = 8;

			else {
				std::cout << "Invalid direction input." << std::endl;
				success == false;
			}

			if (success == true) {
			ActuatorBody body;
			body.Action = action;

			CmdPacket.SetCmd(CLAW);
			CmdPacket.SetBodyData((char *)&body, 1);
			CmdPacket.SetPktCount(pktcount);
			CmdPacket.CalcCRC();

			char * buffer = CmdPacket.GenPacket();
			command->SendData(buffer, CmdPacket.GetLength());
			command->GetData(buffer);
			}
			

		}

		else if (tokens[0] == "SLEEP") {
			unsigned int action = 0;
			ActuatorBody body;
			body.Action = action;

			CmdPacket.SetCmd(SLEEP);
			CmdPacket.SetBodyData((char *)&body, 0);
			CmdPacket.SetPktCount(pktcount);
			CmdPacket.CalcCRC();

			char * buffer = CmdPacket.GenPacket();
			command->SendData(buffer, CmdPacket.GetLength());
			command->GetData(buffer);
			command->DisconnectTCP();
			ExeComplete = true;

		}

		else if (tokens[0] == "LENGTH") {
			unsigned int action = 5;
			ActuatorBody body;
			body.Action = action;

			CmdPacket.SetCmd(ARM);
			CmdPacket.SetBodyData((char *)&body, 1);
			CmdPacket.SetPktCount(pktcount);
			CmdPacket.CalcCRC();

			char * buffer = CmdPacket.GenPacket();
			buffer[6] = 69;
			command->SendData(buffer, CmdPacket.GetLength());
			command->GetData(buffer);

		}

		else if (tokens[0] == "CRC") {
			unsigned int action = 5;
			ActuatorBody body;
			body.Action = action;

			CmdPacket.SetCmd(ARM);
			CmdPacket.SetBodyData((char *)&body, 1);
			CmdPacket.SetPktCount(pktcount);
			
			CmdPacket.CalcCRC();

			char * buffer = CmdPacket.GenPacket();
			buffer[CmdPacket.GetLength()] = 45;
			command->SendData(buffer, CmdPacket.GetLength());
			command->GetData(buffer);
			

		}

		else if (tokens[0] == "HEADER") {
			unsigned int action = 5;
			ActuatorBody body;
			body.Action = action;

			CmdPacket.SetCmd(HEADER);
			CmdPacket.SetBodyData((char *)&body, 1);
			CmdPacket.SetPktCount(pktcount);
			CmdPacket.CalcCRC();

			char * buffer = CmdPacket.GenPacket();
			
			command->SendData(buffer, CmdPacket.GetLength());
			command->GetData(buffer);

		}
		pktcount++;
	}
}

int main(int argc, char *argv[])
{
	std::string IP;
	unsigned int portT;

	unsigned int portC;
	ExeComplete = false;
	std::cout << "IP Addresss: " << std::endl;
	std::cin >> IP;

	std::cout << "Command Port: " << std::endl;
	std::cin >> portC;

	std::cout << "Telemetry Port: " << std::endl;
	std::cin >> portT;



	std::thread T1(TeleThread, IP, portT);
	std::thread T2(CmdThread, IP, portC);
	T1.detach();
	T2.detach();
	while (ExeComplete == false) {
	}
}

