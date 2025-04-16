#include "pch.h"
#include "ServerSession.h"

#include "Protocol.pb.h"
#include "ServerPacketHandler.h"

void ServerSession::OnConnected()
{
    Protocol::C_LOGIN packet;
    auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
    Send(sendBuffer);
    
}

int ServerSession::OnRecvPacket(BYTE* buffer, int len)
{
    shared_ptr<PacketSession> session = GetPacketSession();
    ServerPacketHandler::HandlePacket(session, buffer, len);

    return len;
}

void ServerSession::OnSend(int len)
{
}

void ServerSession::OnDisconnected()
{
}
