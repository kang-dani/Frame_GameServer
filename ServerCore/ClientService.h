#pragma once
#include "Service.h"
class ClientService	 : public Service
{
private:
	shared_ptr<Session> session;
public:
	shared_ptr<Session> GetSession() { return session; }
public:
	ClientService(wstring ip, uint16 port, SessionFactory factory);
	virtual ~ClientService() {}
public:
	virtual bool Start() override;
};

