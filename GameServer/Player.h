#pragma once
#include "ClientSession.h"
#include "Struct.pb.h"

class Player : public enable_shared_from_this<Player>
{
public:
	uint64 id;
	string name;
	uint32 hp;

public:
	weak_ptr<ClientSession> session;
public:
	Player() {}
	virtual ~Player() {}
};
