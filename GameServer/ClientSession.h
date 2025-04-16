#pragma once
#include <PacketSession.h>
class ClientSession	 : public PacketSession
{
public:
	~ClientSession() { printf("~ClientSession\n"); }
public:
	//ClientSession 주소 넘겨주기
	shared_ptr<ClientSession> GetClientSession() { return static_pointer_cast<ClientSession>(shared_from_this()); }

public:
	virtual void OnConnected() override;
	virtual int OnRecvPacket(BYTE* buffer, int len) override;
	virtual void OnSend(int len) override;
	virtual void OnDisconnected() override;
};

