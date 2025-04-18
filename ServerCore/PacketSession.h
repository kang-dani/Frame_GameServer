#pragma once
#include "Session.h"

//[2byte : size][2byte : id][보낼 데이터.....]
struct PacketHeader
{
	uint16 size;	//크기 
	uint16 id;		//분류

};

class PacketSession : public Session
{
public:
	PacketSession() = default;
	virtual ~PacketSession() = default;
public:
	//PacketSession 주소 넘겨주기
	GameRoom<PacketSession> GetPacketSession() { return static_pointer_cast<PacketSession>(shared_from_this()); }
public:
	//sealed : 상속받은 자식이 이 함수를 오버라이드(덮어쓰기) 할수 없음
	virtual int OnRecv(BYTE* buffer, int len) sealed;
	//자식이 OnRecv 대신에 OnRecvPacket을 사용 할수 있도록 전환
	virtual int OnRecvPacket(BYTE* buffer, int len) abstract;

};

