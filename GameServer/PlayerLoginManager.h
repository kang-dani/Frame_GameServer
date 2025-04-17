#pragma once
#include <unordered_map>
#include <memory>
#include "Player.h"

class PlayerLoginManager
{
public:
    static LobbyManager& GetInstance()
    {
        static PlayerLoginManager instance;
        return instance;
    }

    void AddPlayer(uint64 playerId, std::shared_ptr<Player> player)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _players[playerId] = player;
    }

    std::shared_ptr<Player> GetPlayer(uint64 playerId)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _players.find(playerId);
        if (it != _players.end())
            return it->second;
        return nullptr;
    }

    void RemovePlayer(uint64 playerId)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _players.erase(playerId);
    }

    const std::unordered_map<uint64, std::shared_ptr<Player>>& GetAllPlayers() const { return _players; }

private:
    std::unordered_map<uint64, std::shared_ptr<Player>> _players;
    std::mutex _mutex;

    PlayerLoginManager() = default;
    ~LobbyManager() = default;
};
