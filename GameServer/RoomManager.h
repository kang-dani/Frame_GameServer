#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>
#include "GameRoom.h"

class RoomManager {
public:
    static RoomManager& Get() {
        static RoomManager inst;
        return inst;
    }

    // 새 방 생성, 생성자 자동 입장
//    std::GameRoom<Room> CreateRoom(PlayerPtr creator) {
//        std::lock_guard lock(_mtx);
//        uint64_t newId = ++_lastRoomId;
//        auto room = std::make_shared<Room>(newId, creator);
//        _rooms[newId] = room;
//        return room;
//    }
//
//    // 방 입장 시도
//    std::GameRoom<Room> JoinRoom(uint64_t roomId, PlayerPtr p, std::string& outErr) {
//        std::lock_guard lock(_mtx);
//        auto it = _rooms.find(roomId);
//        if (it == _rooms.end()) {
//            outErr = "Room not found";
//            return nullptr;
//        }
//        auto room = it->second;
//        if (room->IsFull()) {
//            outErr = "Room is full";
//            return nullptr;
//        }
//        room->AddPlayer(p);
//        return room;
//    }
//
//    // 모든 방 정보
//    std::vector<std::GameRoom<Room>> ListRooms() {
//        std::lock_guard lock(_mtx);
//        std::vector<std::GameRoom<Room>> v;
//        v.reserve(_rooms.size());
//        for (auto& [id, room] : _rooms) v.push_back(room);
//        return v;
//    }
//
//private:
//    std::mutex _mtx;
//    uint64_t _lastRoomId = 0;
//    std::unordered_map<uint64_t, std::GameRoom<Room>> _rooms;
//};