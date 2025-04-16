#pragma once
#include "pch.h"
#include <ServerService.h>
#include <SendBufferManager.h>

#include "ClientSession.h"
#include "SessionManager.h"

#include "Protocol.pb.h" // include

#include "ClientPacketHandler.h"

#define THREAD_COUNT 5

int main()
{
    ClientPacketHandler::Init();

    printf("============== Server  ================\n");

    shared_ptr<Service> serverService = make_shared<ServerService>(L"127.0.0.1", 27015, []() {return make_shared<ClientSession>(); });


    //serverService 시작하기
    if (!serverService->Start())
    {
        printf("Server Service Error\n");
        return 1;

    }

    vector<thread> threads;

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threads.push_back(thread
        (
            [=]()
            {
                while (true)
                {
                    serverService->ObserveIO();
                }
            }
        ));
    }
   

  


    for (int i = 0; i < THREAD_COUNT; i++)
    {
        if (threads[i].joinable())
        {
            threads[i].join();
        }
    }
 

    return 0;
}
