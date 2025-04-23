#pragma once
#include <PacketHandler.h>
#include "Protocol.pb.h" // Protobuf���� ������ ��� ����

// ��Ŷ ID ����
enum class PacketID : uint16
{
    // Ŭ���̾�Ʈ �� ���� ��Ŷ
    LOGIN_REQUEST = 1001,
    ENTER_GAME_REQUEST = 1101,
    PLAYER_MOVE_REQUEST = 1201,
    CHAT_REQUEST = 1301,
    ACTION_REQUEST = 1401,

    // ���� �� Ŭ���̾�Ʈ ���� ��Ŷ
    LOGIN_RESPONSE = 1002,
    ENTER_GAME_RESPONSE = 1102,
    PLAYER_MOVE_RESPONSE = 1202,
    CHAT_RESPONSE = 1302,
    ACTION_RESPONSE = 1402,
    WORLD_STATE_UPDATE = 1501
};

// ������ ��û�� ������ �Լ� ����
class ClientPacketHandler : public PacketHandler
{
public:
    // ���� �� Ŭ���̾�Ʈ ���� ���� ó�� (Recv �ڵ鷯 ���)
    static void Init();

    // ��Ŷ ���� �ڵ鷯 �Լ�
    static bool Handle_LoginResponse(std::shared_ptr<PacketSession>& session, Protocol::LoginResponse& packet);
    static bool Handle_ChatResponse(std::shared_ptr<PacketSession>& session, Protocol::ChatResponse& packet);
    static bool Handle_InitWorldStateResponse(std::shared_ptr<PacketSession>& session, Protocol::InitWorldStateRequest& packet);

public:
    // Ŭ���̾�Ʈ �� ���� ��Ŷ ���� (Send)
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::LoginRequest& packet)
    {
        return MakeSendBuffer(packet, PacketID::LOGIN_REQUEST);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::ChatRequest& packet)
    {
        return MakeSendBuffer(packet, PacketID::CHAT_REQUEST);
    }

public:
    template<typename T>
    static shared_ptr<SendBuffer> MakeSendBuffer(T& packet, PacketID packetId)
    {
        // enum class -> uint16 ��ȯ �� protected ȣ��
        return PacketHandler::MakeSendBuffer(packet, static_cast<uint16_t>(packetId));
    }

    template<typename PacketType, typename Handler>
    static void RegisterPacketHandler(PacketID packetId, Handler handler)
    {
        PacketHandler::RegisterPacketHandler<PacketType>(static_cast<uint16_t>(packetId), handler);
    }
};
