#pragma once
#include <PacketHandler.h>
#include "Protocol.pb.h"

enum : uint16
{
    C_LOGIN = 1001,
    S_LOGIN = 1002,
    C_ENTER_GAME = 1003,
    S_ENTER_GAME = 1004,
    C_CHAT = 1005,
    S_CHAT = 1006,
    S_JOIN_GAME = 1007,
};

bool Handle_S_LOGIN(shared_ptr<PacketSession>& session, Protocol::S_LOGIN& packet);
bool Handle_S_ENTER_GAME(shared_ptr<PacketSession>& session, Protocol::S_ENTER_GAME& packet);
bool Handle_S_CHAT(shared_ptr<PacketSession>& session, Protocol::S_CHAT& packet);
bool Handle_S_JOIN_GAME(shared_ptr<PacketSession>& session, Protocol::S_JOIN_GAME& packet);


class ServerPacketHandler : public PacketHandler
{
public:
    static void Init();

    //Send
public:
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_LOGIN& packet) { return PacketHandler::MakeSendBuffer(packet, C_LOGIN); }
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_ENTER_GAME& packet) { return PacketHandler::MakeSendBuffer(packet, C_ENTER_GAME); }
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_CHAT& packet) { return PacketHandler::MakeSendBuffer(packet, C_CHAT); }

};

