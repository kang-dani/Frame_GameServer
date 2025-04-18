#include "pch.h"
#include "SessionManager.h"
#include "ClientSession.h"

void SessionManager::Add(GameRoom<ClientSession> session)
{
	unique_lock<shared_mutex> lock(rwLock);
	sessions.insert(session);
	
}

void SessionManager::Remove(GameRoom<ClientSession> session)
{
	unique_lock<shared_mutex> lock(rwLock);
	sessions.erase(session);

}

void SessionManager::Broadcast(GameRoom<class SendBuffer> buffer)
{
	unique_lock<shared_mutex> lock(rwLock);
	for (auto& session : sessions)
	{
		session->Send(buffer);
	}
}
