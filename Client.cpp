﻿#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#include <winsock2.h>
#include <cstdlib>
#include "Foothold.h"
#include "filetobuf.h"
#include "Player.h"
#define pi 3.141592

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid KeyboardUp(unsigned char key, int x, int y);
void Timerfunction(int value);
void make_vertexShader();
void make_fragmentShader();

void renderBitmapCharacher(float, float, float, void*, char*);
void Print_word(float, float, float, float, int, char*);
void Print_GameState();	// 현재 게임 상태에 따라 화면에 Title String, Waiting String, over을 출력하는 함수 

void err_quit(const char* msg); // 소켓 함수 오류 출력 후 종료
void err_display(const char* msg); // 소켓 함수 오류 출력

int g_window_w;
int g_window_h;

GLuint vao, vbo[2];
GLuint s_program;

GLchar* vertexsource, * fragmentsource;
GLuint vertexshader, fragmentshader;
float theta = 0;
float cx = 1, cy = 1, cz = 1;

int font = (int)GLUT_BITMAP_TIMES_ROMAN_24;
int score = 0;
int cnt = 0;
int p_cnt = 0;
int tine = 0;
int p_time = 0;

char char_score[256];
char word1[10] = "score:";
char word2[11] = "life time:";
char over[9] = "You Lose";
char win[8] = "You Win";
char TitleString[16] = "Press Enter Key";
char WaitString[13] = "Waiting. . .";

clock_t past;
clock_t present;
clock_t start;

HANDLE hDrawEvent;
CRITICAL_SECTION cs;

enum class EGameState
{
	TITLE,
	WAITING,
	PLAYING,
	GAMEOVER
};

volatile int CurrentGameState = static_cast<int>(EGameState::TITLE);

DWORD WINAPI ClientMain(LPVOID arg);

GLfloat	box[][3] = {
	{ -0.5, 0, 0.5 },
	{ 0.5, 0, 0.5 },
	{ 0.5,1, 0.5 },

	{ 0.5, 1, 0.5 },
	{ -0.5, 1, 0.5 },
	{ -0.5, 0, 0.5 }, // 앞면

	{ -0.5, 0, -0.5 },
	{ 0.5, 0, -0.5 },
	{ -0.5, 0, 0.5 },

	{ 0.5, 0, -0.5 },
	{ 0.5, 0, 0.5 },
	{ -0.5, 0, 0.5 }, // 밑면

	{ -0.5,0, 0.5 },
	{ -0.5, 1, 0.5 },
	{ -0.5, 0, -0.5 },

	{ -0.5, 1, 0.5 },
	{ -0.5, 1, -0.5 },
	{ -0.5, 0, -0.5 }, //왼옆면 (바라보는기준)

	{ -0.5, 0, -0.5 },
	{ 0.5, 0, -0.5 },
	{ -0.5, 1, -0.5 },

	{ 0.5, 0, -0.5 },
	{ 0.5, 1, -0.5 },
	{ -0.5, 1, -0.5 }, //뒷면

	{ -0.5, 1, 0.5 },
	{ 0.5, 1, 0.5 },
	{ -0.5, 1, -0.5 },

	{ -0.5, 1, -0.5 },
	{ 0.5, 1, 0.5 },
	{ 0.5,1, -0.5 }, //윗면

	{ 0.5, 1, -0.5 },
	{ 0.5, 1, 0.5 },
	{ 0.5,0, -0.5 },

	{ 0.5, 1, 0.5 },
	{ 0.5, 0, 0.5 },
	{ 0.5, 0, -0.5 }, //오른옆면
};
GLfloat	boxN[][3] = {
	 0.0f,  0.0f,  1.0f,
	 0.0f,  0.0f,  1.0f,
	 0.0f,  0.0f,  1.0f,

	 0.0f,  0.0f,  1.0f,
	 0.0f,  0.0f,  1.0f,
	 0.0f,  0.0f,  1.0f, // 앞면

	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,

	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f, // 밑면

	  -1.0f,  0.0f,  0.0f,
	  -1.0f,  0.0f,  0.0f,
	  -1.0f,  0.0f,  0.0f,

	  -1.0f,  0.0f,  0.0f,
	  -1.0f,  0.0f,  0.0f,
	  -1.0f,  0.0f,  0.0f, //옆면

	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,

	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f, //뒷면

	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,

	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f, //윗면

	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,

	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
};

std::vector<Foothold> Bottom;
CPlayer player;
///////////////////////////////////////////////////////////////////////////////////////
struct SendGameData
{
	PlayerMgr PMgr[CLIENT_NUM];
	Foothold Bottom[N * N * N];
	volatile int ServerTime;
};

