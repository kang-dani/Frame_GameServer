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
	//ClientSession �ּ� �Ѱ��ֱ�
	shared_ptr<ClientSession> GetClientSession() { return static_pointer_cast<ClientSession>(shared_from_this()); }

    // �α��� ���� �� ���⿡ Player �ٿ��α�
    void SetPlayer(const std::shared_ptr<Player>& player)
    {
        _player = player;
    }
    // �� ���� ��� Player ��������
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
    // ���� �� �÷��̾� ����
    std::weak_ptr<Player> _player;
};