#pragma once

#include <unordered_map>
#include <memory> // Ensure this is included for std::shared_ptr
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

   void AddPlayer(int playerId, std::shared_ptr<Player> player)
   {
       std::lock_guard<std::mutex> lock(_mutex);
       _players[playerId] = player;
   }

   std::shared_ptr<Player> GetPlayer(int playerId)
   {
       std::lock_guard<std::mutex> lock(_mutex);
       auto it = _players.find(playerId);
       if (it != _players.end())
           return it->second;
       return nullptr;
   }

   void RemovePlayer(int playerId)
   {
       std::lock_guard<std::mutex> lock(_mutex);
       _players.erase(playerId);
   }

   const std::unordered_map<int, std::shared_ptr<Player>>& GetAllPlayers() const { return _players; }

private:
   std::unordered_map<int, std::shared_ptr<Player>> _players;
   std::mutex _mutex;

   PlayerLoginManager() = default;
   ~PlayerLoginManager() = default;
};
