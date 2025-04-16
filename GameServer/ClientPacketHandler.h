#pragma once
#include <PacketHandler.h>
#include "Protocol.pb.h"

enum : uint16
{
    C_LOGIN = 0,
    S_LOGIN = 1,
    C_ENTER_GAME = 2,
    S_ENTER_GAME = 3,
    C_CHAT = 4,
    S_CHAT = 5,
    S_JOIN_GAME = 6,
};

//Recv 함수 정의
bool Handle_C_LOGIN(shared_ptr<PacketSession>& session, Protocol::C_Login& packet);
bool Handle_C_ENTER_GAME(shared_ptr<PacketSession>& session, Protocol::C_EnterGame& packet);
bool Handle_C_CHAT(shared_ptr<PacketSession>& session, Protocol::C_Chat& packet);

class ClientPacketHandler : public PacketHandler
{

public:
    //Recv 함수 등록
    static void Init();
private:


    //Send 함수 정의
public:
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_Login& packet) { return PacketHandler::MakeSendBuffer(packet, S_LOGIN); }
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_JoinGame& packet) { return PacketHandler::MakeSendBuffer(packet, S_JOIN_GAME); }
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_EnterGame& packet) { return PacketHandler::MakeSendBuffer(packet, S_ENTER_GAME); };
    static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_Chat& packet) { return PacketHandler::MakeSendBuffer(packet, S_CHAT); };
};