SendPlayerData myPlayer;
PlayerMgr players[CLIENT_NUM];

SOCKADDR_IN serveraddr;
SendGameData ServerDatas;

//#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

std::string ServerIP;

HANDLE hFootholdEvent; //발판 동기화 작업을 위한 이벤트 핸들 변수

void TimerFunc();
bool IsGameOverState();
bool IsChangeCamera();
int myIndex = -1;
int otherIndex = -1;
int showIndex = -1;
bool bChangeCam = false;

volatile int CurrentTime = 60; // 현재 남은 게임 시간
bool check = false;


////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////
void init()
{
	glEnable(GL_DEPTH_TEST);
}

void make_vertexShader()
{
	vertexsource = filetobuf("vertex.glsl");
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);
	glCompileShader(vertexshader);
	checkCompileErrors(vertexshader, "vertex shader");
}

void make_fragmentShader()
{
	fragmentsource = filetobuf("fragment.glsl");
	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
	glCompileShader(fragmentshader);
	checkCompileErrors(fragmentshader, "fragment shader"); 
}

void InitBuffer()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxN), boxN, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

void InitShader()
{
	make_vertexShader();
	make_fragmentShader();

	s_program = glCreateProgram();
	glAttachShader(s_program, vertexshader);
	glAttachShader(s_program, fragmentshader);
	glLinkProgram(s_program);
	glDeleteShader(vertexshader);
	glDeleteShader(fragmentshader);

	glUseProgram(s_program);
}

int main(int argc, char** argv)
{
	cout << "IP주소 입력: ";
	cin >> ServerIP;

	hDrawEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	InitializeCriticalSection(&cs);
	if (hDrawEvent == NULL)
		return 1;

	srand((unsigned int)time(NULL));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(400, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("기말");

	glewExperimental = GL_TRUE;
	glewInit();
	InitShader();
	InitBuffer();

	init();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutMainLoop();

	DeleteCriticalSection(&cs);
	return 0;
}

void Timerfunction(int value)
{
	if (!check && myIndex == 1)
	{
		check = true;
		Sleep(50);
	}

	glutTimerFunc(50, Timerfunction, 1);
	glutPostRedisplay();
}

void renderBitmapCharacher(float x, float y, float z, void* font, char* string)
{
	char* c;
	glRasterPos3f(x, y, z);
	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(font, *c);
	}
}

void Print_word(float word_x, float word_y, float x, float y, int num, char* word)
{
	renderBitmapCharacher(word_x, word_y, 0, (void*)font, word);
	sprintf(char_score, "%d", num);
	renderBitmapCharacher(x, y, 0, (void*)font, char_score);
}

float radius = 5, camX = 0, camY = 0, camZ = 0;
float lx = 3.0, ly = 3.5, lz = -1.5, ltheta = 0;
float Lx = 0, Ly = 0, Lz = 0;
float aml = 0.35f;

