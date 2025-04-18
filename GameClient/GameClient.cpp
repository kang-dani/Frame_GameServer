#pragma once
#include "pch.h"
#include <ClientService.h>
#include "ServerSession.h"
#include "ClientPacketHandler.h"


int main()
{
    ClientPacketHandler::Init();


    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    printf("============== Client  ================\n");
    shared_ptr<ClientService> clientService = make_shared<ClientService>(L"127.0.0.1", 27015, []() {return make_shared<ServerSession>(); });

     //1000명 정도 접속 시작
     for (int i = 0; i < 1; i++)
     {
         if (!clientService->Start())
         {
             printf("Can not Start\n");
             return 1;

         }
     }

    thread t
    (
        [=]()
        {
            while (true)
            {
                clientService->ObserveIO();
            }
        }
    );


    while (true)
    {
    }

    t.join();

    return 0;
}
