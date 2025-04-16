#pragma once
class Player;

//방하나 있다고 가정하고 전역화
class GameRoom
{
public:
	static GameRoom& Get()
	{
		static GameRoom instance;
		return instance;
	}
private:
	atomic<uint64> playerId = 0;
public:
	uint64 GetID() { return ++playerId; }
private:
	shared_mutex rwLock;
	set<shared_ptr<Player>> players;
public:
	void Enter(shared_ptr<Player> player);
	void Leave(shared_ptr<Player> player);
	void Broadcast(shared_ptr<class SendBuffer> sendBuffer);
};

