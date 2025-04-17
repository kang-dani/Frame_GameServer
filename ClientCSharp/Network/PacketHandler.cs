using Google.Protobuf;

namespace ClientCSharp.Network
{
    public abstract class PacketHandler
    {
        // 패킷 ID에 따라 호출할 핸들러 맵
        protected Dictionary<ushort, Action<PacketSession, IMessage>> handler = new();

        // 수신된 패킷 처리 메서드를 저장하는 맵
        protected Dictionary<ushort, Action<ushort, PacketSession, ArraySegment<byte>>> onRecv = new();

        //Unity 메인 스레드 전용
        public Action<PacketSession, ushort, IMessage> UnityThreadHandler { get; set; }

        // 생성자: 초기화 메서드 호출
        protected PacketHandler()
        {
            Init();
        }

        // 초기화 메서드: 상속받은 클래스에서 구현해야 하는 추상 메서드
        public abstract void Init();

        // 패킷을 수신할 때 호출되는 메서드
        public void HandlePacket(PacketSession session, ArraySegment<byte> buffer)
        {
            if (buffer.Array == null) { return; } // 버퍼가 null인 경우 종료

            // 패킷 ID 추출 (패킷의 두 번째 ushort)
            ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + sizeof(ushort));

            // 수신 맵에서 ID에 해당하는 액션을 찾아서 실행
            if (onRecv.TryGetValue(id, out var action))
            {
                action.Invoke(id, session, buffer);
            }
        }

        // 패킷을 생성하고 처리하는 메서드 (제네릭 타입 T는 IMessage를 상속해야 함)
        protected void MakePacket<T>(ushort id, PacketSession session, ArraySegment<byte> buffer) where T : IMessage, new()
        {
            // 새로운 패킷 생성 및 버퍼에서 데이터 병합
            T packet = new();
            packet.MergeFrom(buffer.Array, buffer.Offset + 4, buffer.Count - 4); // 헤더(4바이트)를 제외하고 데이터 병합

            //Unity 였을 경우
            if (UnityThreadHandler != null)
            {
                UnityThreadHandler.Invoke(session, id, packet);
            }
            else //아닐 경우
            {
                // 패킷 ID에 해당하는 핸들러를 찾아서 실행
                if (handler.TryGetValue(id, out var action))
                {
                    action.Invoke(session, packet);
                }
            }
        }

        public Action<PacketSession, IMessage>? GetPacketHandler(ushort id)
        {
            if (handler.TryGetValue(id, out var action))
                return action;
            return null;
        }

        // 1. 패킷을 ID로 직렬화하고 버퍼 생성 (C++와 동일)
        public static ArraySegment<byte> MakeSendBuffer<T>(T packet, ushort packetID) where T : IMessage
        {
            ushort size = (ushort)packet.CalculateSize();
            ushort packetSize = (ushort)(size + 4);  // 4바이트 헤더 포함

            byte[] sendBuffer = new byte[packetSize];

            // 패킷 크기 + ID 복사
            Array.Copy(BitConverter.GetBytes(packetSize), 0, sendBuffer, 0, sizeof(ushort));
            Array.Copy(BitConverter.GetBytes(packetID), 0, sendBuffer, 2, sizeof(ushort));
            Array.Copy(packet.ToByteArray(), 0, sendBuffer, 4, size);

            return new ArraySegment<byte>(sendBuffer, 0, packetSize);
        }
    }
}
