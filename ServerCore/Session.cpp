#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"
#include "Service.h"

Session::Session() : recvBuffer(BUFFER_SIZE) 
{
	socket = SocketHelper::CreateSocket();	
}

Session::~Session()
{
	SocketHelper::CloseSocket(socket);
}


void Session::RegisterRecv()
{
	if (!IsConnected())
		return;
		
	recvEvent.Init();
	recvEvent.owner = shared_from_this();

	WSABUF wsaBuf;

	wsaBuf.buf = (char*)recvBuffer.WritePos();
	wsaBuf.len = recvBuffer.FreeSize();

	DWORD recvLen = 0;
	DWORD flags = 0;

	if (WSARecv(socket, &wsaBuf, 1, &recvLen, &flags, &recvEvent, nullptr) == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			recvEvent.owner = nullptr;
		}
	}
	
}

void Session::ProcessRecv(int bytesTransferred)
{
	recvEvent.owner = nullptr;

	if (bytesTransferred <= 0)
	{
		Disconnect(L"Recv 0 bytes");
		return;
	}

	if (recvBuffer.OnWrite(bytesTransferred) == false)
	{
		Disconnect(L"On write overflow");
		return;
	}

	uint32 dataSize = recvBuffer.DataSize();

	uint32 processLen = OnRecv(recvBuffer.ReadPos(), bytesTransferred);
	if (processLen < 0 || dataSize < processLen || recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"On read overflow");
		return;
	}
	

	recvBuffer.Clear();

	RegisterRecv();
}

bool Session::Connect()
{
	return RegisterConnect();
}
bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;	

	if (GetService()->GetServiceType() != ServiceType::CLIENT)
		return false;	

	if (!SocketHelper::SetReuseAddress(socket, true))
		return false;

	if (SocketHelper::BindAny(socket, 0) == false)
		return false;

	connectEvent.Init();
	connectEvent.owner = shared_from_this();

	DWORD bytesTransferred = 0;
	SOCKADDR_IN sockAddr = GetService()->GetSockAddr();
	if (SocketHelper::ConnectEx(socket, (SOCKADDR*)&sockAddr, sizeof(sockAddr), nullptr, 0, &bytesTransferred, &connectEvent) == FALSE)
	{
		int errorCode = WSAGetLastError();

		if (errorCode != ERROR_IO_PENDING)
		{
			HandleError(errorCode);
			connectEvent.owner = nullptr;
			return false;
		}

	}

	return true;
}

void Session::ProcessConnect()
{
	connectEvent.owner = nullptr;

	isConnected.store(true);

	GetService()->AddSession(GetSession());

	OnConnected();
	RegisterRecv();
}											




//수정
void Session::Send(shared_ptr<SendBuffer> sendBuffer)
{
	//연결 안되어 있음 return
	if (!IsConnected())
		return;

	//모든 스레드의 기본값
	bool winnerThread = false;	  //[0]:false[1]:false[2]:false[3]:false -- 스레드들

	{  
		//Lock을 잡고
		unique_lock<shared_mutex> lock(rwLock);

		//sendQueue에 넣기만하고 끝냄
		sendQueue.push(sendBuffer);

		//1순위 인 아이가 
		if (sendRegistered.exchange(true) == false)	 //[0]:2순위[1]:1순위[2]:3순위[3]:4순위
		{

			winnerThread = true;
			//[0]:false[1]:true[2]:false[3]:false -- 스레드들
		}

	}	//Lock


	//[0]:false[1]:true[2]:false[3]:false -- 스레드들
	//내가 winnerThread라면
	if (winnerThread)
	{
		//여기서는 선택된 스레드만 활동
		RegisterSend();
	}

}


