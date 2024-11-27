#ifndef MSGSOCKET_H
#define MSGSOCKET_H

#include "myMsg.h"
#include <QTcpSocket>

#include <winsock2.h>
#include <cstdio>
#include <cstring>
#include <QMessageBox>
#include <QString>
#include <windows.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

inline const char* serverAddr = "127.0.0.1";
const int msgPort = 20000;

const int bufSize = 1024 * 1;


bool init_Socket();
bool close_Socket();
bool getPeerSock(SOCKET me, char* peerAddr, UINT& peerPort);
bool getMySock(SOCKET me, char* peerAddr, UINT& peerPort);
bool createSocket(SOCKET &fd, int nSockType = SOCK_STREAM);
bool connectServer(SOCKET &fd,LPCSTR lpszHostAddress, UINT nPort);
int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
int recvMsg(SOCKET socket,myMsg &msg);
int sendMsg(SOCKET socket,myMsg &msg);

#endif // MSGSOCKET_H

