#pragma once
#include <string>
#include <memory>
#include "Struct.pb.h"
#include "ClientSession.h"
#include "Protocol.pb.h"

class ClientSession;

class Player : public std::enable_shared_from_this<Player>
{
public:
	int id;
	std::string nickname;

public:
	std::weak_ptr<ClientSession> session;
	
public:
	Player(){}
	virtual ~Player(){}
};