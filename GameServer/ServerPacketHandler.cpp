#include "pch.h"
#include "ServerPacketHandler.h"
#include "PlayerIdGenerator.h"
#include "PlayerLoginManager.h"
#include "Player.h"


bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
    return false;
}


// 패킷 핸들러 초기화 (서버 부팅 시 1회 호출)
void ServerPacketHandler::Init()
{
    // 요청 패킷에 대한 핸들러 등록
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
   printf("플레이어 접속 완료\n");
   printf("Player ID: %u, Nickname: %s\n", player->id, player->name.c_str());

   // 서버 플레이어 목록에 등록  
   PlayerLoginManager::GetInstance().AddPlayer(player->id, player);  

   Protocol::LoginResponse response;  
   response.set_success(true);  
   response.set_error_message("null");  
   response.set_playerid(player->id);  
   response.set_usernickname(player->name);  

   // 5. 응답 전송  
   auto sendBuffer = MakeSendBuffer(response);  
   session->Send(sendBuffer);  

   return response.success();  
}



// 게임 입장 요청
bool ServerPacketHandler::Handle_EnterGameRequest(shared_ptr<PacketSession>& session, Protocol::EnterGameRequest& packet)
{

    return true;
}

// 플레이어 이동 요청 처리
bool ServerPacketHandler::Handle_PlayerMoveRequest(shared_ptr<PacketSession>& session, Protocol::PlayerMoveRequest& packet)
{

    return true;
}

// 채팅 요청 처리
bool ServerPacketHandler::Handle_ChatRequest(shared_ptr<PacketSession>& session, Protocol::ChatRequest& packet)
{

    return true;
}

// 액션 요청 처리
bool ServerPacketHandler::Handle_ActionRequest(shared_ptr<PacketSession>& session, Protocol::ActionRequest& packet)
{

    return true;
}
