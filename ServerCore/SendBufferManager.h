#pragma once
#include "SendBuffer.h"
#include "SendBufferChunk.h"	//SendBufferChunck 호출

class SendBufferManager
{
private:
	SendBufferManager() = default;
	~SendBufferManager() = default;
public:
	static SendBufferManager& Get()
	{
		static SendBufferManager instance;
		return instance;
	}
public:
	SendBufferManager(const SendBufferManager&) = delete;
	SendBufferManager& operator=(const SendBufferManager&) = delete;
private:
	shared_mutex rwLock;
	vector<GameRoom<SendBufferChunk>> sendBufferChunks;
public:
	//각 스레드가 sendBufferChunks에서 꺼내거나 새로 만든 SendBufferChunk를 로컬 스토리지에 연결 시켜 놓게
	static thread_local GameRoom<SendBufferChunk> localSendBufferChunk;
public:
	GameRoom<SendBuffer> Open(int size);
public:
	GameRoom<SendBufferChunk> Pop();
	void Push(GameRoom<SendBufferChunk> bufferChunk);
	static void PushGlobal(SendBufferChunk* bufferChunck);


};

