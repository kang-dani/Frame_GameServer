#pragma once
#include "pch.h"
#include <ClientService.h>

#include "ServerSession.h"

#include "ServerPacketHandler.h"

#include <locale>
#include <codecvt>
#include <string>

int main()
{
    ServerPacketHandler::Init();


    std::this_thread::sleep_for(std::chrono::seconds(1));

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

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;


    while (true)
    {
        wchar_t msg[100];
        wprintf(L"Enter message: ");
        wscanf_s(L"%s", msg, sizeof(msg) / sizeof(wchar_t));  // UTF-16로 입력받음

        // UTF-16(wchar_t*) -> UTF-8(std::string) 변환
        std::string utf8Msg = converter.to_bytes(msg);

        // 패킷에 UTF-8 문자열 설정
        Protocol::C_CHAT chatPacket;
        chatPacket.set_msg(utf8Msg);  // UTF-8로 설정
        auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPacket);

        clientService->GetSession()->Send(sendBuffer);
    }

    t.join();

    return 0;
}
