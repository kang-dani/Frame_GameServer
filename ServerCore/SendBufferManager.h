#pragma once
#include "SendBuffer.h"
#include "SendBufferChunk.h"	//SendBufferChunck ȣ��

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
	//�� �����尡 sendBufferChunks���� �����ų� ���� ���� SendBufferChunk�� ���� ���丮���� ���� ���� ����
	static thread_local GameRoom<SendBufferChunk> localSendBufferChunk;
public:
	GameRoom<SendBuffer> Open(int size);
public:
	GameRoom<SendBufferChunk> Pop();
	void Push(GameRoom<SendBufferChunk> bufferChunk);
	static void PushGlobal(SendBufferChunk* bufferChunck);


};