GLvoid drawScene()
{
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	EnterCriticalSection(&cs);
	glUseProgram(s_program);

	GLUquadricObj* armL, * armR;
	armL = gluNewQuadric();
	armR = gluNewQuadric();

	glm::mat4 ptrans = glm::mat4(1.0f);
	glm::mat4 vtrans = glm::mat4(1.0f);

	Lx = (float)cos(ltheta / 180 * 3.141592) * lx;			// 조명 회전
	Ly = ly;
	Lz = (float)sin(ltheta / 180 * 3.141592) * (-lz);

	unsigned int lightPosLocation = glGetUniformLocation(s_program, "lightPos");
	glUniform3f(lightPosLocation, Lx, Ly, Lz);
	unsigned int lightColorLocation = glGetUniformLocation(s_program, "lightColor");
	glUniform3f(lightColorLocation, cx, cy, cz);
	unsigned int amlight = glGetUniformLocation(s_program, "ambientLight");
	glUniform1f(amlight, aml);

	unsigned int color_location = glGetUniformLocation(s_program, "objectColor");
	unsigned int model = glGetUniformLocation(s_program, "model");

	camX = (float)sin(theta / 180 * 3.141592) * radius;
	camY = +0.0;
	camZ = -1 * (float)cos(theta / 180 * 3.141592) * radius;
	// 자신의 플레이어 인덱스 구분을 통해서 출력 위치 변경필요

	if (myIndex != -1)
	{
		showIndex = (bChangeCam) ? otherIndex : myIndex;
		if (showIndex == -1) showIndex = myIndex;

		vtrans = glm::lookAt(glm::vec3((ServerDatas.PMgr[showIndex]).player.x, (ServerDatas.PMgr[showIndex]).player.y + 2, (ServerDatas.PMgr[showIndex]).player.z + 2),
			glm::vec3((ServerDatas.PMgr[showIndex]).player.x, (ServerDatas.PMgr[showIndex]).player.y, (ServerDatas.PMgr[showIndex]).player.z), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	unsigned int view = glGetUniformLocation(s_program, "view");
	glUniformMatrix4fv(view, 1, GL_FALSE, &vtrans[0][0]);

	unsigned int projection = glGetUniformLocation(s_program, "projection");
	ptrans = glm::perspective(glm::radians(45.0f), (float)g_window_w / (float)g_window_h, 0.1f, 100.0f);
	ptrans = glm::translate(ptrans, glm::vec3(0, 0, -5.0));

	glUniformMatrix4fv(projection, 1, GL_FALSE, &ptrans[0][0]);

	glBindVertexArray(vao);

	for (size_t i = 0; i < N * N * N; ++i)
	{
		if (ServerDatas.Bottom[i].Del) continue;

		ServerDatas.Bottom[i].Draw_Start();
		glUniform3f(color_location, ServerDatas.Bottom[i].r, ServerDatas.Bottom[i].g, ServerDatas.Bottom[i].b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4(ServerDatas.Bottom[i].Drawing)));
		glDrawArrays(GL_TRIANGLES, 0, ServerDatas.Bottom[i].size);
	}

	for (size_t i = 0; i < CLIENT_NUM; ++i)
	{
		// head
		glUniform3f(color_location, (ServerDatas.PMgr[i]).player.head.r, (ServerDatas.PMgr[i]).player.head.g, (ServerDatas.PMgr[i]).player.head.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas.PMgr[i]).player.head.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// nose
		glUniform3f(color_location, (ServerDatas.PMgr[i]).player.nose.r, (ServerDatas.PMgr[i]).player.nose.g, (ServerDatas.PMgr[i]).player.nose.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas.PMgr[i]).player.nose.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// body
		glUniform3f(color_location, (ServerDatas.PMgr[i]).player.body.r, (ServerDatas.PMgr[i]).player.body.g, (ServerDatas.PMgr[i]).player.body.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas.PMgr[i]).player.body.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// arm
		glUniform3f(color_location, (ServerDatas.PMgr[i]).player.arm_l.r, (ServerDatas.PMgr[i]).player.arm_l.g, (ServerDatas.PMgr[i]).player.arm_l.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas.PMgr[i]).player.arm_l.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glUniform3f(color_location, (ServerDatas.PMgr[i]).player.arm_r.r, (ServerDatas.PMgr[i]).player.arm_r.g, (ServerDatas.PMgr[i]).player.arm_r.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas.PMgr[i]).player.arm_r.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// leg
		glUniform3f(color_location, (ServerDatas.PMgr[i]).player.leg_l.r, (ServerDatas.PMgr[i]).player.leg_l.g, (ServerDatas.PMgr[i]).player.leg_l.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas.PMgr[i]).player.leg_l.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glUniform3f(color_location, (ServerDatas.PMgr[i]).player.leg_r.r, (ServerDatas.PMgr[i]).player.leg_r.g, (ServerDatas.PMgr[i]).player.leg_r.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas.PMgr[i]).player.leg_r.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	if (myIndex != -1)
		Print_word(0.5f, 0.8f, 0.7f, 0.8f, (ServerDatas.PMgr[myIndex]).player.m_nScore, word1);

	Print_GameState();

	glutSwapBuffers();
	LeaveCriticalSection(&cs);

	SetEvent(hDrawEvent);
}

void Print_GameState()
{
	switch (CurrentGameState)
	{
		case static_cast<int>(EGameState::TITLE):
		{
			renderBitmapCharacher(-0.2f, 0.0f, 0, (void*)font, TitleString);
			break;
		}
		case static_cast<int>(EGameState::WAITING):
		{
			renderBitmapCharacher(-0.2f, 0.0f, 0, (void*)font, WaitString);
			break;
		}
		case static_cast<int>(EGameState::GAMEOVER):
		{
			if ((ServerDatas.PMgr[myIndex]).Win)
				renderBitmapCharacher(-0.2f, 0.0f, 0, (void*)font, win);
			else
				renderBitmapCharacher(-0.2f, 0.0f, 0, (void*)font, over);
			break;
		}
		case static_cast<int>(EGameState::PLAYING):
		{
			char cTime[5];
			_itoa(CurrentTime, cTime, 10);
			renderBitmapCharacher(0.0f, 0.9f, 0, (void*)font, cTime);
			break;
		}
		default:
			break;
	}
}

GLvoid Reshape(int w, int h)
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case VK_RETURN:
		{
			if (CurrentGameState == static_cast<int>(EGameState::TITLE))
			{
				CurrentGameState = static_cast<int>(EGameState::WAITING);
				CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
				glutTimerFunc(50, Timerfunction, 1);
				hFootholdEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
			}
			break;
		}
		case 'Q':
		case 'q':
		{
			exit(0);
			break;
		}
		case 'W':
		case 'w':
		{
			if (CurrentGameState == static_cast<int>(EGameState::PLAYING))
			{
				myPlayer.Input.bUp = true;
			}
			break;
		}
		case 'A':
		case 'a':
		{
			if (CurrentGameState == static_cast<int>(EGameState::PLAYING))
			{
				myPlayer.Input.bLeft = true;
			}
			break;
		}
		case 'S':
		case 's':
		{
			if (CurrentGameState == static_cast<int>(EGameState::PLAYING))
			{
				myPlayer.Input.bDown = true;
			}
			break;
		}
		case 'D':
		case 'd':
		{
			if (CurrentGameState == static_cast<int>(EGameState::PLAYING))
			{
				myPlayer.Input.bRight = true;
			}
			break;
		}
		case 32:
		{
			if (CurrentGameState == static_cast<int>(EGameState::PLAYING))
			{
				myPlayer.Input.bSpace = true;
			}
			break;
		}
		case 'p':
		{
			if (CurrentGameState == static_cast<int>(EGameState::PLAYING))
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			break;
		}
		case 'P':
		{
			if (CurrentGameState == static_cast<int>(EGameState::PLAYING))
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			break;
		}
		case 'v':
		case 'V':
			if (IsChangeCamera() || bChangeCam) bChangeCam = !bChangeCam;
			break;
	}

	glutPostRedisplay();
}

