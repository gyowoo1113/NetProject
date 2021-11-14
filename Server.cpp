﻿#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>
#include "Player.h"
#include "Foothold.h"

#define SERVERPORT 9000
#define CLIENT_NUM 3

static int custom_counter;
vector<Foothold> Bottom;

struct PlayerMgr
{
	DWORD portnum;
	CPlayer player;
};

clock_t serverInit_time;
clock_t serverPre_time;
clock_t serverDelta_time;

bool IsCollisionPandF;
bool IsCollisionP1andP2;

bool IsNeedUpdateLoc;

bool isPlayingGame;

int portnum;

bool Win;

PlayerMgr Players[CLIENT_NUM];


HANDLE hClientThread; //클라이언트와 데이터 통신을 위한 쓰레드 핸들 변수
HANDLE hFootholdEvent; //발판 동기화 작업을 위한 이벤트 핸들 변수


void ServerInit();
BOOL IsOkGameStart(int PlayerCount);
void RecTimer();
void UpdateTimer();
float timeInterpolation();
void CreateMainGameScene();
void CheckCollideFoothold();

bool IsCollideFootholdByPlayer(Foothold foot, CPlayer& player);
bool IsCollidePlayerByPlayer(CPlayer& a, CPlayer& b);

void CheckCollidePlayers();
void UpdatePlayerLocation();
void UpdateClientKeyInput();
void CheckGameOver();
void SetCilentData(DWORD portnum);
void FootHoldInit();
void PlayerInit();
bool IsReadytoPlay(bool isReady);
DWORD WINAPI ProcessClient(LPVOID arg);

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0) break;

		left -= received;
		ptr += received;
	}

	return (len - left);
}

using namespace std;
int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	HANDLE hClientThread = {};

	while (1)
	{
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		if (IsOkGameStart(++custom_counter))
			break;
		hClientThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		

		// closesocket()
		closesocket(client_sock);
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

void ServerInit()
{

}

BOOL IsOkGameStart(int PlayerCount)
{
	if (PlayerCount == CLIENT_NUM)
		return TRUE;
	return FALSE;
}

void RecTimer()
{
}

void UpdateTimer()
{
}

float timeInterpolation()
{
	return 0;
}

void CreateMainGameScene()
{
}

void CheckCollideFoothold()
{
}

bool IsCollideFootholdByPlayer(Foothold foot, CPlayer& player)
{
	return false;
}

bool IsCollidePlayerByPlayer(CPlayer& a, CPlayer& b)
{
	return false;
}

void CheckCollidePlayers()
{
}

void UpdatePlayerLocation(CPlayer& p, InputData& input)
{
	if (input.dUp) p.dz = -0.1;
	if (input.dDown) p.dz = 0.1;
	if (input.dLeft) p.dx = -0.1;
	if (input.dRight) p.dx = 0.1;

	// 업데이트 중인지 판단 -> dx dz로 판단
	// 현재 키 입력 전부 안된상태 -> 0으로 초기화 (업데이트 중지)
	if (p.dz && !input.dUp && !input.dDown) p.dz = 0.0;
	if (p.dx && !input.dLeft && !input.dRight) p.dx = 0.0; 
}

void UpdateClientKeyInput()
{
}

void CheckGameOver()
{
}

void SetCilentData(DWORD portnum)
{

}

void FootHoldInit()
{
	Bottom.clear();
	MakeFoothold(Bottom);
	DeleteRandomFoothold(Bottom);
}

void PlayerInit()
{
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		Players[i].player.x = 0;	// 좌표 값 어떻게 설정할 것인지(랜덤 or 지정) 나중에 상의!
		Players[i].player.y = 5;
		Players[i].player.z = 0;
		Players[i].player.dx = 0;
		Players[i].player.dy = 0;
		Players[i].player.dz = 0;
		Players[i].player.fall = true;
		Players[i].player.Locate();
	}
}

DWORD __stdcall ProcessClient(LPVOID arg)
{
	// 플레이어 구분 후 좌표업데이트
	/*
	CPlayer* tPlayer;
	for (int i = 0; i < CLIENT_NUM; ++i)
		if (Players[i].threadId == GetCurretnThreadId()) tPlayer == &player[i].player;

	while (1)
	{
		UpdatePlayerLocation(tPlayer,PlayerMgr.input)
	}*/
}

bool IsReadytoPlay(bool isReady)
{

}