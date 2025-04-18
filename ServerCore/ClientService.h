#pragma once
#include "Service.h"
class ClientService	 : public Service
{
private:
	GameRoom<Session> session;
public:
	GameRoom<Session> GetSession() { return session; }
public:
	ClientService(wstring ip, uint16 port, SessionFactory factory);
	virtual ~ClientService() {}
public:
	virtual bool Start() override;
};

