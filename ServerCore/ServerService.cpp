#include "pch.h"
#include "ServerService.h"
#include "Listener.h"

ServerService::ServerService(wstring ip, uint16 port, SessionFactory factory) : Service(ServiceType::SERVER, ip, port, factory)
{
}

ServerService::~ServerService()
{
   // if (listener != nullptr)
   // {
   //     delete listener;
   //     listener = nullptr;
   // }

}
bool ServerService::Start()
{
    //listener = new Listener;
    listener = make_shared<Listener>();
    //this -> shared_from_this() 
    return listener->StartAccept(static_pointer_cast<ServerService>(shared_from_this()));
}
