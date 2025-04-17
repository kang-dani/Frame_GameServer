#pragma once
#include <PacketHandler.h>
#include "Protocol.pb.h" // Protobuf에서 생성된 헤더 파일

// 패킷 ID 정의
enum class PacketID : uint16
{
    // 클라이언트 → 서버 패킷
    LOGIN_REQUEST = 1001,
    ENTER_GAME_REQUEST = 1101,
    PLAYER_MOVE_REQUEST = 1201,
    CHAT_REQUEST = 1301,
    ACTION_REQUEST = 1401,

    // 서버 → 클라이언트 응답 패킷
    LOGIN_RESPONSE = 1002,
    ENTER_GAME_RESPONSE = 1102,
    PLAYER_MOVE_RESPONSE = 1202,
    CHAT_RESPONSE = 1302,
    ACTION_RESPONSE = 1402,
    WORLD_STATE_UPDATE = 1501
};

// 서버로 요청을 보내는 함수 선언
class ClientPacketHandler : public PacketHandler
{
public:
    // 서버 → 클라이언트 응답 수신 처리 (Recv 핸들러 등록)
    static void Init();

    // 패킷 수신 핸들러 함수
    static bool Handle_LoginResponse(shared_ptr<PacketSession>& session, Protocol::LoginResponse& packet);
    static bool Handle_EnterGameResponse(shared_ptr<PacketSession>& session, Protocol::EnterGameResponse& packet);
    static bool Handle_PlayerMoveResponse(shared_ptr<PacketSession>& session, Protocol::PlayerMoveResponse& packet);
    static bool Handle_ChatResponse(shared_ptr<PacketSession>& session, Protocol::ChatResponse& packet);
    static bool Handle_ActionResponse(shared_ptr<PacketSession>& session, Protocol::ActionResponse& packet);
    static bool Handle_WorldStateUpdate(shared_ptr<PacketSession>& session, Protocol::ActionResponse& packet);

public:
    // 클라이언트 → 서버 패킷 전송 (Send)
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::LoginRequest& packet)
    {
        return MakeSendBuffer(packet, PacketID::LOGIN_REQUEST);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::EnterGameRequest& packet)
    {
        return MakeSendBuffer(packet, PacketID::ENTER_GAME_REQUEST);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::PlayerMoveRequest& packet)
    {
        return MakeSendBuffer(packet, PacketID::PLAYER_MOVE_REQUEST);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::ChatRequest& packet)
    {
        return MakeSendBuffer(packet, PacketID::CHAT_REQUEST);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::ActionRequest& packet)
    {
        return MakeSendBuffer(packet, PacketID::ACTION_REQUEST);
    }
public:
    template<typename T>
    static shared_ptr<SendBuffer> MakeSendBuffer(T& packet, PacketID packetId)
    {
        // enum class -> uint16 변환 후 protected 호출
        return PacketHandler::MakeSendBuffer(packet, static_cast<uint16_t>(packetId));
    }

    template<typename PacketType, typename Handler>
    static void RegisterPacketHandler(PacketID packetId, Handler handler)
    {
        PacketHandler::RegisterPacketHandler<PacketType>(static_cast<uint16_t>(packetId), handler);
    }
};
