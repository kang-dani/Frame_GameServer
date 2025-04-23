#include "pch.h"
#include "ServerPacketHandler.h"
#include "PlayerIdGenerator.h"
#include "PlayerLoginManager.h"
#include "Player.h"
#include "RoomManager.h"
#include "ServerService.h"


bool Handle_INVALID(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
    return false;
}


// ��Ŷ �ڵ鷯 �ʱ�ȭ (���� ���� �� 1ȸ ȣ��)
void ServerPacketHandler::Init()
{
    // ��û ��Ŷ�� ���� �ڵ鷯 ���
    RegisterPacketHandler<Protocol::LoginRequest>(PacketID::LOGIN_REQUEST, Handle_LoginRequest);
    RegisterPacketHandler<Protocol::CreateRoomRequest>(PacketID::CREATE_ROOM_REQUEST, Handle_CreateRoomRequest);
    RegisterPacketHandler<Protocol::ListRoomsRequest>(PacketID::LIST_ROOMS_REQUEST, Handle_ListRoomsRequest);
    RegisterPacketHandler<Protocol::JoinRoomRequest>(PacketID::JOIN_ROOM_REQUEST, Handle_JoinRoomRequest);
    RegisterPacketHandler<Protocol::PlayerReadyRequest>(PacketID::PLAYER_READY_REQUEST, Handle_PlayerReadyRequest);
    RegisterPacketHandler<Protocol::PlayerMoveRequest>(PacketID::PLAYER_MOVE_REQUEST, Handle_PlayerMoveRequest);
    RegisterPacketHandler<Protocol::ChatRequest>(PacketID::CHAT_REQUEST, Handle_ChatRequest);
    RegisterPacketHandler<Protocol::InitWorldStateRequest>(PacketID::INIT_WORLD_STATE_REQUEST, Handle_InitWorldStateRequest);
    RegisterPacketHandler<Protocol::WorldStateDelta>(PacketID::WORLD_STATE_DELTA, Handle_WorldStateDelta);
    
}

bool ServerPacketHandler::Handle_LoginRequest(
    std::shared_ptr<PacketSession>& session,
    Protocol::LoginRequest& packet)
{
    auto client = std::static_pointer_cast<ClientSession>(session);

    // �� �÷��̾� ���� �� ���� ���ε�
    auto player = std::make_shared<Player>();
    player->id = PlayerIdGenerator::Generator();
    player->nickname = packet.usernickname();
    client->SetPlayer(player);
    PlayerLoginManager::GetInstance().AddPlayer(player->id, player);

    // ���� ����
    Protocol::LoginResponse resp;
    resp.set_success(true);
    resp.set_playerid(player->id);
    resp.set_usernickname(player->nickname);
    SendResponse(session, resp, PacketID::LOGIN_RESPONSE);
	printf("[Login] %s (%u)\n", player->nickname.c_str(), player->id);
    return true;
}

bool ServerPacketHandler::Handle_CreateRoomRequest(
    std::shared_ptr<PacketSession>& session,
    Protocol::CreateRoomRequest& packet)
{
    auto client = std::static_pointer_cast<ClientSession>(session);
    auto player = client->GetPlayer();
    Protocol::CreateRoomResponse resp;

    if (!player) {
        resp.set_success(false);
        resp.set_error("�α����� �ʿ��մϴ�.");
    }
    else {
        auto room = RoomManager::Get().CreateRoom(player);
        resp.set_success(true);
        resp.set_roomid(room->id);
    }
    SendResponse(session, resp, PacketID::CREATE_ROOM_RESPONSE);
	printf("[CreateRoom] %s (%u) - RoomID: %u\n", player->nickname.c_str(), player->id, resp.roomid());
    return true;
}

bool ServerPacketHandler::Handle_ListRoomsRequest(
    std::shared_ptr<PacketSession>& session,
    Protocol::ListRoomsRequest& /*packet*/)
{
    Protocol::ListRoomsResponse resp;

    // �� ��� ��ȸ
    auto rooms = RoomManager::Get().ListRooms();
    for (auto& room : rooms) 
    {
        auto* info = resp.add_rooms();

        // 1) �� �⺻ ����
        info->set_roomid(room->id);
        info->set_currentcount(static_cast<uint32_t>(room->playersInRoom.size()));
        info->set_maxcount(GameRoom::MAX_COUNT);

        // 2) �� ������ ���� (creator�� GameRoom�� ���� �ִ� ������ ������)
        if (room->creator) 
        {
            info->set_creatorid(room->creator->id);
            info->set_creatorname(room->creator->nickname);
        }

        // 3) ������ ����Ʈ
        for (auto& p : room->playersInRoom) 
        {
            auto* pi = info->add_players();
            pi->set_playerid(p->id);
            pi->set_nickname(p->nickname);
        }
    }

    // 4) ���� ����
    auto sendBuffer = MakeSendBuffer(resp);
    session->Send(sendBuffer);

    // 5) ���� �ֿܼ��� �α�
    printf("[ListRooms] %zu rooms sent\n", resp.rooms_size());
    for (int i = 0; i < resp.rooms_size(); ++i) {
        const auto& r = resp.rooms(i);
        printf("  Room %llu by %s (ID:%llu) [%u/%u]\n",
            (unsigned long long)r.roomid(),
            r.creatorname().c_str(),
            (unsigned long long)r.creatorid(),
            r.currentcount(), r.maxcount());
    }

    return true;
}

