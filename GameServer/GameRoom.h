// GameRoom.h
#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <random>
#include "Player.h"
#include "WorldState.h"
#include "ClientSession.h"
#include "ServerPacketHandler.h"

using PlayerPtr = std::shared_ptr<Player>;

class GameRoom 
{
public:
    uint64_t id;
    PlayerPtr creator;
    std::vector<PlayerPtr> playersInRoom;
    static constexpr uint32_t MAX_COUNT = 5;
    bool started = false;

    std::unordered_map<uint64_t, bool> readyStates;
    mutable std::mutex readyMutex;
    std::unordered_map<uint64_t, std::shared_ptr<ClientSession>> playerSessions;

    WorldState& GetWorldState() { return worldState; }

    GameRoom(uint64_t roomID, PlayerPtr creator_)
        : id(roomID), creator(std::move(creator_)) {
        playersInRoom.push_back(creator);
        readyStates[creator->id] = false;
    }

    void Tick() {
        std::lock_guard<std::mutex> lock(stateMutex);
        Simulate();
        BroadcastDelta();
    }
    
	void Simulate() {
        static int frameCounter = 0;
        {
            std::lock_guard<std::mutex> lock(stateMutex);
            worldState.Simulate(); 
        }

        frameCounter++;
        if (frameCounter >= 20) {
            BroadcastDelta();  
        }
	}

    void SendInitWorldState(std::shared_ptr<ClientSession> session) {
        Protocol::InitWorldStateRequest msg;

        for (const auto& kv : worldState.players) {
            uint64_t pid = kv.first;
            const PlayerState& ps = kv.second;

            auto* dst = msg.add_players();
            dst->set_playerid(pid);
            dst->mutable_position()->CopyFrom(ps.transform);
            dst->set_statetype(ps.stateType);
            dst->set_hasupgradedflashlight(ps.flashlightOn);
            dst->set_isloading(ps.isLoading);
            dst->set_minigamesuccess(ps.miniGameCleared);
            dst->set_escapecode(ps.escapeCode);
            dst->set_alphaplayer(ps.alphaPlayer);
            dst->set_minigamesparkle(ps.miniGameSparkle);
            dst->set_characterchoice(ps.characterChoice);
            dst->set_isjailed(ps.isJailed);
            dst->set_successgenerator(ps.successGenerator);
            dst->set_isboss(ps.isBoss);
            dst->set_bosstype(ps.bossType);
        }

        for (const auto& kv : worldState.generators) {
            uint64_t gid = kv.first;
            const GeneratorState& gs = kv.second;

            auto* dst = msg.add_generators();
            dst->set_generatorid(gid);
            dst->mutable_transform()->CopyFrom(gs.transform);
            dst->set_isactivated(gs.isActivated);
            dst->set_issparkling(gs.isSparkling);
        }

        for (const auto& kv : worldState.miniGames) {
            uint64_t mid = kv.first;
            const MiniGameState& ms = kv.second;

            auto* dst = msg.add_minigames();
            dst->set_minigameid(mid);
            dst->mutable_transform()->CopyFrom(ms.transform);
            dst->set_iscleared(ms.isCleared);
            dst->set_issparkling(ms.isSparkling);
        }

        auto buf = ServerPacketHandler::MakeSendBuffer(msg);
        session->Send(buf);

        uint64_t cid = session->GetPlayer()->id;
        lastSentState[cid] = worldState;
    }

