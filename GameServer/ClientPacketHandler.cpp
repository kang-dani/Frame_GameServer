#include "pch.h"
#include "ClientPacketHandler.h"
#include "GameRoom.h"
#include "Player.h"

void ClientPacketHandler::Init()
{
	PacketHandler::Init();

	RegisterPacketHandler<Protocol::C_Login>(C_LOGIN, Handle_C_LOGIN);
	RegisterPacketHandler<Protocol::C_EnterGame>(C_ENTER_GAME, Handle_C_ENTER_GAME);
	RegisterPacketHandler<Protocol::C_Chat>(C_CHAT, Handle_C_CHAT);

}

bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
	printf("Invalid handle\n");
	return false;
}

bool Handle_C_LOGIN(shared_ptr<PacketSession>& session, Protocol::C_Login& packet)
{
	printf("Client Login\n");

	Protocol::S_Login sendPacket;
	//Todo
	sendPacket.set_success(true);
	int id = GameRoom::Get().GetID();
	sendPacket.set_playerid(id);

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sendPacket);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_ENTER_GAME(shared_ptr<PacketSession>& session, Protocol::C_EnterGame& packet)
{
	printf("Client Enter Game\n");

	shared_ptr<Player> player = make_shared<Player>();
	auto& p = packet.player();
	player->id = p.id();
	player->name = p.name().c_str();
	player->session = static_pointer_cast<ClientSession>(session);

	{
		Protocol::S_JoinGame sendPacket;

		Protocol::Player* joinPlayer = new Protocol::Player;
		joinPlayer->set_id(p.id());
		joinPlayer->set_name(p.name());

		sendPacket.set_allocated_player(joinPlayer);
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sendPacket);

		GameRoom::Get().Broadcast(sendBuffer);

		GameRoom::Get().Enter(player);

	}
	return true;
}

bool Handle_C_CHAT(shared_ptr<PacketSession>& session, Protocol::C_Chat& packet)
{
	
	printf("C_CHAT : ");
	printf("%s\n", packet.message().c_str());
	return false;
}
