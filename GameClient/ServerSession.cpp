#include "pch.h"
#include "ServerSession.h"

#include "Protocol.pb.h"
#include "ClientPacketHandler.h"

void ServerSession::OnConnected()
{
    // 클라이언트에서 로그인 정보를 입력받아 패킷 생성
    Protocol::LoginRequest packet;
    packet.set_user_id("player1");               // 사용자 이름
    packet.set_token("debug-token-12345");        // 임시 토큰 (이후 실제 토큰 사용)

    auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
    Send(sendBuffer);
}

int ServerSession::OnRecvPacket(BYTE* buffer, int len)
{
    shared_ptr<PacketSession> session = GetPacketSession();
    ClientPacketHandler::HandlePacket(session, buffer, len);

    return len;
}

void ServerSession::OnSend(int len)
{
}

void ServerSession::OnDisconnected()
{
}
