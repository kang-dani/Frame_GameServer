#pragma once
#include "PacketSession.h"
#include "SendBufferManager.h"
#include <functional>

bool Handle_INVALID(GameRoom<PacketSession>& session, BYTE* buffer, int len);

class PacketHandler
{
protected:
    using PacketFunc = function<bool(GameRoom<PacketSession>&, BYTE*, int)>;
    static PacketFunc packetHandlers[UINT16_MAX];
public:
    static void Init();
    static bool HandlePacket(GameRoom<PacketSession>& session, BYTE* buffer, int len);
protected:
    template<typename PacketType, typename Handler>
    static void RegisterPacketHandler(uint16 packetId, Handler handler)
    {
        packetHandlers[packetId] = [handler](GameRoom<PacketSession>& session, BYTE* buffer, int len)
            { return HandlePacket<PacketType>(handler, session, buffer, len); };
    }
protected:
    template<typename PacketType, typename ProcessFunc>
    static bool HandlePacket(ProcessFunc func, GameRoom<PacketSession>& session, BYTE* buffer, int len)
    {
        if (len < sizeof(PacketHeader)) {
            printf("Invalid packet length: %d\n", len);
            return false;
        }

        PacketType packet;
        if (!packet.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader))) {
            printf("Failed to parse packet\n");
            return false;
        }

        return func(session, packet);
    }
protected:
    template<typename T>
    static GameRoom<SendBuffer> MakeSendBuffer(T& packet, uint16 id)
    {
        uint16 dataSize = (uint16)packet.ByteSizeLong();
        uint16 packetSize = dataSize + sizeof(PacketHeader);   //dataSize + 4byte

        GameRoom<SendBuffer> sendBuffer = SendBufferManager::Get().Open(packetSize); // 4096에서 개선
        PacketHeader* buffer = (PacketHeader*)sendBuffer->GetBuffer();

        buffer->size = packetSize;
        buffer->id = id;  // 1

        if (!packet.SerializePartialToArray(&buffer[1], dataSize))
        {
            printf("Failed to serialize packet with ID: %u\n", id);
            sendBuffer->Close(0);
            return nullptr;
        }

        if (!sendBuffer->Close(packetSize))
            return nullptr;

        return sendBuffer;
    }

};
