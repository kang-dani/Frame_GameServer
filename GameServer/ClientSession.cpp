#include "pch.h"
#include "ClientSession.h"


void ClientSession::OnConnected()
{
    SessionManager::Get().Add(GetClientSession());
}

int ClientSession::OnRecvPacket(BYTE* buffer, int len)
{
    shared_ptr<PacketSession> session = GetPacketSession();
    ServerPacketHandler::HandlePacket(session, buffer, len);

    return len;
}

void ClientSession::OnSend(int len)
{
}

void ClientSession::OnDisconnected()
{
    SessionManager::Get().Remove(GetClientSession());
    auto player = GetPlayer();
    if (player)
    {
        RoomManager::Get().RemovePlayerFromRoom(player->id);
        PlayerLoginManager::GetInstance().RemovePlayer(player->id); 
    }
}