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


// 패킷 핸들러 초기화 (서버 부팅 시 1회 호출)
void ServerPacketHandler::Init()
{
    // 요청 패킷에 대한 핸들러 등록
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

    // 새 플레이어 생성 및 세션 바인딩
    auto player = std::make_shared<Player>();
    player->id = PlayerIdGenerator::Generator();
    player->nickname = packet.usernickname();
    client->SetPlayer(player);
    PlayerLoginManager::GetInstance().AddPlayer(player->id, player);

    // 응답 세팅
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
        resp.set_error("로그인이 필요합니다.");
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

    // 방 목록 순회
    auto rooms = RoomManager::Get().ListRooms();
    for (auto& room : rooms) 
    {
        auto* info = resp.add_rooms();

        // 1) 방 기본 정보
        info->set_roomid(room->id);
        info->set_currentcount(static_cast<uint32_t>(room->playersInRoom.size()));
        info->set_maxcount(GameRoom::MAX_COUNT);

        // 2) 방 생성자 정보 (creator는 GameRoom이 갖고 있던 생성자 포인터)
        if (room->creator) 
        {
            info->set_creatorid(room->creator->id);
            info->set_creatorname(room->creator->nickname);
        }

        // 3) 참가자 리스트
        for (auto& p : room->playersInRoom) 
        {
            auto* pi = info->add_players();
            pi->set_playerid(p->id);
            pi->set_nickname(p->nickname);
        }
    }

    // 4) 응답 전송
    auto sendBuffer = MakeSendBuffer(resp);
    session->Send(sendBuffer);

    // 5) 서버 콘솔에도 로그
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
    // 1) 세션을 ClientSession으로 캐스트
    auto client = std::static_pointer_cast<ClientSession>(session);

    // 2) 세션에 바인딩된 Player 가져오기
    auto player = client->GetPlayer();

    Protocol::JoinRoomResponse resp;

    if (!player) {
        // 로그인 안 된 세션이라면
        resp.set_success(false);
        resp.set_error("로그인이 필요합니다.");
    }
    else {
        std::string err;
        // 3) RoomManager에서 해당 방 ID로 참가 시도
        auto room = RoomManager::Get().JoinRoom(packet.roomid(), player, err);

        if (!room) {
            // 참가 실패 (방 없음, 인원 초과 등)
            resp.set_success(false);
            resp.set_error(err);
        }
        else {
            // 참가 성공
            resp.set_success(true);

            // 응답 패킷에 방 정보 채우기
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

    // 4) 서버 콘솔 로그
    if (resp.success()) {
        printf("[JoinRoom] player %llu joined room %llu\n",
            (unsigned long long)player->id,
            (unsigned long long)packet.roomid());
    }
    else {
        printf("[JoinRoom] failed: %s\n", resp.error().c_str());
    }

    // 5) 클라이언트에게 응답 전송
    session->Send(MakeSendBuffer(resp));

    return true;
}

bool ServerPacketHandler::Handle_PlayerReadyRequest(
    std::shared_ptr<PacketSession>& session,
    Protocol::PlayerReadyRequest& packet)
{
    // 1) 세션 → 플레이어, 방 찾기
    auto client = std::static_pointer_cast<ClientSession>(session);
    auto player = client->GetPlayer();
    auto room = RoomManager::Get().FindRoomByPlayer(player->id);
    if (!room) return false;

    // 2) 준비 상태 업데이트
    room->SetReady(player->id, packet.isready());

    // 3) 준비 상태 브로드캐스트
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

    // 4) 모두 준비됐으면 역할 배정 후 게임 시작 통보
    if (!room->started && room->AllReady()) {
        room->started = true;

        // 1) 무작위로 섞기
        static thread_local std::mt19937_64 rng{ std::random_device{}() };
        auto& players = room->playersInRoom;
        std::shuffle(players.begin(), players.end(), rng);

        uint64_t selectedBossId = players.front()->id;

        std::uniform_int_distribution<int> dist(1, 3); // 1~3
        Protocol::BossType randomBossType = static_cast<Protocol::BossType>(dist(rng));

        // 2) 월드 상태에 보스 설정
        auto& bossState = room->GetWorldState().players[selectedBossId];
        bossState.isBoss = true;
        bossState.bossType = randomBossType;

        // 3) GameStartNotification 구성
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

        // 4) 방의 모든 세션에 통보 + 초기 월드 상태 전송
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

bool ServerPacketHandler::Handle_InitWorldStateRequest(shared_ptr<PacketSession>& session, Protocol::InitWorldStateRequest& packet)
{
    return false;
}

bool ServerPacketHandler::Handle_WorldStateDelta(shared_ptr<PacketSession>& session, Protocol::WorldStateDelta& packet)
{
	return false;
}