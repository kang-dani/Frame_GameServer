#pragma once
#include "IocpObj.h"

class AcceptEvent;
class ServerService;

class Listener : public IocpObj
{
private:
	shared_ptr<ServerService> serverService = nullptr;
	SOCKET socket = INVALID_SOCKET;
	vector<AcceptEvent*> accentEvents;	//Ãß°¡
public:
	Listener() = default;
	virtual ~Listener();
public:
	bool StartAccept(shared_ptr<ServerService> service);
	void RegisterAccept(AcceptEvent* acceptEvent);	
	void ProcessAccept(AcceptEvent* acceptEvent);
	void CloseSocket();
	

public:
	HANDLE GetHandle() override { return (HANDLE)socket; };
	void ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred) override;
};

