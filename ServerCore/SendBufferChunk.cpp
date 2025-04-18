#include "pch.h"
#include "SendBufferChunk.h"
#include "SendBuffer.h"

void SendBufferChunk::Init()
{
    //로콜 스토리지 라서 싱글 스레드처럼 사용하면 됨
    //값을 초기화
    open = false;
    usedsize = 0;
}

shared_ptr<SendBuffer> SendBufferChunk::Open(int size)
{
    //사용할려는 데이터가 BUFFER_SIZE보다 크면
    if (size > BUFFER_SIZE)
        return nullptr;

    //사용하고 있는 상태라면
    if (open)
        return nullptr;

    //사용할려는 데이터가 남은 공간보다 크면
    if (size > FreeSize())
        return nullptr;

    //사용하겠다 표시
    open = true;

    //공간 확보
    //size == 4096
    //               [  4096                                    ]
    //[x][x][x][x][x][usedSize][][][][][][][][][][][][][][][][][]
    return make_shared<SendBuffer>(shared_from_this(), &buffer[usedsize], size);
}

void SendBufferChunk::Close(int size)
{
    //사용하는 상태가 아니라면
    if (!open)
        return;

    //실제 사용한 데이터 만큼 메모리 자리 옮겨주기
    usedsize += size;   //size == 10
   //               [  4096                                    ]
   //[x][x][x][x][x][usedSize][][][][][][][][][][][][][][][][][]

   //               [  4096                                              ]
   //[x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][usedSize][][][][][][][]

    //사용 끝났다고 닫아주고
    open = false;
}
