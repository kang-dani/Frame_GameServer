#pragma once

class ClientSession;

class SessionManager
{
public:
	static SessionManager& Get()
	{
		static SessionManager instance;
		return instance;
	}
private:
	SessionManager() = default;
	~SessionManager() = default;
public:
	SessionManager(const SessionManager&) = delete;
	SessionManager& operator=(const SessionManager&) = delete;
private:
	shared_mutex rwLock;
	set<shared_ptr<ClientSession>> sessions;
public:
	void Add(shared_ptr<ClientSession> session);
	void Remove(shared_ptr<ClientSession> session);
	void Broadcast(shared_ptr<class SendBuffer> buffer);
};

