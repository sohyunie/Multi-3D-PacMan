#pragma comment(lib, "ws2_32")
#pragma comment (lib, "WS2_32.LIB")
#include "NetworkManager.h"
//#include <winsock2.h>

char buf[BUFSIZE2 + 1]; // 데이터 송수신 버퍼
HANDLE hWrite_Event, hRead_Event;

char address[128];
SOCKET g_s_socket;
const short SERVER_PORT = 4000;

char g_recv_buf[BUFSIZE];
WSABUF mybuf_r;
WSABUF mybuf;
bool canSend = true;

// 소켓 함수 오류 출력
void error_display(int err_no)
{
    WCHAR* lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err_no,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, 0);
    wcout << lpMsgBuf << endl;
    while (true);
    LocalFree(lpMsgBuf);
}

void do_recv()
{
    cout << "do_recv()" << endl;
    mybuf_r.buf = g_recv_buf;
    mybuf_r.len = BUFSIZE;
    DWORD recv_flag = 0;
    WSAOVERLAPPED* recv_over = new WSAOVERLAPPED;
    ZeroMemory(recv_over, sizeof(recv_over));
    int ret = WSARecv(g_s_socket, &mybuf_r, 1, 0, &recv_flag, recv_over, recv_callback);
    if (SOCKET_ERROR == ret) {
        int err_num = WSAGetLastError();
        if (WSA_IO_PENDING != err_num) {
            cout << "Send Error";
            error_display(err_num);
        }
    }
}

void do_send(int input)
{
    cout << "do_send()" << endl;
    if (!canSend) {
        cout << "보낼 수 없는 상황입니다." << endl;
        return;
    }
    canSend = false;
    buf[0] = (char)input;

    //cout << input << endl;
    DWORD sent_byte;
    mybuf.buf = buf;
    mybuf.len = static_cast<ULONG>(strlen(buf)) + 1;	// 문자열 길이 , NULL 까지 같이 전송? 이해 못함.. 

    WSAOVERLAPPED* send_over = new WSAOVERLAPPED;
    ZeroMemory(send_over, sizeof(*send_over));

    int ret = WSASend(g_s_socket, &mybuf, 1, 0, 0, send_over, send_callback);	// &mybuf 는 구조체 배열을 위함
    // send_callback 콜백 등록, 등록되면 계속 호출되나 봄.
    if (SOCKET_ERROR == ret) {
        cout << "Send Error";
        int err_num = WSAGetLastError();
        error_display(err_num);
    }
}

void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag)
{
    cout << "send_callback" << endl;
    delete send_over;
    canSend = true;
    //do_send();
}

void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag)
{
    cout << "recv_callback" << endl;
    delete recv_over;

    char* p = g_recv_buf;

    while (p < g_recv_buf + num_bytes)
    {
        char packet_size = *p;
        int c_id = *(p + 1);
        cout << "Client [" << c_id << "] Sent[" << packet_size - 2 << "bytes]: " << "i : " << *(p + 2) << "j : " << *(p + 1) << endl;

        p = p + packet_size;
    }
    do_recv();
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;
    }

    return (len - left);
}

DWORD WINAPI Network(LPVOID arg) {
    char sInputIP[30];

    cout << "ip 주소를 입력해주세요." << endl;
    cin.getline(address, sizeof(address));
    cout << address << "와 연결되었습니다" << endl;

	// 서버 connect
	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	g_s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, address, &server_addr.sin_addr);
	int ret = connect(g_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));		// reinterret_cast : type casting
	cout << "ret : " << ret << endl;
	if (SOCKET_ERROR == ret) {
		int err_num = WSAGetLastError();
		if (WSA_IO_PENDING != err_num) {
			cout << "Send Error";
			error_display(err_num);
		}
	}


    int tcp_option = 1;
    setsockopt(g_s_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&tcp_option), sizeof(tcp_option));
    do_recv(); // 리시브 콜백 설정

    return 1;
}