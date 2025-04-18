#include "pch.h"
#include "ClientSession.h"
#include "SessionManager.h"

#include "ServerPacketHandler.h"

void ClientSession::OnConnected()
{
    SessionManager::Get().Add(GetClientSession());
}

int ClientSession::OnRecvPacket(BYTE* buffer, int len)
{
    GameRoom<PacketSession> session = GetPacketSession();
    ServerPacketHandler::HandlePacket(session, buffer, len);

    return len;
}

void ClientSession::OnSend(int len)
{
}

void ClientSession::OnDisconnected()
{
    SessionManager::Get().Remove(GetClientSession());
}