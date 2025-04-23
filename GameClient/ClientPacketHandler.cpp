#include "pch.h"
#include "ClientPacketHandler.h"

bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
    return false;
}

void ClientPacketHandler::Init()
{
    // ��Ŷ ID�� �ڵ鷯�� ���� (���� �� Ŭ���̾�Ʈ ���� ��Ŷ)
    RegisterPacketHandler<Protocol::LoginResponse>(PacketID::LOGIN_RESPONSE, Handle_LoginResponse);
    RegisterPacketHandler<Protocol::ChatResponse>(PacketID::CHAT_RESPONSE, Handle_ChatResponse);
}

bool ClientPacketHandler::Handle_LoginResponse(shared_ptr<PacketSession>& session, Protocol::LoginResponse& packet)
{
    if (packet.success()) {
        // 1. �α��� ���� ó��
        printf("Login successful! Welcome to the game.\n");

        // �� ��ȯ or �κ� �̵�
        //GameSceneManager::LoadScene("LobbyScene");
    }
    else {
        // 2. �α��� ���� ó��
        printf("Login failed: %s\n", packet.error_message().c_str());

        // ���� �޽����� UI �˾����� ǥ��
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

