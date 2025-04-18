#include "pch.h"
#include "SendBufferManager.h"

thread_local GameRoom<SendBufferChunk> SendBufferManager::localSendBufferChunk = nullptr;

GameRoom<SendBuffer> SendBufferManager::Open(int size)
{
    if (localSendBufferChunk == nullptr)
    {
        localSendBufferChunk = Pop();
        localSendBufferChunk->Init(); 

    }

    if (localSendBufferChunk->IsOpen())
        return nullptr;

    if (localSendBufferChunk->FreeSize() < size)
    {
        localSendBufferChunk = Pop();
        localSendBufferChunk->Init(); 

    }

    //Chunk�� �� �����ִ��� üũ
    //printf("Chunck Free Data : %u\n", localSendBufferChunk->FreeSize());

    return localSendBufferChunk->Open(size);

}

GameRoom<SendBufferChunk> SendBufferManager::Pop()
{
    {
        unique_lock<shared_mutex> lock(rwLock);

        if (!sendBufferChunks.empty())
        {
            GameRoom<SendBufferChunk> sendBufferChunk = sendBufferChunks.back();
            sendBufferChunks.pop_back();

            return sendBufferChunk;

        }
    }

    return  GameRoom<SendBufferChunk>(new SendBufferChunk, PushGlobal);
}


void SendBufferManager::PushGlobal(SendBufferChunk* bufferChunck)
{
    //üũ�ϱ� ����
    printf("Pushed Chunk : %u\n", Get().sendBufferChunks.size());
    Get().Push(GameRoom<SendBufferChunk>(bufferChunck, PushGlobal));
  
}

void SendBufferManager::Push(GameRoom<SendBufferChunk> bufferChunk)
{
    unique_lock<shared_mutex> lock(rwLock);
    sendBufferChunks.push_back(bufferChunk);
}

