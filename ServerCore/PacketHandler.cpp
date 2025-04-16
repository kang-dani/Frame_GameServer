#include "pch.h"
#include "PacketHandler.h"

PacketHandler::PacketFunc PacketHandler::packetHandlers[UINT16_MAX];


void PacketHandler::Init()
{
	for (int i = 0; i < UINT16_MAX; i++)
	{
		packetHandlers[i] = Handle_INVALID;

	}
}

bool PacketHandler::HandlePacket(shared_ptr<PacketSession>& session, BYTE* buffer, int len)
{
	PacketHeader* header = (PacketHeader*)buffer;
	return packetHandlers[header->id](session, buffer, len);
}
