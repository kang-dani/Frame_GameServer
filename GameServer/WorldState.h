#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include "Protocol.pb.h"  // TransformData, Vector3, PlayerStateType, EscapeType 등

// ————————————————————————————————
// 플레이어 상태
// ————————————————————————————————
struct PlayerState {
    Protocol::TransformData   transform;         // position, scale, rotation
    Protocol::PlayerStateType stateType;        // proto: stateType
    bool                      isJailed = false;          // proto: isJailed
    int32_t                   facingDirX;        // -1=왼쪽, 1=오른쪽
    int32_t                   facingDirY;       // -1=아래, 1=위
    bool                      flashlightOn = false;  // proto: flashlightOn
    bool                      isLoading = false;  // proto: isLoading
    bool                      miniGameCleared = false; // proto: miniGameCleared
    Protocol::EscapeType      escapeCode = Protocol::ESCAPE_DEAD; // proto: escapeCode
    float                     alphaPlayer = 1.0f;    // proto: alphaPlayer
    Protocol::Vector3         miniGamePos;       // proto: miniGamePos
    bool                      miniGameSparkle = false; // proto: miniGameSparkle
    int                       characterChoice = 0;   // proto: characterChoice
    bool                      successGenerator = false;  // 발전기 성공 플래그
    bool                      isBoss = false;
    Protocol::BossType	      bossType = Protocol::BOSS_NONE; // 보스 타입
};

// ————————————————————————————————
// 발전기 상태
// ————————————————————————————————
struct GeneratorState {
    Protocol::TransformData   transform;   // position, scale, rotation
    int                    geratorId; // 발전기 ID
    bool                      isActivated = false; // proto: isActivated
    bool                      isSparkling = false; // proto: isSparkling
    bool                      isDone = false;       // proto: isDone
    uint32_t                  generatorGage;
};

// ————————————————————————————————
// 미니게임 상태
// ————————————————————————————————
struct MiniGameState {
    Protocol::TransformData   transform;   // position, scale, rotation
    bool                      isCleared = false; // proto: isCleared
    bool                      isSparkling = false; // proto: isSparkling
};

// ————————————————————————————————
// 월드 전체 상태
// ————————————————————————————————
class WorldState {
public:
    // ID → 상태 맵
    std::unordered_map<uint64_t, PlayerState>    players;
    std::unordered_map<uint64_t, GeneratorState> generators;
    std::unordered_map<uint64_t, MiniGameState>  miniGames;

    void Simulate()
    {
        // 서버 측 자동 갱신 없음
    }
};
