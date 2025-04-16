#include "pch.h"
#include "GameRoom.h"
#include "Player.h"

#include "Protocol.pb.h"
#include "ClientPacketHandler.h"

void GameRoom::Enter(shared_ptr<Player> player)
{
	shared_ptr<Session> session = player->session.lock();
	if (session != nullptr)
	{
		unique_lock<shared_mutex> lock(rwLock);
		players.insert(player);
		printf("Player ID [%u] ÀÔÀå\n", player->id);


		Protocol::S_EnterGame sendPacket;
		for (const auto& p : players)
		{
			Protocol::Player* player = sendPacket.add_players();
			player->set_id(p->id);
			player->set_name(p->name);
		}

		sendPacket.set_success(true);

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sendPacket);
		session->Send(sendBuffer);
	}

}

void GameRoom::Leave(shared_ptr<Player> player)
{
	unique_lock<shared_mutex> lock(rwLock);
	players.erase(player);
}

void GameRoom::Broadcast(shared_ptr<class SendBuffer> sendBuffer)
{
	unique_lock<shared_mutex> lock(rwLock);
	for (auto& player : players)
	{
		auto session = player->session.lock();
		if (session)
		{
			session->Send(sendBuffer);
		}

	}
}
