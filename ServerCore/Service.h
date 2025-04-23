#pragma once
#include <functional>
#include "PacketSession.h"

class IocpCore;
class Session;

//스마트 포인터로 관리
using SessionFactory = function<shared_ptr<Session>(void)>;

enum class ServiceType : uint8
{
	NONE,
	SERVER, 
	CLIENT,
};

//Service도 스마트 포인터로 레퍼 관리
class Service  : public enable_shared_from_this<Service>
{
private:
	ServiceType serviceType = ServiceType::NONE;
	SOCKADDR_IN sockAddr = {};
	//스마트 포인터로 관리
	shared_ptr<IocpCore> iocpCore;
protected:
	shared_mutex rwLock;
	//스마트 포인터로 관리
	set<shared_ptr<Session>> sessions;
	int sessionCount = 0;
	SessionFactory sessionFactory;		
public:
	Service(ServiceType type, wstring ip, uint16 port, SessionFactory factory);
	virtual ~Service();
public:
	ServiceType GetServiceType() const { return serviceType; }
	SOCKADDR_IN& GetSockAddr() { return sockAddr; }
	//스마트 포인터
	shared_ptr<IocpCore> GetIocpCore() const { return iocpCore; }
	int GetSessionCount() const { return sessionCount; }
public:
	//스마트 포인터로 관리
	shared_ptr<Session> CreateSession();
	void AddSession(shared_ptr<Session> session);
	void RemoveSession(shared_ptr<Session> session);
	
public:
	virtual bool Start() abstract;
	bool ObserveIO(DWORD time = INFINITE);

	virtual void OnClientJoined(std::shared_ptr<PacketSession> session) = 0;
};

