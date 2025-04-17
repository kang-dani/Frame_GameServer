using System.Net.Sockets;
using System.Net;


namespace ClientCSharp.Network
{
    public class ClientService
    {
        // 세션을 생성하는 팩토리 메서드
        Func<Session>? sessionFactory;

        // 서버에 연결을 요청하는 메서드
        public void Connect(IPEndPoint endPoint, Func<Session> sessionFactory, int count = 1)
        {
            for (int i = 0; i < count; i++)
            {
                // TCP 소켓 생성
                Socket socket = new Socket(endPoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
                this.sessionFactory = sessionFactory;

                // 비동기 연결 이벤트 설정
                SocketAsyncEventArgs args = new();
                args.Completed += OnConnectCompleted; // 연결 완료 이벤트 핸들러 등록
                args.RemoteEndPoint = endPoint; // 서버의 엔드포인트 설정
                args.UserToken = socket; // 연결 요청에 사용할 소켓을 UserToken에 저장

                RegisterConnect(args); // 연결 요청 등록
            }
        }

        // 비동기 연결을 요청하는 메서드
        void RegisterConnect(SocketAsyncEventArgs args)
        {
            Socket? socket = args.UserToken as Socket;
            if (socket == null)
                return;

            // 비동기 연결 요청, 대기 중이 아니면 즉시 완료 처리
            bool pending = socket.ConnectAsync(args);
            if (pending == false)
                OnConnectCompleted(null, args); // 대기 중이 아니면 직접 완료 처리
        }

        // 연결 완료 이벤트 핸들러
        void OnConnectCompleted(object? sender, SocketAsyncEventArgs args)
        {
            if (args.SocketError == SocketError.Success) // 연결 성공 시
            {
                if (sessionFactory != null)
                {
                    // 세션을 생성하고 소켓을 연결
                    Session session = sessionFactory.Invoke();
                    if (args.ConnectSocket != null)
                    {
                        session.Connect(args.ConnectSocket);
                        if (args.RemoteEndPoint != null)
                        {
                            session.OnConnected(args.RemoteEndPoint); // 세션 연결 후 콜백 호출
                        }
                    }
                }
            }
            else // 연결 실패 시 오류 메시지 출력
            {
                Console.WriteLine($"OnConnectCompleted Fail: {args.SocketError}");
            }
        }
    }

}
