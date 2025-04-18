#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include "Player.h"
#include "Protocol.pb.h"

using PlayerPtr = std::shared_ptr<Player>;

struct GameRoom
{
	uint64_t id;
	std::vector<PlayerPtr> playersInRoom;			//���� �濡 ���� �ִ� �ο�
	static constexpr uint32_t MAX_COUNT = 5;		// �濡 ���� �ִ� �ο�
	
	GameRoom(uint64_t roomID, PlayerPtr creator) : id(roomID)
	{
		playersInRoom.push_back(creator);
	}

	bool IsFull() const { return playersInRoom.size() >= MAX_COUNT; }
	void AddPlayer(PlayerPtr p) { playersInRoom.push_back(p); }


};