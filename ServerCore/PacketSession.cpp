#include "pch.h"
#include "PacketSession.h"

//보내는 데이터가 21byte
//인식용으로 4byte (2byte:size, 2byte:id)
//[0]...........................[25][26]..........................[50] 
//[packet                          ][packet                          ]
//[[size(2byte)][id(2byte)][data21]][[size(2byte)][id(2byte)][data21]] 
int PacketSession::OnRecv(BYTE* buffer, int len)
{
	//처리한 길이
	int processLen = 0;

	while (true)
	{
		//패킷을 두개 받는다면
		//50 		 = 50  - 0
		int dataSize = len - processLen;
		//25
	//	int dataSize = 50 -  25


		//적어도 4byte는 까봐야 size랑 id를 알수 있으니까
		//50		 < 4 == sizeof(PacketHeader)
		if (dataSize < sizeof(PacketHeader))
			break;
								
		//                 복사 [2][2]
		//[size(2)][id(2)]	= *( 형 변환     )(buffer[0]번째 주소)
		PacketHeader header = *(PacketHeader*)(&buffer[processLen]);

		//[패킷 1][패킷 2]
		//OnRecvPacket 에다가 (패킷의 시작 주소, 크기)
		//큰데이터를 쪼개서 넘김
		//50byte 받았다면
		//header : 21 + 4
		
		char a = buffer[4];
		char b = buffer[5];
		char c = buffer[6];
		char d = buffer[7];
		char e = buffer[8];
		char f = buffer[9];

		OnRecvPacket(&buffer[processLen], header.size);
		
		//[패킷 1번만 우선 처리]
		//[패킷 2번은 다음 while문]

		//25바이트 처리하고
		processLen += header.size;
	}

	return processLen;
}
