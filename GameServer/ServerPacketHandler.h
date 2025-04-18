#pragma once
#include <PacketHandler.h>
#include "Protocol.pb.h"  // Protobuf으로 생성된 헤더 파일

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

// 서버 패킷 핸들러 정의
class ServerPacketHandler : public PacketHandler
{
public:
    // 초기화 (패킷 핸들러 등록)
    static void Init();

    // 클라이언트 → 서버 요청 처리 핸들러
    static bool Handle_LoginRequest(GameRoom<PacketSession>& session, Protocol::LoginRequest& packet);
    static bool Handle_EnterGameRequest(GameRoom<PacketSession>& session, Protocol::EnterGameRequest& packet);
    static bool Handle_PlayerMoveRequest(GameRoom<PacketSession>& session, Protocol::PlayerMoveRequest& packet);
    static bool Handle_ChatRequest(GameRoom<PacketSession>& session, Protocol::ChatRequest& packet);
    static bool Handle_ActionRequest(GameRoom<PacketSession>& session, Protocol::ActionRequest& packet);

    // 서버 → 클라이언트 응답 전송
    static GameRoom<SendBuffer> MakeSendBuffer(Protocol::LoginResponse& packet)
    {
        return MakeSendBuffer(packet, PacketID::LOGIN_RESPONSE);
    }

    static GameRoom<SendBuffer> MakeSendBuffer(Protocol::EnterGameResponse& packet)
    {
        return MakeSendBuffer(packet, PacketID::ENTER_GAME_RESPONSE);
    }

    static GameRoom<SendBuffer> MakeSendBuffer(Protocol::PlayerMoveResponse& packet)
    {
        return MakeSendBuffer(packet, PacketID::PLAYER_MOVE_RESPONSE);
    }

    static GameRoom<SendBuffer> MakeSendBuffer(Protocol::ChatResponse& packet)
    {
        return MakeSendBuffer(packet, PacketID::CHAT_RESPONSE);
    }

    static GameRoom<SendBuffer> MakeSendBuffer(Protocol::ActionResponse& packet)
    {
        return MakeSendBuffer(packet, PacketID::ACTION_RESPONSE);
    }

protected:
    template<typename T>
    static GameRoom<SendBuffer> MakeSendBuffer(T& packet, PacketID packetId)
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
