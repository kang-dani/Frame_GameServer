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
	set<GameRoom<ClientSession>> sessions;
public:
	void Add(GameRoom<ClientSession> session);
	void Remove(GameRoom<ClientSession> session);
	void Broadcast(GameRoom<class SendBuffer> buffer);
};

