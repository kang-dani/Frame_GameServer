namespace ClientCSharp.Network
{
    internal class RecvBuffer
    {
        // Read-only ArraySegment<byte> buffer 초기화 (버퍼 배열)
        readonly ArraySegment<byte> buffer;
        int readPos; // 읽기 위치
        int writePos; // 쓰기 위치

        // 생성자: 지정된 크기로 버퍼 초기화
        public RecvBuffer(int size)
        {
            buffer = new ArraySegment<byte>(new byte[size], 0, size);
        }

        // 현재 읽기 가능한 데이터 범위를 반환
        public ArraySegment<byte> ReadPos
        {
            get
            {
                // buffer.Array가 null이 아닌 경우에만 읽기 가능한 데이터 반환, null일 경우 default 반환
                return buffer.Array != null ? new ArraySegment<byte>(buffer.Array, buffer.Offset + readPos, DataSize) : default;
            }
        }

        // 현재 쓰기 가능한 데이터 범위를 반환
        public ArraySegment<byte> WritePos
        {
            get
            {
                // buffer.Array가 null이 아닌 경우에만 쓰기 가능한 데이터 반환, null일 경우 default 반환
                return buffer.Array != null ? new ArraySegment<byte>(buffer.Array, buffer.Offset + writePos, FreeSize) : default;
            }
        }

        // 현재 버퍼 내 데이터 크기 (쓰기 위치 - 읽기 위치)
        public int DataSize { get { return writePos - readPos; } }

        // 버퍼 내 여유 공간 크기 (버퍼 전체 크기 - 쓰기 위치)
        public int FreeSize { get { return buffer.Count - writePos; } }

        // 버퍼 정리: 읽은 데이터 제거하고 커서 위치 재설정
        public void Clear()
        {
            int dataSize = DataSize;
            if (dataSize == 0)
            {
                // 데이터가 남아있지 않으면 읽기, 쓰기 위치를 모두 0으로 초기화
                readPos = 0;
                writePos = 0;
            }
            else
            {
                // buffer.Array가 null이 아닌 경우에만 남은 데이터 복사
                if (buffer.Array != null)
                {
                    // 남아있는 데이터가 있으면 시작 위치로 복사하여 데이터 연속성 유지
                    Array.Copy(buffer.Array, buffer.Offset + readPos, buffer.Array, buffer.Offset, dataSize);
                }

                // 읽기 위치를 0으로, 쓰기 위치를 남은 데이터 크기로 설정
                readPos = 0;
                writePos = dataSize;
            }
        }

        // 읽기 작업 수행 후 읽기 위치를 이동시키는 메서드
        public bool OnRead(int readBytes)
        {
            // 요청한 읽기 바이트가 현재 데이터 크기를 초과하면 false 반환
            if (readBytes > DataSize)
                return false;

            // 읽기 위치를 요청한 바이트만큼 이동
            readPos += readBytes;
            return true;
        }

        // 쓰기 작업 수행 후 쓰기 위치를 이동시키는 메서드
        public bool OnWrite(int writeBytes)
        {
            // 요청한 쓰기 바이트가 여유 공간을 초과하면 false 반환
            if (writeBytes > FreeSize)
                return false;

            // 쓰기 위치를 요청한 바이트만큼 이동
            writePos += writeBytes;
            return true;
        }
    }

}
