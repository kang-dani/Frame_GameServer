
namespace ClientCSharp.Network
{
    public abstract class PacketSession : Session
    {
        // 패킷 헤더 크기 (패킷의 크기를 나타내는 부분)
        public static readonly int HeaderSize = 2;

        // [size(2)][packetId(2)][ ... ][size(2)][packetId(2)][ ... ]
        // 수신된 데이터를 처리하는 메서드
        public sealed override int OnRecv(ArraySegment<byte> buffer)
        {
            int processLen = 0; // 현재까지 처리한 바이트 수

            while (buffer.Array != null)
            {
                // 최소한 헤더만큼의 데이터를 가지고 있는지 확인
                if (buffer.Count < HeaderSize)
                    break;

                // 패킷 크기 확인 (buffer.Array에서 패킷의 크기 추출)
                ushort dataSize = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
                if (buffer.Count < dataSize)
                    break; // 패킷 전체가 아직 도착하지 않은 경우 중지

                // 패킷 전체가 도착한 경우 패킷 데이터를 전달
                OnRecvPacket(new ArraySegment<byte>(buffer.Array, buffer.Offset, dataSize));

                // 처리한 패킷의 길이만큼 이동
                processLen += dataSize;
                buffer = new ArraySegment<byte>(buffer.Array, buffer.Offset + dataSize, buffer.Count - dataSize);
            }

            // 처리한 바이트 수 반환
            return processLen;
        }

        // 실제 패킷 데이터를 처리하는 추상 메서드, 하위 클래스에서 구현
        public abstract void OnRecvPacket(ArraySegment<byte> buffer);
    }

}
