using ClientCSharp.Network;
using Google.Protobuf;
using Protocol;
using System.Net;

namespace ClientCSharp.Packet
{
    internal class ServerSession : PacketSession
    {
        public void Send(IMessage packet)
        {
            var sendBuffer = ClientPacketHandler.MakeSendBuffer(packet);
            Send(sendBuffer);
        }

        public override void OnConnected(EndPoint endPoint)
        {
            Console.WriteLine("OnConnected");

            LoginRequest packet = new LoginRequest
            {
                UserId = "player1",
                Token = "debug-token-12345"
            };

            Send(packet);
        }

        public override void OnDisconnected(EndPoint endPoint)
        {
            Console.WriteLine("OnDisconnected");
        }

        public override void OnRecvPacket(ArraySegment<byte> buffer)
        {
            Console.WriteLine("OnRecvPacket");
            ClientPacketHandler.Instance.HandlePacket(this, buffer);
        }

        public override void OnSend(int numOfBytes)
        {
            Console.WriteLine("OnSend");
        }

    }
}