void Session::RegisterSend()
{
	if (!IsConnected())
		return;

	//sendEvent 초기화
	sendEvent.Init();
	//sendEvent의 owner는 나의 session
	sendEvent.owner = shared_from_this();

	int writeSize = 0;
	//sendQueue의 데이터가 남아 있지 않을 때까지 돌림
	while (!sendQueue.empty())
	{
		//sendQueue의 앞부분부터 pop시키기 위해서 앞부분 데이터 캐싱
		shared_ptr<SendBuffer> sendBuffer = sendQueue.front();
		//얼마나 사용했는지 크기 추가
		writeSize += sendBuffer->WriteSize();

		//앞부분 날림
		sendQueue.pop();

		//SendEvent에 sendQueue에 들어 있는 값들을 밀어 넣음
		sendEvent.sendBuffers.push_back(sendBuffer);

	}

	//한꺼번에 데이터를 보내기 위해
	vector<WSABUF>wsaBufs;
	//SendEvent의 sendBuffers의 크기 만큼 공간 예약
	wsaBufs.reserve(sendEvent.sendBuffers.size());


	//SendEvent의 sendBuffers를 순회하면서 등록
	for (auto sendBuffer : sendEvent.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = (char*)sendBuffer->GetBuffer();
		wsaBuf.len = sendBuffer->WriteSize();
		wsaBufs.push_back(wsaBuf);
		
	}

	DWORD sendLen = 0;
	DWORD flags = 0;

	//한꺼번에 여러개 보내기
	if (WSASend(socket, wsaBufs.data(), (DWORD)wsaBufs.size(), &sendLen, flags, &sendEvent, nullptr) == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			//내가 들고 있으니까
			sendEvent.owner = nullptr;
			//SendEvent의 sendBuffers가 들구 있는 값들 비우기
			sendEvent.sendBuffers.clear();
			//sendRegistered상태를 false
			//sendRegistered = false;
			sendRegistered.store(false);
		
		}
	}
}



void Session::ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred)
{
	switch (iocpEvent->eventType)
	{
	case EventType::CONNECT:
		ProcessConnect();
		break;
	case EventType::RECV:
		ProcessRecv(byteTransferred);
		break;
	case EventType::SEND:
		//SendEvent 내가 들구 있어서
		ProcessSend(byteTransferred);
		break;
	case EventType::DISCONNECT:	 
		ProcessDisconnect();
		break;
	default:
		break;
	}

}



void Session::ProcessSend(int bytesTransferred)
{
	//null로 밀고
	sendEvent.owner = nullptr;
	//할일 다 끝냈으니까 SendEvent의 sendBuffers를 비우기
	sendEvent.sendBuffers.clear();

	if (bytesTransferred == 0)
	{
		Disconnect(L"Send 0 bytes");

	}

	OnSend(bytesTransferred);

	unique_lock<shared_mutex> lock(rwLock);
	//sendQueue가 비어있다면
	if (sendQueue.empty())
	{
	   //담은 애가 일처리 할수 있게 sendRegistered를 false 상태로 
		sendRegistered.store(false);
	}
	else //아직 처리 할 일이 있으니까. 내 스레드가 send할 준비를 함.
	{
		RegisterSend();
	}
}

void Session::Disconnect(const WCHAR* cause)
{
	if (isConnected.exchange(false) == false)
		return;

	wprintf(L"Disconnect reason : %ls\n", cause);

	RegisterDisconnect();
}

bool Session::RegisterDisconnect()
{
	disconnectEvent.Init();
	disconnectEvent.owner = shared_from_this();

	if (SocketHelper::DisconnectEx(socket, &disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int errorCode = WSAGetLastError();

		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			disconnectEvent.owner = nullptr;

			return false;
		}
	}

	return true;
}


void Session::ProcessDisconnect()
{
	disconnectEvent.owner = nullptr;

	OnDisconnected();
	GetService()->RemoveSession(GetSession());
}

void Session::HandleError(int errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"Handle Error");
		break;
	default:
		printf("Error Code : %d\n", errorCode);
		break;
	}
}

