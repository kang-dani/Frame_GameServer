#pragma once

#include <set>
#include <memory>
#include <shared_mutex>

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
	std::shared_mutex rwLock;
	std::set<std::shared_ptr<ClientSession>> sessions;
public:
	void Add(std::shared_ptr<ClientSession> session);
	void Remove(std::shared_ptr<ClientSession> session);
	void Broadcast(std::shared_ptr<class SendBuffer> buffer);
};

