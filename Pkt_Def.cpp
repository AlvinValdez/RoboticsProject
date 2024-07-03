

#include "Pkt_Def.h"
#include <cstring>
#include <bitset>

//Alvin
PktDef::PktDef() {

	//Header
	CmdPacket.Header.pktCount = 0;
	CmdPacket.Header.ack = 0;
	CmdPacket.Header.arm = 0;
	CmdPacket.Header.claw = 0;
	CmdPacket.Header.drive = 0;
	CmdPacket.Header.length = 0;
	CmdPacket.Header.padding = 0;
	CmdPacket.Header.status = 0;
	CmdPacket.Header.sleep = 0;

	//Body
	CmdPacket.data = nullptr;

	//Tail
	CmdPacket.CRC = 0;

	

}

//Carl
PktDef::PktDef(char * rawDataBuffer) {
	char *ptr = rawDataBuffer;

	memcpy(&CmdPacket.Header.pktCount, ptr, sizeof(int));

	ptr += sizeof(int);

	CmdPacket.Header.sleep = (*ptr) & 1;

	CmdPacket.Header.status = (*ptr >> 1) & 1;

	CmdPacket.Header.drive = (*ptr >> 2) & 1;

	CmdPacket.Header.claw = (*ptr >> 3) & 1;

	CmdPacket.Header.arm = (*ptr >> 4) & 1;

	CmdPacket.Header.ack = (*ptr >> 5) & 1;

	CmdPacket.Header.padding = (*ptr >> 6) & 2;

	ptr += sizeof(char);

	memcpy(&CmdPacket.Header.length, ptr, sizeof(char));

	ptr += sizeof(char);

	this->SetBodyData(ptr, (CmdPacket.Header.length - HEADERSIZE - 1));

	ptr += sizeof(CmdPacket.Header.length - HEADERSIZE - 1);


	CalcCRC();
	
}

//Carl
PktDef::~PktDef() {
	delete[] CmdPacket.data;
	delete[] rawBuffer;
}

//Carl
void PktDef::SetCmd(CmdType command) {
	if (command == DRIVE) {
		CmdPacket.Header.arm = 0;
		CmdPacket.Header.sleep = 0;
		CmdPacket.Header.claw = 0;
		CmdPacket.Header.drive = 1;
		CmdPacket.Header.status = 0;
		CmdPacket.Header.ack = 0;

	}

	if (command == ARM) {
		CmdPacket.Header.arm = 1;
		CmdPacket.Header.sleep = 0;
		CmdPacket.Header.claw = 0;
		CmdPacket.Header.drive = 0;
		CmdPacket.Header.status = 0;
		CmdPacket.Header.ack = 0;
	}

	if (command == CLAW) {
		CmdPacket.Header.arm = 0;
		CmdPacket.Header.sleep = 0;
		CmdPacket.Header.claw = 1;
		CmdPacket.Header.drive = 0;
		CmdPacket.Header.status = 0;
		CmdPacket.Header.ack = 0;
	}

	if (command == SLEEP) {
		CmdPacket.Header.arm = 0;
		CmdPacket.Header.sleep = 1;
		CmdPacket.Header.claw = 0;
		CmdPacket.Header.drive = 0;
		CmdPacket.Header.status = 0;
		CmdPacket.Header.ack = 0;
	}

	if (command == ACK) {
		CmdPacket.Header.ack = 1;
	}

	if (command == HEADER) {
		CmdPacket.Header.arm = 1;
		CmdPacket.Header.sleep = 0;
		CmdPacket.Header.claw = 1;
		CmdPacket.Header.drive = 1;
		CmdPacket.Header.status = 0;
		CmdPacket.Header.ack = 0;
	}
	

}

//Carl
void PktDef::SetBodyData(char * rawDataBuffer, int sizeBuffer) {
	if (CmdPacket.data != nullptr) {
		delete[] CmdPacket.data;
		CmdPacket.data = nullptr;
	}

	CmdPacket.Header.length = 7 + sizeBuffer;

	CmdPacket.data = new char[sizeBuffer];
	std::memcpy(CmdPacket.data, rawDataBuffer, sizeBuffer); 
}

//Carl
void PktDef::SetPktCount(int count) {
	CmdPacket.Header.pktCount = count;
}

//Carl
CmdType PktDef::GetCmd() const {
	CmdType command;

	if (CmdPacket.Header.drive) {
		command = DRIVE;
	}

	if (CmdPacket.Header.arm) {
		command = ARM;
	}

	if (CmdPacket.Header.claw) {
		command = CLAW;
	}

	if (CmdPacket.Header.sleep) {
		command = SLEEP;
	}

	return command;

}

//Carl
bool PktDef::GetAck() const {
	return CmdPacket.Header.ack;
}

//Alvin
int PktDef::GetLength() const {
	return CmdPacket.Header.length;
}

//Alvin
char * PktDef::GetBodyData() const {
	return CmdPacket.data;
}

//Alvin
int PktDef::GetPktCount() const {
	return CmdPacket.Header.pktCount;
}

//Alvin
bool PktDef::CheckCRC(char * rxBuffer, int size) const {
	
	char * ptr = rxBuffer;
	bool return_v = false;
	int count = 0;

	for (int i = 0; i < size - 1; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			count += (*ptr >> j) & 0x01;
		}

		ptr += sizeof(char);
	}

	if ((char)count == (char)*ptr)
	{
		return_v = true;
	}

	return return_v;

	
}


//Alvin
void PktDef::CalcCRC() {
	
	int oneBits = 0;
	int bodySize = CmdPacket.Header.length - 7;

	int loopIntFor = 8 * sizeof(CmdPacket.Header.pktCount);

	
	for (int i = 0; i < loopIntFor; i++)
	{
		oneBits += (CmdPacket.Header.pktCount >> i) & 1;
	}

	
	if (CmdPacket.Header.status & 1) { 
		oneBits++; 
	}
	
	if (CmdPacket.Header.ack & 1) { 
		oneBits++; 
	}
	
	if (CmdPacket.Header.drive & 1) { 
		oneBits++; 
	}
	
	if (CmdPacket.Header.arm & 1) { 
		oneBits++; 
	}
	
	if (CmdPacket.Header.claw & 1) { 
		oneBits++; 
	}
	
	if (CmdPacket.Header.sleep & 1) { 
		oneBits++; 
	}

	
	for (int counter = 0; counter < 8; counter++)
	{
		oneBits += (CmdPacket.Header.length >> counter) & 1;
	}

	
	if (bodySize > 0) {

		for (int out = 0; out < bodySize; out++)
		{
			for (int i = 0; i < 8; i++)
			{
				oneBits += (CmdPacket.data[out] >> i) & 1;
			}
		}
	}
	
	CmdPacket.CRC = oneBits;
	



}

//Alvin
char* PktDef::GenPacket() {
	
	int bodySize = CmdPacket.Header.length - 7;

	rawBuffer = new char[CmdPacket.Header.length];

	memcpy(rawBuffer, &CmdPacket, HEADERSIZE);

	memcpy(rawBuffer + HEADERSIZE, CmdPacket.data, bodySize);

	memcpy(rawBuffer + HEADERSIZE + bodySize, &CmdPacket.CRC, sizeof(char));

	
	return rawBuffer;
}