bool ServerPacketHandler::Handle_JoinRoomRequest(
    std::shared_ptr<PacketSession>& session,
    Protocol::JoinRoomRequest& packet)
{
    // 1) ������ ClientSession���� ĳ��Ʈ
    auto client = std::static_pointer_cast<ClientSession>(session);

    // 2) ���ǿ� ���ε��� Player ��������
    auto player = client->GetPlayer();

    Protocol::JoinRoomResponse resp;

    if (!player) {
        // �α��� �� �� �����̶��
        resp.set_success(false);
        resp.set_error("�α����� �ʿ��մϴ�.");
    }
    else {
        std::string err;
        // 3) RoomManager���� �ش� �� ID�� ���� �õ�
        auto room = RoomManager::Get().JoinRoom(packet.roomid(), player, err);

        if (!room) {
            // ���� ���� (�� ����, �ο� �ʰ� ��)
            resp.set_success(false);
            resp.set_error(err);
        }
        else {
            // ���� ����
            resp.set_success(true);

            // ���� ��Ŷ�� �� ���� ä���
            auto* info = resp.mutable_room();
            info->set_roomid(room->id);
            info->set_currentcount(static_cast<uint32_t>(room->playersInRoom.size()));
            info->set_maxcount(GameRoom::MAX_COUNT);

            for (auto& p : room->playersInRoom) {
                auto* pi = info->add_players();
                pi->set_playerid(p->id);
                pi->set_nickname(p->nickname);
            }
        }
    }

    // 4) ���� �ܼ� �α�
    if (resp.success()) {
        printf("[JoinRoom] player %llu joined room %llu\n",
            (unsigned long long)player->id,
            (unsigned long long)packet.roomid());
    }
    else {
        printf("[JoinRoom] failed: %s\n", resp.error().c_str());
    }

    // 5) Ŭ���̾�Ʈ���� ���� ����
    session->Send(MakeSendBuffer(resp));

    return true;
}

bool ServerPacketHandler::Handle_PlayerReadyRequest(
    std::shared_ptr<PacketSession>& session,
    Protocol::PlayerReadyRequest& packet)
{
    // 1) ���� �� �÷��̾�, �� ã��
    auto client = std::static_pointer_cast<ClientSession>(session);
    auto player = client->GetPlayer();
    auto room = RoomManager::Get().FindRoomByPlayer(player->id);
    if (!room) return false;

    // 2) �غ� ���� ������Ʈ
    room->SetReady(player->id, packet.isready());

    // 3) �غ� ���� ��ε�ĳ��Ʈ
    Protocol::PlayerReadyResponse readyResp;
    readyResp.set_roomid(room->id);
    for (auto& p : room->playersInRoom)
    {
        auto* info = readyResp.add_players();
        info->set_playerid(p->id);
        info->set_isready(room->IsReady(p->id));
    }

    auto readyBuf = PacketHandler::MakeSendBuffer(
        readyResp,
        static_cast<uint16_t>(PacketID::PLAYER_READY_RESPONSE)
    );

    for (auto& p : room->playersInRoom)
        if (auto s = p->session.lock())
            s->Send(readyBuf);

    // 4) ��� �غ������ ���� ���� �� ���� ���� �뺸
    if (!room->started && room->AllReady()) {
        room->started = true;

        // 1) �������� ����
        static thread_local std::mt19937_64 rng{ std::random_device{}() };
        auto& players = room->playersInRoom;
        std::shuffle(players.begin(), players.end(), rng);

        uint64_t selectedBossId = players.front()->id;

        std::uniform_int_distribution<int> dist(1, 3); // 1~3
        Protocol::BossType randomBossType = static_cast<Protocol::BossType>(dist(rng));

        // 2) ���� ���¿� ���� ����
        auto& bossState = room->GetWorldState().players[selectedBossId];
        bossState.isBoss = true;
        bossState.bossType = randomBossType;

        // 3) GameStartNotification ����
        Protocol::GameStartNotification start;
        start.set_roomid(room->id);
        start.set_boss_type(randomBossType);

        for (auto& p : players) {
            auto* pi = start.add_players();
            pi->set_playerid(p->id);
            pi->set_nickname(p->nickname);

            Protocol::Role role = (p->id == selectedBossId) ? Protocol::ROLE_BOSS : Protocol::ROLE_SURVIVOR;
            start.add_playerroles(role);
        }

        // 4) ���� ��� ���ǿ� �뺸 + �ʱ� ���� ���� ����
        for (auto& p : players) {
            if (auto s = p->session.lock()) {
                s->Send(PacketHandler::MakeSendBuffer(start,
                    static_cast<uint16_t>(PacketID::ROLE_ASSIGNMENT_NOTIFICATION)));
                room->SendInitWorldState(s);
            }
        }

        printf("[GameStart] room %llu start: boss=%llu, type=%d\n",
            static_cast<unsigned long long>(room->id),
            static_cast<unsigned long long>(selectedBossId),
            static_cast<int>(randomBossType));
    }

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

bool ServerPacketHandler::Handle_InitWorldStateRequest(shared_ptr<PacketSession>& session, Protocol::InitWorldStateRequest& packet)
{
    return false;
}

bool ServerPacketHandler::Handle_WorldStateDelta(shared_ptr<PacketSession>& session, Protocol::WorldStateDelta& packet)
{
	return false;
}