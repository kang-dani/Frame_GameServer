#pragma once

//전방선언
class SendBufferChunk;

class SendBuffer : public enable_shared_from_this<SendBuffer>
{
private:
	//vector<BYTE> buffer;
	BYTE* buffer; // 실제 어디다 써야 할지 주소만
	//내가 사용하고 있는 거대한 메모리 주소
	GameRoom<SendBufferChunk> sendBufferChunk;
	//여유 공간 
	int freeSize = 0;
	int writeSize = 0;
public:
	SendBuffer(GameRoom<SendBufferChunk> chunk, BYTE* start, int size);
	~SendBuffer();
public:
	BYTE* GetBuffer() { return buffer; }
	int WriteSize() const { return writeSize; }
	//int Capacity() const { return buffer.size(); }
public:
	//bool CopyData(void* data, int len);
	bool Close(int usedSize);
};

