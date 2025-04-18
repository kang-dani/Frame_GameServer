#include "pch.h"
#include "ServerPacketHandler.h"
#include "PlayerIdGenerator.h"
#include "PlayerLoginManager.h"
#include "Player.h"


bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
    return false;
}


// ��Ŷ �ڵ鷯 �ʱ�ȭ (���� ���� �� 1ȸ ȣ��)
void ServerPacketHandler::Init()
{
    // ��û ��Ŷ�� ���� �ڵ鷯 ���
    RegisterPacketHandler<Protocol::LoginRequest>(PacketID::LOGIN_REQUEST, Handle_LoginRequest);
    RegisterPacketHandler<Protocol::EnterGameRequest>(PacketID::ENTER_GAME_REQUEST, Handle_EnterGameRequest);
    RegisterPacketHandler<Protocol::PlayerMoveRequest>(PacketID::PLAYER_MOVE_REQUEST, Handle_PlayerMoveRequest);
    RegisterPacketHandler<Protocol::ChatRequest>(PacketID::CHAT_REQUEST, Handle_ChatRequest);
    RegisterPacketHandler<Protocol::ActionRequest>(PacketID::ACTION_REQUEST, Handle_ActionRequest);
}

bool ServerPacketHandler::Handle_LoginRequest(shared_ptr<PacketSession>& session, Protocol::LoginRequest& packet)
{  
   shared_ptr<Player> player = make_shared<Player>();
   player->id = PlayerIdGenerator::Generator();  
   player->name = packet.usernickname();
   player->session = static_pointer_cast<ClientSession>(session); 
   printf("�÷��̾� ���� �Ϸ�\n");
   printf("Player ID: %u, Nickname: %s\n", player->id, player->name.c_str());

   // ���� �÷��̾� ��Ͽ� ���  
   PlayerLoginManager::GetInstance().AddPlayer(player->id, player);  

   Protocol::LoginResponse response;  
   response.set_success(true);  
   response.set_error_message("null");  
   response.set_playerid(player->id);  
   response.set_usernickname(player->name);  

   // 5. ���� ����  
   auto sendBuffer = MakeSendBuffer(response);  
   session->Send(sendBuffer);  

   return response.success();  
}



// ���� ���� ��û
bool ServerPacketHandler::Handle_EnterGameRequest(shared_ptr<PacketSession>& session, Protocol::EnterGameRequest& packet)
{

    return true;
}

// �÷��̾� �̵� ��û ó��
bool ServerPacketHandler::Handle_PlayerMoveRequest(shared_ptr<PacketSession>& session, Protocol::PlayerMoveRequest& packet)
{

    return true;
}

// ä�� ��û ó��
bool ServerPacketHandler::Handle_ChatRequest(shared_ptr<PacketSession>& session, Protocol::ChatRequest& packet)
{

    return true;
}

// �׼� ��û ó��
bool ServerPacketHandler::Handle_ActionRequest(shared_ptr<PacketSession>& session, Protocol::ActionRequest& packet)
{

    return true;
}
