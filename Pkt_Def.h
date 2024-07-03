#include <Windows.networking.sockets.h>

const unsigned char FORWARD = 1;
const unsigned char BACKWARD = 2;
const unsigned char RIGHT = 3;
const unsigned char LEFT = 4;
const unsigned char UP = 5;
const unsigned char DOWN = 6;
const unsigned char OPEN = 7;
const unsigned char CLOSE = 8;
const unsigned int HEADERSIZE = 6;

enum CmdType { DRIVE, SLEEP, ARM, CLAW, ACK,HEADER };

struct header {
		unsigned int pktCount;
		bool sleep : 1;
		bool status : 1;
		bool drive : 1;
		bool claw : 1;
		bool arm : 1;
		bool ack : 1;
		bool padding : 2;
		unsigned char length;

	};

	struct MotorBody {
		unsigned char Direction;
		unsigned char Duration;
	};

	struct ActuatorBody {
		unsigned char Action;
	};

	

	class PktDef {
		struct {
			header Header;
			char *data;
			char CRC;
	}CmdPacket;

		char * rawBuffer;

	public:
		PktDef();
		PktDef(char *);
		~PktDef();
		void SetCmd(CmdType);
		void SetBodyData(char *, int);
		void SetPktCount(int);
		CmdType GetCmd() const;
		bool GetAck() const;
		int GetLength() const;
		char *GetBodyData() const;
		int GetPktCount() const;
		bool CheckCRC(char *, int) const;
		void CalcCRC();
		char *GenPacket();
}; 

