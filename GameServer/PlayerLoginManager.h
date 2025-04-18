#pragma once
#include <unordered_map>
#include "pch.h"
#include <memory>
#include "Player.h"
#include <mutex>

class PlayerLoginManager
{
public:
    static PlayerLoginManager& GetInstance()
    {
        static PlayerLoginManager instance;
        return instance;
    }

    void AddPlayer(uint32 playerId, shared_ptr<Player> player)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _players[playerId] = player;
    }

    std::shared_ptr<Player> GetPlayer(uint32 playerId)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _players.find(playerId);
        if (it != _players.end())
            return it->second;
        return nullptr;
    }

    void RemovePlayer(uint32 playerId)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _players.erase(playerId);
    }

    const std::unordered_map<uint32, std::shared_ptr<Player>>& GetAllPlayers() const { return _players; }

private:
    std::unordered_map<uint32, std::shared_ptr<Player>> _players;
    std::mutex _mutex;

    PlayerLoginManager() = default;
    ~PlayerLoginManager() = default;
};