    void BroadcastDelta() {
        for (auto it = playerSessions.begin(); it != playerSessions.end(); ++it) {
            uint64_t pid = it->first;
            const auto& session = it->second;
            if (!session) continue;

            uint64_t cid = pid;
            const WorldState& prev = lastSentState[cid];
            Protocol::WorldStateDelta delta;

            for (const auto& kv : worldState.players) {
                uint64_t pid = kv.first;
                const PlayerState& ps = kv.second;
                const PlayerState& old = prev.players.at(pid);

                bool hasUpdate = false;
                auto* up = delta.add_playerupdates();
                up->set_playerid(pid);

                if (ps.transform.SerializeAsString() != old.transform.SerializeAsString()) {
                    up->mutable_transform()->CopyFrom(ps.transform);
                    hasUpdate = true;
                }
                if (ps.stateType != old.stateType) {
                    up->set_statetype(ps.stateType);
                    hasUpdate = true;
                }
                if (ps.isJailed != old.isJailed) {
                    up->set_isjailed(ps.isJailed);
                    hasUpdate = true;
                }
                if (ps.flashlightOn != old.flashlightOn) {
                    up->set_flashlighton(ps.flashlightOn);
                    hasUpdate = true;
                }
                if (ps.isLoading != old.isLoading) {
                    up->set_isloading(ps.isLoading);
                    hasUpdate = true;
                }
                if (ps.miniGameCleared != old.miniGameCleared) {
                    up->set_minigamesuccess(ps.miniGameCleared);
                    hasUpdate = true;
                }
                if (ps.escapeCode != old.escapeCode) {
                    up->set_escapecode(ps.escapeCode);
                    hasUpdate = true;
                }
                if (std::abs(ps.alphaPlayer - old.alphaPlayer) > 1e-6f) {
                    up->set_alphaplayer(ps.alphaPlayer);
                    hasUpdate = true;
                }
                if (ps.miniGameSparkle != old.miniGameSparkle) {
                    up->set_minigamesparkle(ps.miniGameSparkle);
                    hasUpdate = true;
                }

                if (!hasUpdate) delta.mutable_playerupdates()->RemoveLast();
            }

            for (const auto& kv : worldState.generators) {
                uint64_t gid = kv.first;
                const GeneratorState& gs = kv.second;
                const GeneratorState& old = prev.generators.at(gid);

                auto* ug = delta.add_generatorupdates();
                ug->set_generatorid(gid);

                bool genUpdate = false;
                if (gs.isActivated != old.isActivated) {
                    ug->set_isactivated(gs.isActivated);
                    genUpdate = true;
                }
                if (gs.isSparkling != old.isSparkling) {
                    ug->set_issparkling(gs.isSparkling);
                    genUpdate = true;
                }

                if (!genUpdate) delta.mutable_generatorupdates()->RemoveLast();
            }

            for (const auto& kv : worldState.miniGames) {
                uint64_t mid = kv.first;
                const MiniGameState& ms = kv.second;
                const MiniGameState& old = prev.miniGames.at(mid);

                auto* um = delta.add_miniupdates();
                um->set_minigameid(mid);

                bool miniUpdate = false;
                if (ms.isCleared != old.isCleared) {
                    um->set_iscleared(ms.isCleared);
                    miniUpdate = true;
                }
                if (ms.isSparkling != old.isSparkling) {
                    um->set_issparkling(ms.isSparkling);
                    miniUpdate = true;
                }

                if (!miniUpdate) delta.mutable_miniupdates()->RemoveLast();
            }

            if (delta.playerupdates_size() > 0) {
                auto buf = ServerPacketHandler::MakeSendBuffer(delta);
                session->Send(buf);
                lastSentState[cid] = worldState;
            }
        }
    }

    bool IsFull() const {
        return playersInRoom.size() >= MAX_COUNT;
    }

    void AddPlayer(const PlayerPtr& p) {
        std::lock_guard<std::mutex> lock(readyMutex);
        playersInRoom.push_back(p);
        readyStates[p->id] = false;
    }

    void SetReady(uint64_t playerId, bool isReady) {
        std::lock_guard<std::mutex> lock(readyMutex);
        readyStates[playerId] = isReady;
    }

    bool IsReady(uint64_t playerId) const {
        std::lock_guard<std::mutex> lock(readyMutex);
        auto it = readyStates.find(playerId);
        return it != readyStates.end() && it->second;
    }

    bool AllReady() const {
        std::lock_guard<std::mutex> lock(readyMutex);
        if (!IsFull()) return false;

        for (auto& p : playersInRoom) {
            auto it = readyStates.find(p->id);
            if (it == readyStates.end() || !it->second) return false;
        }

        return true;
    }

private:
    WorldState worldState;
    std::unordered_map<uint64_t, WorldState> lastSentState;
    std::mutex stateMutex;
};
