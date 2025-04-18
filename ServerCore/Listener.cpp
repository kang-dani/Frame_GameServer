#include "pch.h"
#include "Listener.h"
#include "ServerService.h"
#include "SocketHelper.h"
#include "IocpCore.h"
#include "Session.h"
#include "IocpEvent.h"


Listener::~Listener()
{
    CloseSocket();

    //Listener ¼Ò¸êµÉ¶§ AcceptEventµµ ¼Ò¸ê ½ÃÄÑÁÜ
    for (auto acceptEvent : accentEvents)
    {
        delete(acceptEvent);

    }
}

bool Listener::StartAccept(shared_ptr<ServerService> service)
{
    serverService = service;

    socket = SocketHelper::CreateSocket();
    if (socket == INVALID_SOCKET)
        return false;

    if (!SocketHelper::SetReuseAddress(socket, true))
        return false;


    if (!SocketHelper::SetLinger(socket, 0, 0))
        return false;
 
    ULONG_PTR key = 0;
    service->GetIocpCore()->Register(shared_from_this());

    if (!SocketHelper::Bind(socket, service->GetSockAddr()))
        return false;

    if (!SocketHelper::Listen(socket))
        return false;

    //AcceptEvent °¹¼ö
    const int acceptEventCount = 3;

    for (int i = 0; i < acceptEventCount; i++)
    {
        AcceptEvent* acceptEvent = new AcceptEvent;
        acceptEvent->owner = shared_from_this();

        //accentEvent¸¦ °ü¸® ÇÏ±âÀ§ÇØ
        accentEvents.push_back(acceptEvent);
        RegisterAccept(acceptEvent);
    }



    return true;
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    shared_ptr<Session> session = serverService->CreateSession();

    acceptEvent->Init();
    acceptEvent->session = session;

    DWORD dwBytes = 0;
                                                              //WritePos À§Ä¡°ª ³Ñ°ÜÁÜ
    if (!SocketHelper::AcceptEx(socket, session->GetSocket(), session->recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, (LPOVERLAPPED)acceptEvent))
    {
        if (WSAGetLastError() != ERROR_IO_PENDING)
            RegisterAccept(acceptEvent);
    }
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
    shared_ptr<Session> session = acceptEvent->session;

    if (!SocketHelper::SetUpdateAcceptSocket(session->GetSocket(), socket))
    {
        printf("SetUpdateAcceptSocket Error\n");
        RegisterAccept(acceptEvent);
        return;
    }


    SOCKADDR_IN sockAddr;
    int sockAddrSize = sizeof(sockAddr); 
    if (getpeername(session->GetSocket(), (SOCKADDR*)&sockAddr, &sockAddrSize) == SOCKET_ERROR)
    {
        printf("getpeername Error\n");
        RegisterAccept(acceptEvent);
        return;
    }

    session->SetSockAddr(sockAddr);

    session->ProcessConnect();
    RegisterAccept(acceptEvent);   
}

void Listener::CloseSocket()
{
    SocketHelper::CloseSocket(socket);
}



void Listener::ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred)
{
    ProcessAccept((AcceptEvent*)iocpEvent);
}
