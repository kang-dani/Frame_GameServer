#include "pch.h"
#include "ClientService.h"
#include "Session.h"

ClientService::ClientService(wstring ip, uint16 port, SessionFactory factory) : Service(ServiceType::CLIENT, ip, port, factory)
{
}

bool ClientService::Start()
{
    //스마트 포인터로 변환
    session = CreateSession();
    return session->Connect();
    
}
