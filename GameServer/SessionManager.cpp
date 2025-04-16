#include "pch.h"
#include "SessionManager.h"
#include "ClientSession.h"

void SessionManager::Add(shared_ptr<ClientSession> session)
{
	unique_lock<shared_mutex> lock(rwLock);
	sessions.insert(session);
	
}

void SessionManager::Remove(shared_ptr<ClientSession> session)
{
	unique_lock<shared_mutex> lock(rwLock);
	sessions.erase(session);

}

void SessionManager::Broadcast(shared_ptr<class SendBuffer> buffer)
{
	unique_lock<shared_mutex> lock(rwLock);
	for (auto& session : sessions)
	{
		session->Send(buffer);
	}
}
