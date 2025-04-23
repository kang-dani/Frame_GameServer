#pragma once
#include "GameRoom.h"
#include "ServerPacketHandler.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>


class RoomManager 
{
public:
    static RoomManager& Get() 
    {
        static RoomManager inst;
        return inst;
    }

    // �� ����: ������(Player)�� �Ѱ��ָ� ���ο��� ID�� �Ź� �������� �߱�
    std::shared_ptr<GameRoom> CreateRoom(const std::shared_ptr<Player>& owner) {
        std::lock_guard<std::mutex> lk(_mtx);
        uint64_t newId = _nextRoomId++;  // ���⼭ ID�� ä��
        auto room = std::make_shared<GameRoom>(newId, owner);
        _rooms[newId] = room;
        return room;
    }

   // �� ���� �õ�  
   std::shared_ptr<GameRoom> JoinRoom(uint64_t roomId, PlayerPtr p, std::string& outErr) 
   {  
       std::lock_guard<std::mutex> lock(_mtx); 
       auto it = _rooms.find(roomId);  
       if (it == _rooms.end()) {  
           outErr = "Room not found";  
           return nullptr;  
       }  
       auto room = it->second;  
       if (room->IsFull()) {  
           outErr = "Room is full";  
           return nullptr;  
       }  
       room->AddPlayer(p);  
       return room;  
   }  

   // ��� �� ����  
   std::vector<std::shared_ptr<GameRoom>> ListRooms() 
   {  
       std::lock_guard<std::mutex> lock(_mtx); 
       std::vector<std::shared_ptr<GameRoom>> v;  
       v.reserve(_rooms.size());  
       for (const auto& pair : _rooms) {
           // pair.first == id, pair.second == room
           v.push_back(pair.second);
       };
       return v;  
   }  

   // �� ID�� ���� �� ���� ��������
   std::shared_ptr<GameRoom> GetRoom(uint64_t roomId) {
       std::lock_guard<std::mutex> lk(_mtx);
       auto it = _rooms.find(roomId);
       return it != _rooms.end() ? it->second : nullptr;
   }

   std::shared_ptr<GameRoom> FindRoomByPlayer(uint64_t playerId)
   {
       std::lock_guard<std::mutex> lock(_mtx);
       for (const auto& kv : _rooms)  // kv.first = roomId, kv.second = room
       {
           const auto& room = kv.second;
           for (const auto& p : room->playersInRoom)
           {
               if (p->id == playerId)
                   return room;
           }
       }
       return nullptr;
   }

   void RemovePlayerFromRoom(uint64_t playerId)
   {
       std::lock_guard<std::mutex> lock(_mtx);

       for (auto it = _rooms.begin(); it != _rooms.end(); )
       {
           auto& room = it->second;
           auto& players = room->playersInRoom;

           // �÷��̾� Ž��
           auto found = std::find_if(players.begin(), players.end(),
               [playerId](const PlayerPtr& p) { return p && p->id == playerId; });

           if (found != players.end())
           {
               bool isHost = room->creator && room->creator->id == playerId;

               // ����
               players.erase(found);
               room->readyStates.erase(playerId);

               if (isHost)
               {
                   // �� ���� �� �˸�
                   printf("[Room] Host %llu left. Removing room %llu\n", playerId, room->id);

                   Protocol::JoinRoomResponse resp;
                   resp.set_success(false);
                   resp.set_error("Host has left. Room removed.");

                   auto buf = ServerPacketHandler::MakeSendBuffer(resp);

                   for (std::vector<PlayerPtr>::iterator it = room->playersInRoom.begin(); it != room->playersInRoom.end(); ++it)
                   {
                       PlayerPtr p = *it;
                       if (p)
                       {
                           std::shared_ptr<ClientSession> s = p->session.lock();
                           if (s)
                           {
                               s->Send(buf);
                           }
                       }
                   }

                   it = _rooms.erase(it); // �� ����
                   continue;
               }
           }
           ++it;
       }
   }

private:  
   RoomManager() = default;
   ~RoomManager() = default;
   RoomManager(const RoomManager&) = delete;
   RoomManager& operator=(const RoomManager&) = delete;

   std::mutex _mtx;  
   uint64_t _nextRoomId = 1;
   std::unordered_map<uint64_t, std::shared_ptr<GameRoom>> _rooms; // Fixed type to std::shared_ptr<GameRoom>  
};