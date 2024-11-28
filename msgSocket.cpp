#include "msgSocket.h"

using namespace std;

//初始化Socket
bool init_Socket()
{
    WORD wVersion = MAKEWORD(2, 2);//MAKEWORD(2,2)可以同时接收和发送，支持多协议，支持异步
    WSADATA wsadata;
    if (0 != WSAStartup(wVersion, &wsadata)){
        QMessageBox::warning(nullptr,QString("WSAStartup"),QString("Error in init_Socket()"),QMessageBox::Cancel);
        return false;
    }
    return true;
}

//反初始化
bool close_Socket()
{
    if (0 != WSACleanup()){
        QMessageBox::warning(nullptr,QString("WSACleanup"),QString("Error in close_Socket()"),QMessageBox::Cancel);
    }
    return true;
}

// 获取对方IP、PORT
bool getPeerSock(SOCKET me, char* peerAddr, UINT& peerPort)
{
    sockaddr_in name = { AF_INET };
    int lenname = sizeof(name);
    if (getpeername(me, (sockaddr*)&name, &lenname) < 0)
        return false;
    strcpy(peerAddr, inet_ntoa(name.sin_addr));
    peerPort = htons(name.sin_port);
    return true;
}

// 获取本机IP、PORT
bool getMySock(SOCKET me, char* peerAddr, UINT& peerPort)
{
    sockaddr_in name = { AF_INET };
    int lenname = sizeof(name);
    if (getsockname(me, (sockaddr*)&name, &lenname) < 0)
        return false;
    strcpy(peerAddr, inet_ntoa(name.sin_addr));
    peerPort = htons(name.sin_port);
    return true;
}

// 创建socket
bool createSocket(SOCKET &fd, int nSockType)
{
    //创建一个空的socket
    fd = socket(AF_INET, nSockType, IPPROTO_TCP);
    if (INVALID_SOCKET == fd){
        // QMessageBox::warning(nullptr,QString("socket"),QString("Error in createSocket()"),QMessageBox::Cancel);
        return false;
    }

    //连接server
    if(connectServer(fd,serverAddr, msgPort))
        return true;
    else{
        // QMessageBox::warning(nullptr,QString("connectServer"),QString("Error in createSocket()"),QMessageBox::Cancel);
        return false;
    }
}

// 连接server
bool connectServer(SOCKET &fd,LPCSTR lpszHostAddress, UINT nPort)
{
    //给socket绑定服务端ip地址和端口号
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(nPort);
    sa.sin_addr.s_addr = inet_addr(lpszHostAddress);
    if(connect(fd, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
        return false;
    else return true;
}

// 逐条发送
int Send(SOCKET &fd,const void* lpBuf, int nBufLen, int nFlags)
{
    return send(fd, (const char*)lpBuf, nBufLen, nFlags);
}

// 逐条接收
int Receive(SOCKET &fd,void* lpBuf, int nBufLen, int nFlags)
{
    return recv(fd, (char*)lpBuf, nBufLen, nFlags);
}

// 接收消息
int recvMsg(SOCKET socket,myMsg &msg){
    msg.setmsgType(msgType::NONE);
    msg.setmsgContent("");
    char buffer[bufSize + 1] = {'\0'};
    int ret = -1;
    while(1){
        ret = recv(socket, buffer, bufSize, 0);
        if(ret < 0){ //No receive data
            if(errno == EINTR) continue;
            else{
                QMessageBox::warning(nullptr,QString("recv"),QString("error in recvMsg()"));
                break;
            }
        }
        else if(ret == 0){
            continue;
        }else{
            msg = myMsg::fromString(buffer);
            break;
        }
    }
    return ret;
}

//发送消息
int sendMsg(SOCKET socket,myMsg &msg){
    std::string msgstr = msg.toString();
    const char *buffer = msgstr.c_str();
    int ret = -1;
    int cnt = strlen(buffer);
    ret = send(socket,buffer,cnt,0);
    if(ret < 0)
        QMessageBox::warning(nullptr,QString("send"),QString("error in sendMsg()"));
    return ret;
}
