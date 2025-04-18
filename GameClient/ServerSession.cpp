#include "pch.h"
#include "ServerSession.h"

#include "Protocol.pb.h"
#include "ClientPacketHandler.h"

void ServerSession::OnConnected()
{
    // Ŭ���̾�Ʈ���� �α��� ������ �Է¹޾� ��Ŷ ����
    Protocol::LoginRequest packet;
	packet.set_usernickname(packet.usernickname());

    auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
    Send(sendBuffer);
}

int ServerSession::OnRecvPacket(BYTE* buffer, int len)
{
    GameRoom<PacketSession> session = GetPacketSession();
    ClientPacketHandler::HandlePacket(session, buffer, len);

    return len;
}

void ServerSession::OnSend(int len)
{
}

void ServerSession::OnDisconnected()
{
}
