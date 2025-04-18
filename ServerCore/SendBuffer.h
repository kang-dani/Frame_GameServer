#pragma once

//���漱��
class SendBufferChunk;

class SendBuffer : public enable_shared_from_this<SendBuffer>
{
private:
	//vector<BYTE> buffer;
	BYTE* buffer; // ���� ���� ��� ���� �ּҸ�
	//���� ����ϰ� �ִ� �Ŵ��� �޸� �ּ�
	GameRoom<SendBufferChunk> sendBufferChunk;
	//���� ���� 
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

