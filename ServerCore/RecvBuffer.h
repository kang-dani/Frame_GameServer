#pragma once
#include <cstdint>          // uint32_t, uint16_t, uint8_t
#include <WinSock2.h>       // BYTE, DWORD, SOCKET 등
#include <Windows.h>        // HANDLE 등
#include <memory>           // shared_ptr
#include <set>              // std::set
#include <vector>           // std::vector
#include <unordered_map>    // std::unordered_map
#include <mutex>            // std::mutex
#include <shared_mutex>     // shared_mutex

class RecvBuffer
{
	enum {BUFFER_COUNT = 10 };
private:
	//bufferSize * BUFFER_COUNT	 == capacity
	int capacity = 0;
	int bufferSize = 0;
	int readPos = 0;
	int writePos = 0;
	std::vector<BYTE> buffer;
public:
	RecvBuffer(int size);
	~RecvBuffer();
public:
	BYTE* ReadPos() { return &buffer[readPos]; }
	BYTE* WritePos() { return &buffer[writePos]; }
	int DataSize() const { return writePos - readPos; }
	//FreeSize == 남은 공간	총 용량에서 - writePos
	int FreeSize() const { return capacity - writePos; }
public:
	void Clear();
	bool OnRead(int readBytes);
	bool OnWrite(int writeBytes);

};

