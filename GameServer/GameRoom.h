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
	std::vector<PlayerPtr> playersInRoom;			//현재 방에 들어와 있는 인원
	static constexpr uint32_t MAX_COUNT = 5;		// 방에 허용된 최대 인원
	
	GameRoom(uint64_t roomID, PlayerPtr creator) : id(roomID)
	{
		playersInRoom.push_back(creator);
	}

	bool IsFull() const { return playersInRoom.size() >= MAX_COUNT; }
	void AddPlayer(PlayerPtr p) { playersInRoom.push_back(p); }


};