#include "pch.h"
#include "SendBufferManager.h"

thread_local shared_ptr<SendBufferChunk> SendBufferManager::localSendBufferChunk = nullptr;

shared_ptr<SendBuffer> SendBufferManager::Open(int size)
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

    //Chunk가 얼마 남아있는지 체크
    //printf("Chunck Free Data : %u\n", localSendBufferChunk->FreeSize());

    return localSendBufferChunk->Open(size);

}

shared_ptr<SendBufferChunk> SendBufferManager::Pop()
{
    {
        unique_lock<shared_mutex> lock(rwLock);

        if (!sendBufferChunks.empty())
        {
            shared_ptr<SendBufferChunk> sendBufferChunk = sendBufferChunks.back();
            sendBufferChunks.pop_back();

            return sendBufferChunk;

        }
    }

    return  shared_ptr<SendBufferChunk>(new SendBufferChunk, PushGlobal);
}


void SendBufferManager::PushGlobal(SendBufferChunk* bufferChunck)
{
    //체크하기 위해
    printf("Pushed Chunk : %u\n", Get().sendBufferChunks.size());
    Get().Push(shared_ptr<SendBufferChunk>(bufferChunck, PushGlobal));
  
}

void SendBufferManager::Push(shared_ptr<SendBufferChunk> bufferChunk)
{
    unique_lock<shared_mutex> lock(rwLock);
    sendBufferChunks.push_back(bufferChunk);
}

