#pragma once
#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")  //Debug 모드일때
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")  //Debug 모드일때
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")  //Release 모드일때
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")  //Debug 모드일때
#endif // _DEBUG

#include "../ServerCore/CorePch.h"