#pragma once
#include "IocpObj.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"
#include <atomic>

class Service;

class Session : public IocpObj
{
	friend class Listener;
	enum {BUFFER_SIZE = 0x10000};
private:
	shared_mutex rwLock;	
	atomic<bool> isConnected = false;
	shared_ptr<Service> service = nullptr;
	SOCKET socket = INVALID_SOCKET;
	SOCKADDR_IN sockAddr = {};
private:
	ConnectEvent connectEvent;
	SendEvent sendEvent;
	RecvEvent recvEvent;
	DisconnectEvent disconnectEvent;
public:
	queue<shared_ptr<SendBuffer>> sendQueue;
	atomic <bool> sendRegistered = false;
	RecvBuffer recvBuffer;
public:
	Session();
	virtual ~Session();

public:
	SOCKET GetSocket() const { return socket; }
	HANDLE GetHandle() override { return (HANDLE)socket; };
	shared_ptr<Service> GetService() const { return service; }
	shared_ptr<Session> GetSession() { return static_pointer_cast<Session>(shared_from_this()); }
public:
	void SetSockAddr(SOCKADDR_IN address) { sockAddr = address; }
	void SetService(shared_ptr<Service> _service) { service = _service; }
public:
	bool IsConnected() const { return isConnected; }
private:
	bool RegisterConnect();
	void RegisterRecv();
	void RegisterSend();
	bool RegisterDisconnect();
private:
	void ProcessConnect();
	void ProcessRecv(int bytesTransferred);
	void ProcessSend(int bytesTransferred);
	void ProcessDisconnect();
private:
	void HandleError(int errorCode);
public:
	virtual void OnConnected() {}
	virtual int OnRecv(BYTE* buffer, int len) { return len; }
	virtual void OnSend(int len) {}
	virtual void OnDisconnected() {}
public:
	bool Connect();
	void Send(shared_ptr<SendBuffer> sendBuffer);
	void Disconnect(const WCHAR* cause);
public:
	void ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred) override;

};

