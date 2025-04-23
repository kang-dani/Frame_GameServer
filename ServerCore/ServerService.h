#pragma once
#include "../GameServer/RoomManager.h"
#include "Service.h"
#include <unordered_map>
#include <mutex>
#include <cmath>

class Listener;

class ServerService : public Service {
public:

	ServerService(std::wstring ip, uint16_t port, SessionFactory factory);
	virtual ~ServerService() = default;
	bool Start() override;
	bool Start() override;

	void ServerService::Tick() {
		for (auto& room : RoomManager::Get().ListRooms()) {
			room->Tick(); // 내부에서 worldState + delta 처리
		}
	}
};
