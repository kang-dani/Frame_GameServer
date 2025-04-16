#pragma once
class RecvBuffer
{
	enum {BUFFER_COUNT = 10 };
private:
	//bufferSize * BUFFER_COUNT	 == capacity
	uint32 capacity = 0;
	uint32 bufferSize = 0;
	uint32 readPos = 0;
	uint32 writePos = 0;
	vector<BYTE> buffer;
public:
	RecvBuffer(uint32 size);
	~RecvBuffer();
public:
	BYTE* ReadPos() { return &buffer[readPos]; }
	BYTE* WritePos() { return &buffer[writePos]; }
	uint32 DataSize() const { return writePos - readPos; }
	//FreeSize == 남은 공간	총 용량에서 - writePos
	uint32 FreeSize() const { return capacity - writePos; }
public:
	void Clear();
	bool OnRead(uint32 readBytes);
	bool OnWrite(uint32 writeBytes);

};

