#pragma once
#include "ClientSession.h"

class Player : public enanble_shared_from_this<Player>
{
public:
	uint32 id;
	string name;

public:
	weak_ptr<ClientSession> session;
	
public:
	Player(){}
	virtual ~Player(){}
};