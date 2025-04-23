#pragma once
#include "SessionManager.h"
#include "RoomManager.h"
#include "PlayerLoginManager.h"
#include <PacketSession.h>
#include <memory>
#include "Player.h"

#include "ServerPacketHandler.h"

class Player;

class ClientSession	 : public PacketSession
{
public:
	~ClientSession() { printf("~ClientSession\n"); }
public:
	//ClientSession 주소 넘겨주기
	shared_ptr<ClientSession> GetClientSession() { return static_pointer_cast<ClientSession>(shared_from_this()); }

    // 로그인 성공 시 여기에 Player 붙여두기
    void SetPlayer(const std::shared_ptr<Player>& player)
    {
        _player = player;
    }
    // 방 생성 등에서 Player 꺼내쓰기
    std::shared_ptr<Player> GetPlayer() const
    {
        return _player.lock();
    }

public:
	virtual void OnConnected() override;
	virtual int OnRecvPacket(BYTE* buffer, int len) override;
	virtual void OnSend(int len) override;
	virtual void OnDisconnected() override;

private:
    // 세션 ↔ 플레이어 매핑
    std::weak_ptr<Player> _player;
};