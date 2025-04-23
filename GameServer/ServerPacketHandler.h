#pragma once
#include "GameRoom.h"
#include "RoomManager.h"
#include <PacketHandler.h>
#include "Player.h"
#include "Protocol.pb.h"  // Protobuf���� ������ ��� ����

// ��Ŷ ID ����
enum class PacketID : int
{
    // ���� Ŭ���̾�Ʈ �� ���� ��û ��������������������������������������������������
    LOGIN_REQUEST = 1001,
    CREATE_ROOM_REQUEST = 1101,
    LIST_ROOMS_REQUEST = 1201,
    JOIN_ROOM_REQUEST = 1301,
    PLAYER_READY_REQUEST = 1401,
    ENTER_GAME_REQUEST = 1501,
    PLAYER_MOVE_REQUEST = 1601,
    CHAT_REQUEST = 1701,
    ACTION_REQUEST = 1801,

    // ���� ���� �� Ŭ���̾�Ʈ ����/�˸� ����������������������������������������
    LOGIN_RESPONSE = 1002,
    CREATE_ROOM_RESPONSE = 1202,
    LIST_ROOMS_RESPONSE = 1302,
    JOIN_ROOM_RESPONSE = 1402,
    PLAYER_READY_RESPONSE = 1802,
    ROLE_ASSIGNMENT_NOTIFICATION = 1702,
    GAME_START_NOTIFICATION = 1902,
    ENTER_GAME_RESPONSE = 2002,
    PLAYER_MOVE_RESPONSE = 2102,
    CHAT_RESPONSE = 2202,
    ACTION_RESPONSE = 2302,

    // ���� ���� ����ȭ ��������������������������������������������������������������������������
    INIT_WORLD_STATE_REQUEST = 2401,  // InitWorldStateRequest
    WORLD_STATE_DELTA = 2402,  // WorldStateDelta

};

// ���� ��Ŷ �ڵ鷯 ����
class ServerPacketHandler : public PacketHandler
{
public:
    // �ʱ�ȭ (��Ŷ �ڵ鷯 ���)
    static void Init();

    // Ŭ���̾�Ʈ �� ���� ��û ó�� �ڵ鷯
    static bool Handle_LoginRequest         (shared_ptr<PacketSession>& session, Protocol::LoginRequest& packet);
    static bool Handle_CreateRoomRequest    (shared_ptr<PacketSession>& session, Protocol::CreateRoomRequest& packet);
    static bool Handle_ListRoomsRequest     (shared_ptr<PacketSession>& session, Protocol::ListRoomsRequest& packet);
    static bool Handle_JoinRoomRequest      (shared_ptr<PacketSession>& session, Protocol::JoinRoomRequest& packet);
	static bool Handle_PlayerReadyRequest   (shared_ptr<PacketSession>& session, Protocol::PlayerReadyRequest& packet);
	static bool Handle_GameStartNotification(shared_ptr<PacketSession>& session, Protocol::GameStartNotification& packet);
    static bool Handle_PlayerMoveRequest    (shared_ptr<PacketSession>& session, Protocol::PlayerMoveRequest& packet);
    static bool Handle_ChatRequest          (shared_ptr<PacketSession>& session, Protocol::ChatRequest& packet);
	static bool Handle_InitWorldStateRequest(shared_ptr<PacketSession>& session, Protocol::InitWorldStateRequest& packet);
	static bool Handle_WorldStateDelta      (shared_ptr<PacketSession>& session, Protocol::WorldStateDelta& packet);

    // ������ �޽����� �÷��̾� ���� ä���
    static void FillPlayerInfo(::Protocol::RoomInfo* info, const std::shared_ptr<GameRoom>& room)
    {
        info->set_roomid(room->id);
        info->set_currentcount((uint32_t)room->playersInRoom.size());
        info->set_maxcount(5);
        for (std::vector<PlayerPtr>::iterator it = room->playersInRoom.begin(); it != room->playersInRoom.end(); ++it)
        {
            PlayerPtr& p = *it;
            if (!p) continue;

            Protocol::PlayerInfo* pi = info->add_players();
            pi->set_playerid(p->id);
            pi->set_nickname(p->nickname);
        }
    }


    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::WorldStateDelta& packet)
    {
        return MakeBuf(packet, PacketID::WORLD_STATE_DELTA);
    }

	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::InitWorldStateRequest& packet)
	{
		return MakeBuf(packet, PacketID::INIT_WORLD_STATE_REQUEST);
	}
  
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::LoginResponse& packet)
    {
        return MakeBuf(packet, PacketID::LOGIN_RESPONSE);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::CreateRoomResponse& packet)
    {
        return MakeBuf(packet, PacketID::CREATE_ROOM_RESPONSE);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::ListRoomsResponse& packet)
    {
        return MakeBuf(packet, PacketID::LIST_ROOMS_RESPONSE);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::JoinRoomResponse& packet)
    {
        return MakeBuf(packet, PacketID::JOIN_ROOM_RESPONSE);
    }

    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::PlayerReadyResponse& packet)
    {
        return MakeBuf(packet, PacketID::PLAYER_READY_RESPONSE);
    }

	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::GameStartNotification& packet)
	{
		return MakeBuf(packet, PacketID::GAME_START_NOTIFICATION);
	}
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::ChatResponse& packet)
    {
        return MakeBuf(packet, PacketID::CHAT_RESPONSE);
    }

    

protected:
private:
    // ���� ����: T�� protobuf �޽��� Ÿ��
    template<typename T>
    static auto MakeBuf(const T& msg, PacketID pid) {
        return PacketHandler::MakeSendBuffer(msg, static_cast<uint16_t>(pid));
    }

    template<typename Resp>
    static void SendResponse(std::shared_ptr<PacketSession> sess,
        const Resp& resp, PacketID pid)
    {
        sess->Send(MakeBuf(resp, pid));
    }

    template<typename PacketType, typename Handler>
    static void RegisterPacketHandler(PacketID packetId, Handler handler)
    {
        PacketHandler::RegisterPacketHandler<PacketType>(static_cast<uint16_t>(packetId), handler);
    }

    template<typename Msg> 
    static void BroadcastToRoom(const std::shared_ptr<GameRoom>& room, const Msg& msg, PacketID pid)
    {
        // �� ���� ���۸� �����
        auto buf = PacketHandler::MakeSendBuffer(msg, static_cast<uint16_t>(pid));
        // �濡 ���� ��� �÷��̾� ���ǿ� �ѷ��ش�
        for (auto& p : room->playersInRoom) {
            if (auto s = p->session.lock())
                s->Send(buf);
        }
    }
};
