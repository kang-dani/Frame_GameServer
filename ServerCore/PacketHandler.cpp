#include "pch.h"
#include "PacketHandler.h"

PacketHandler::PacketFunc PacketHandler::packetHandlers[UINT16_MAX];

void PacketHandler::Init()
{
	for (int32 i = 0; i < UINT16_MAX; i++)
		packetHandlers[i] = Handle_INVALID;
}

bool PacketHandler::HandlePacket(GameRoom<PacketSession>& session, BYTE* buffer, int len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	return packetHandlers[header->id](session, buffer, len);
}
