using Google.Protobuf;

namespace ClientCSharp.Packet
{
    public class PacketMessage
    {
        public ushort Id { get; set; }
        public IMessage Message { get; set; }
    }

    public class PacketQueue
    {
        public static PacketQueue Instance { get; } = new PacketQueue();

        readonly Queue<PacketMessage> packetQueue = new();
        private readonly object lockObj = new();

        public void Push(ushort id, IMessage packet)
        {
            lock (lockObj)
            {
                packetQueue.Enqueue(new PacketMessage() { Id = id, Message = packet });
            }
        }

        public PacketMessage Pop()
        {
            lock (lockObj)
            {
                if (packetQueue.Count == 0)
                    return null;

                return packetQueue.Dequeue();
            }
        }

        public List<PacketMessage> PopAll()
        {
            List<PacketMessage> list = new List<PacketMessage>();

            lock (lockObj)
            {
                while (packetQueue.Count > 0)
                    list.Add(packetQueue.Dequeue());
            }

            return list;
        }
    }
}