GLvoid KeyboardUp(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'W':
		case 'w':
		{
			myPlayer.Input.bUp = false;
			break;
		}
		case 'S':
		case 's':
		{
			myPlayer.Input.bDown = false;
			break;
		}
		case 'A':
		case 'a':
		{
			myPlayer.Input.bLeft = false;
			break;
		}
		case 'D':
		case 'd':
		{
			myPlayer.Input.bRight = false;
			break;
		}
		case 32:
		{
			myPlayer.Input.bSpace = false;
			break;
		}
	}

	glutPostRedisplay();
}

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

DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ServerIP.c_str());
	//serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	if (CurrentGameState == static_cast<int>(EGameState::WAITING))
	{
		retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR)
			err_quit("connect()");
	}

	int CurrentPlayerNum = 0;
	recvn(sock, (char*)&CurrentPlayerNum, sizeof(int), 0);
	if (CurrentPlayerNum == CLIENT_NUM)
	{
		CurrentGameState = static_cast<int>(EGameState::PLAYING);
	}

	int nClientDataLen = sizeof(SendPlayerData);
	int nServerDataLen = sizeof(SendGameData);

	while (1)
	{
		WaitForSingleObject(hDrawEvent, INFINITE);
		EnterCriticalSection(&cs);

		DWORD retval = WaitForSingleObject(hFootholdEvent, INFINITE);

		// myPlayer 송신
		send(sock, (char*)&myPlayer, nClientDataLen, 0);
		
		// ServerGameData 수신
		recvn(sock, (char*)&ServerDatas, nServerDataLen, 0);

		CurrentTime = ServerDatas.ServerTime;
		for (int i = 0; i < CLIENT_NUM; ++i)
		{
			if (ServerDatas.PMgr[i].mine) myIndex = i;
			else otherIndex = i;
		}

		if (IsGameOverState())
		{
			CurrentGameState = static_cast<int>(EGameState::GAMEOVER);

			myPlayer.Input.bUp = false;
			myPlayer.Input.bDown = false;
			myPlayer.Input.bLeft = false;
			myPlayer.Input.bRight = false;
			myPlayer.Input.bSpace = false;
		}

		SetEvent(hFootholdEvent);

		LeaveCriticalSection(&cs);
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

bool IsGameOverState()
{
	for (int i = 0; i < CLIENT_NUM; ++i)
		if (!ServerDatas.PMgr[i].bGameOver) return false;
	return true;
}

bool IsChangeCamera()
{
	if (CurrentGameState != static_cast<int>(EGameState::PLAYING)) return false;
	if (!ServerDatas.PMgr[myIndex].bGameOver) return false;
	if (ServerDatas.PMgr[otherIndex].bGameOver) return false;

	return true;
}