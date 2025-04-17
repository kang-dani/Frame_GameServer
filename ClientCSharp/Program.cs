using ClientCSharp.Network;
using ClientCSharp.Packet;
using Google.Protobuf;
using System.Net;


Thread.Sleep(1000);

ServerSession session = new();

IPAddress ipAddr = IPAddress.Parse("127.0.0.1");
IPEndPoint endPoint = new IPEndPoint(ipAddr, 27015);

ClientService service = new ClientService();
service.Connect(endPoint, () => { return session; }, 1);

while (true)
{
    List<PacketMessage> list = PacketQueue.Instance.PopAll();
    foreach (PacketMessage packet in list)
    {
        Action<PacketSession, IMessage>? handler = ClientPacketHandler.Instance.GetPacketHandler(packet.Id);
        if (handler != null && packet.Message != null)
            handler.Invoke(session, packet.Message);
    }
}
