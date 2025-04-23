#include "pch.h"
#include "ClientPacketHandler.h"

bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
    return false;
}

void ClientPacketHandler::Init()
{
    // 패킷 ID와 핸들러를 매핑 (서버 → 클라이언트 응답 패킷)
    RegisterPacketHandler<Protocol::LoginResponse>(PacketID::LOGIN_RESPONSE, Handle_LoginResponse);
    RegisterPacketHandler<Protocol::ChatResponse>(PacketID::CHAT_RESPONSE, Handle_ChatResponse);
}

bool ClientPacketHandler::Handle_LoginResponse(shared_ptr<PacketSession>& session, Protocol::LoginResponse& packet)
{
    if (packet.success()) {
        // 1. 로그인 성공 처리
        printf("Login successful! Welcome to the game.\n");

        // 씬 전환 or 로비 이동
        //GameSceneManager::LoadScene("LobbyScene");
    }
    else {
        // 2. 로그인 실패 처리
        printf("Login failed: %s\n", packet.error_message().c_str());

        // 실패 메시지를 UI 팝업으로 표시
        //GameUI::ShowErrorPopup(packet.error_message());
    }

    return true;
}


bool ClientPacketHandler::Handle_ChatResponse(shared_ptr<PacketSession>& session, Protocol::ChatResponse& packet)
{
    return true;
}

bool ClientPacketHandler::Handle_InitWorldStateResponse(std::shared_ptr<PacketSession>& session, Protocol::InitWorldStateRequest& packet)
{
    return false;
}

