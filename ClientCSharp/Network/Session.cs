using System.Net.Sockets;
using System.Net;


namespace ClientCSharp.Network
{
    public abstract class Session
    {
        // 소켓 객체
        private Socket? socket;
        // 연결이 종료되었는지 확인하는 플래그 (0: 연결됨, 1: 연결 종료됨)
        private int disconnected = 0;

        // 수신 버퍼
        private readonly RecvBuffer recvBuffer = new(65535);

        // 동기화 객체
        private readonly object lockObj = new();
        // 보낼 데이터 큐
        private readonly Queue<ArraySegment<byte>> sendQueue = new();
        // 현재 전송 중인 데이터 리스트
        private readonly List<ArraySegment<byte>> bufferList = new();
        // 전송 이벤트 및 수신 이벤트 인스턴스
        private readonly SocketAsyncEventArgs sendEvent = new();
        private readonly SocketAsyncEventArgs recvEvent = new();

        // 추상 메서드들: 하위 클래스에서 구현 필요
        public abstract void OnConnected(EndPoint endPoint);
        public abstract int OnRecv(ArraySegment<byte> buffer);
        public abstract void OnSend(int numOfBytes);
        public abstract void OnDisconnected(EndPoint endPoint);

        // 소켓 연결 설정 및 이벤트 핸들러 등록
        public void Connect(Socket socket)
        {
            this.socket = socket;
            recvEvent.Completed += new EventHandler<SocketAsyncEventArgs>(ProcessRecv);
            sendEvent.Completed += new EventHandler<SocketAsyncEventArgs>(ProcessSend);

            RegisterRecv(); // 수신 대기 시작
        }

        // 데이터를 전송 큐에 추가 (복수 개의 버퍼)
        public void Send(List<ArraySegment<byte>> sendBuffList)
        {
            if (sendBuffList.Count == 0)
                return;

            lock (lockObj)
            {
                foreach (ArraySegment<byte> sendBuff in sendBuffList)
                    sendQueue.Enqueue(sendBuff);

                // 현재 전송 중이 아닌 경우에만 전송 등록
                if (bufferList.Count == 0)
                    RegisterSend();
            }
        }

        // 데이터를 전송 큐에 추가 (단일 버퍼)
        public void Send(ArraySegment<byte> sendBuff)
        {
            lock (lockObj)
            {
                sendQueue.Enqueue(sendBuff);
                if (bufferList.Count == 0)
                    RegisterSend();
            }
        }

        // 연결 해제
        public void Disconnect()
        {
            if (Interlocked.Exchange(ref disconnected, 1) == 1)
                return;

            if (socket != null)
            {
                if (socket.RemoteEndPoint != null)
                    OnDisconnected(socket.RemoteEndPoint);

                socket.Shutdown(SocketShutdown.Both);
                socket.Close();
            }

            Clear(); // 전송 및 수신 대기열 초기화
        }

        // 전송 및 수신 대기열 초기화
        private void Clear()
        {
            lock (lockObj)
            {
                sendQueue.Clear();
                bufferList.Clear();
            }
        }

        // 전송 작업 등록
        void RegisterSend()
        {
            if (disconnected == 1)
                return;

            // 큐에 있는 데이터를 bufferList로 이동
            while (sendQueue.Count > 0)
            {
                ArraySegment<byte> buff = sendQueue.Dequeue();
                bufferList.Add(buff);
            }
            sendEvent.BufferList = bufferList;

            try
            {
                // 비동기 전송 요청
                if (socket != null)
                {
                    bool pending = socket.SendAsync(sendEvent);
                    if (pending == false)
                        ProcessSend(null, sendEvent); // 비동기가 아닐 경우 바로 처리
                }
            }
            catch (Exception e)
            {
                Console.WriteLine($"RegisterSend Failed {e}");
            }
        }

        // 전송 완료 처리
        void ProcessSend(object? sender, SocketAsyncEventArgs args)
        {
            lock (lockObj)
            {
                if (args.BytesTransferred > 0 && args.SocketError == SocketError.Success)
                {
                    try
                    {
                        sendEvent.BufferList = null;
                        bufferList.Clear();

                        OnSend(sendEvent.BytesTransferred); // 전송 완료 콜백 호출

                        if (sendQueue.Count > 0)
                            RegisterSend(); // 남은 데이터가 있으면 다시 전송
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine($"OnSendCompleted Failed {e}");
                    }
                }
                else
                {
                    Disconnect(); // 오류가 있거나 전송 바이트가 0이면 연결 해제
                }
            }
        }

        // 수신 작업 등록
        void RegisterRecv()
        {
            if (disconnected == 1)
                return;

            recvBuffer.Clear(); // 수신 버퍼 초기화
            ArraySegment<byte> segment = recvBuffer.WritePos;
            recvEvent.SetBuffer(segment.Array, segment.Offset, segment.Count);

            try
            {
                // 비동기 수신 요청
                if (socket != null)
                {
                    bool pending = socket.ReceiveAsync(recvEvent);
                    if (pending == false)
                        ProcessRecv(null, recvEvent); // 비동기가 아닐 경우 바로 처리
                }
            }
            catch (Exception e)
            {
                Console.WriteLine($"RegisterRecv Failed {e}");
            }
        }

        // 수신 완료 처리
        void ProcessRecv(object? sender, SocketAsyncEventArgs args)
        {
            if (args.BytesTransferred > 0 && args.SocketError == SocketError.Success)
            {
                try
                {
                    // 수신 버퍼에 데이터 추가
                    if (recvBuffer.OnWrite(args.BytesTransferred) == false)
                    {
                        Disconnect();
                        return;
                    }

                    // 수신 데이터 처리 후 처리한 바이트 길이를 반환
                    int processLen = OnRecv(recvBuffer.ReadPos);
                    if (processLen < 0 || recvBuffer.DataSize < processLen)
                    {
                        Disconnect();
                        return;
                    }

                    // 처리한 바이트 수 만큼 읽기 위치 이동
                    if (recvBuffer.OnRead(processLen) == false)
                    {
                        Disconnect();
                        return;
                    }

                    RegisterRecv(); // 다음 수신 작업 등록
                }
                catch (Exception e)
                {
                    Console.WriteLine($"OnRecvCompleted Failed {e}");
                }
            }
            else
            {
                Disconnect(); // 오류가 있거나 수신 바이트가 0이면 연결 해제
            }
        }
    }
}
