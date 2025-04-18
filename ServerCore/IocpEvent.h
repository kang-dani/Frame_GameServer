#pragma once

enum class EventType : uint8
{
	CONNECT,
	DISCONNECT,
	ACCEPT,
	RECV,
	SEND,

};

class IocpObj;
class Session;

class IocpEvent : public OVERLAPPED
{
public:
	EventType eventType;
	shared_ptr<IocpObj> owner;
public:
	IocpEvent(EventType type);
public:
	void Init();

};

class AcceptEvent : public IocpEvent
{
public:
	shared_ptr<Session> session;
public:
	AcceptEvent() : IocpEvent(EventType::ACCEPT) {}
};

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::CONNECT) {}
};


class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::DISCONNECT) {}
};


class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::RECV) {}
};

//전방선언
class SendBuffer;

class SendEvent : public IocpEvent
{
public:
	//sendBuffer class로 변경
	//vector<BYTE> sendBuffer;
	vector<shared_ptr<SendBuffer>> sendBuffers;
public:
	SendEvent() : IocpEvent(EventType::SEND) {}
};
