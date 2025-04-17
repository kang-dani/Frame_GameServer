using ClientCSharp.Network;
using Google.Protobuf;
using Protocol;
using System;

namespace ClientCSharp.Packet
{
    // 클라이언트 → 서버 패킷 ID (서버와 동일하게 유지)
    enum PacketID : ushort
    {
        // 클라이언트 → 서버 패킷
        LoginRequest = 1001,
        EnterGameRequest = 1101,
        PlayerMoveRequest = 1201,
        ChatRequest = 1301,
        ActionRequest = 1401,

        // 서버 → 클라이언트 응답 패킷
        LoginResponse = 1002,
        EnterGameResponse = 1102,
        PlayerMoveResponse = 1202,
        ChatResponse = 1302,
        ActionResponse = 1402,
        WorldStateUpdate = 1501
    }

    internal class ClientPacketHandler : PacketHandler
    {
        private static readonly ClientPacketHandler instance = new();
        public static ClientPacketHandler Instance { get { return instance; } }

        public override void Init()
        {
            onRecv.Add((ushort)PacketID.LoginResponse, MakePacket<LoginResponse>);
            handler.Add((ushort)PacketID.LoginResponse, Handle_LoginResponse);
            onRecv.Add((ushort)PacketID.EnterGameResponse, MakePacket<EnterGameResponse>);
            handler.Add((ushort)PacketID.EnterGameResponse, Handle_EnterGameResponse);
            onRecv.Add((ushort)PacketID.PlayerMoveResponse, MakePacket<PlayerMoveResponse>);
            handler.Add((ushort)PacketID.PlayerMoveResponse, Handle_PlayerMoveResponse);
            onRecv.Add((ushort)PacketID.ChatResponse, MakePacket<ChatResponse>);
            handler.Add((ushort)PacketID.ChatResponse, Handle_ChatResponse);
            onRecv.Add((ushort)PacketID.ActionResponse, MakePacket<ActionResponse>);
            handler.Add((ushort)PacketID.ActionResponse, Handle_ActionResponse);

        }

        public static void Handle_LoginResponse(PacketSession session, IMessage packet)
        {
            Console.WriteLine("Handle_LoginResponse");
        }

        public static void Handle_EnterGameResponse(PacketSession session, IMessage packet)
        {
            Console.WriteLine("Handle_EnterGameResponse");
        }

        public static void Handle_PlayerMoveResponse(PacketSession session, IMessage packet)
        {
            Console.WriteLine("Handle_PlayerMoveResponse");
        }

        public static void Handle_ChatResponse(PacketSession session, IMessage packet)
        {
            Console.WriteLine("Handle_ChatResponse");

        }

        public static void Handle_ActionResponse(PacketSession session, IMessage packet)
        {
            Console.WriteLine("Handle_ActionResponse");

        }

        public static void Handle_WorldStateUpdate(PacketSession session, IMessage packet)
        {
            Console.WriteLine("Handle_WorldStateUpdate");

        }

        // IMessage 기반으로 버퍼 생성 (enum 사용)
        public static ArraySegment<byte> MakeSendBuffer<T>(T packet) where T : IMessage
        {
            string msgName = packet.Descriptor.Name.Replace("_", string.Empty);

            // enum에서 PacketID 변환
            PacketID packetID = (PacketID)Enum.Parse(typeof(PacketID), msgName, true);

            // 부모의 ID 기반 버퍼 생성 메서드 호출
            return MakeSendBuffer(packet, (ushort)packetID);
        }
    }
}
