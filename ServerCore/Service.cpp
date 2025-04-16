#include "pch.h"
#include "Service.h"
#include "SocketHelper.h"
#include "IocpCore.h"
#include "Session.h"

Service::Service(ServiceType type, wstring ip, uint16 port, SessionFactory factory) : serviceType(type), sessionFactory(factory)
{
    if (!SocketHelper::StartUp())
        return;

    memset(&sockAddr, 0, sizeof(sockaddr));
    sockAddr.sin_family = AF_INET;

    IN_ADDR address{};
    InetPton(AF_INET, ip.c_str(), &address);
    sockAddr.sin_addr = address;
    sockAddr.sin_port = htons(port);

    //make_shared<자료형> 스마트 포인터에 할당
    //iocpCore = new IocpCore
    iocpCore = make_shared<IocpCore>();
}

Service::~Service()
{
   //if (iocpCore != nullptr)
   //{
   //    delete iocpCore;
   //    iocpCore = nullptr;
   //}


    SocketHelper::CleanUp();
}

//스마트 포인터로 변환
shared_ptr<Session> Service::CreateSession()
{
    //스마트 포인터로 변환
    shared_ptr<Session> session = sessionFactory();

    //this -> shared_from_this
    session->SetService(shared_from_this());

    if (!iocpCore->Register(session))
        return nullptr;

    return session;
   
}

//스마트 포인터로 변환
void Service::AddSession(shared_ptr<Session> session)
{
    unique_lock<shared_mutex> lock(rwLock);
    sessionCount++;
    sessions.insert(session);
}
//스마트 포인터로 변환
void Service::RemoveSession(shared_ptr<Session> session)
{
    unique_lock<shared_mutex> lock(rwLock);
    sessions.erase(session);
    sessionCount--;
}

bool Service::ObserveIO(DWORD time)
{
    if (iocpCore != nullptr)
    {
        return iocpCore->ObserveIO(time);
    }

    return false;
}
