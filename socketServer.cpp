#include "socketServer.h"
#include "utils.h"
// #include "../DataMapper.h"
// #include "../EncryptService.h"

using namespace std;

socketServer::socketServer(QObject *parent) : QObject(parent) {
    msgserver = new QTcpServer();
    printf("start to listen\n");
    msgserver->listen(QHostAddress::Any,msgPort);

    // 创建用于通信的套接字
    printf("start to accept socket\n");
    msgsocket = msgserver->nextPendingConnection();
    printf("Succeed to accept client\n");

    thread_func();

    // 客户端是否断开连接
    connect(msgsocket, &QTcpSocket::disconnected, nullptr, [=](){
        printf("socket has disconnected\n");
        msgsocket->close();
        msgserver->close();
        delete msgsocket;
        delete msgserver;
        exit(-1);
    });
    // QObject::connect(msgserver, &QTcpServer::newConnection, this, [=](){

    // });
}


void* socketServer::thread_func(){
    EncryptionParameters parms(scheme_type::bfv);

    // 设置 SEAL 参数
    size_t poly_modulus_degree = 2048;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 20));
    EncryptService service(parms);

    int tableId = 0;

    while(1){
        // 初始化并接受msg
        myMsg msg(msgType::NONE);
        if(!recvMsg(msgsocket,msg)){
            printf("error in recvMsg");
            msgsocket->disconnectFromHost();
            exit(-1);
        }

        msgType type = msg.getmType();

        // 连接数据库
        if(type == msgType::CONNECTION_REQUEST){
            QJsonObject mContent = msg.getmContent();
            QString CONNINFO = mContent["CONNINFO"].toString();
            string conninfostr = CONNINFO.toStdString();
            PGconn *conn= PQconnectdb(conninfostr.c_str());
            // 连接成功
            if (PQstatus(conn) != CONNECTION_OK) {
                std::cerr << "Failed to connect to database: " << PQerrorMessage(conn) << std::endl;
                PQfinish(conn);
                myMsg msg2(msgType::CONNECTION_FAILED);
                sendMsg(msgsocket,msg2);
                continue;
            }
            // 连接失败
            else {
                myMsg msg2(msgType::CONNECTION_SUCCESS);
                sendMsg(msgsocket,msg2);
                std::cout << "Succeeded to connect to database\n";
            }
            service.setConn(conn);
        }
        // 上传文件
        else if(type == msgType::FILE_REQUEST){
            // 获取 文件名、文件大小
            QJsonObject mContent = msg.getmContent();
            QString filename_ = mContent["fileName"].toString();
            qint64 filesize = mContent["fileSize"].toInteger();
            // 创建 文件专用socket
            printf("start to accept socket for file\n");
            QTcpSocket * filesocket = msgserver->nextPendingConnection();
            printf("Succeed to accept socket for file\n");

            // 接收文件
            bool ret2 = recvFile(filesocket,filename_,filesize);
            filesocket->disconnectFromHost();
            printf("文件用代理已断开");
            filesocket->close();
            filesocket->disconnect();

            if(ret2){
                myMsg msg2(msgType::FILE_RECEIVE_SUCCESS);
                sendMsg(msgsocket,msg2);
            }else{
                myMsg msg2(msgType::FILE_RECEIVE_FAILED);
                sendMsg(msgsocket,msg2);
            }
            // 加密数据并上传到数据库
            std::string filename = filename_.toStdString();
            service.updateFileIntoSql(filename);
            // 尝试删除文件
            if (remove(filename.c_str()) != 0) {
                printf("Failed to delete file.\n");
            } else {
                printf("File deleted successfully.\n");
            }

        }
        // sql查询
        else if(type==msgType::SQL_REQUEST){
            QJsonObject mContent = msg.getmContent();
            QString sql = mContent["sql"].toString();
            std::string sqlquery = sql.toStdString();
            // 查询加密数据库，查询是否成功
            vector<vector<string>> sqlresult = service.executeSql(sqlquery);

            bool hasResult = !sqlresult.empty();
            if(hasResult){
                myMsg msg2(msgType::SQL_SUCCESS);
                sendMsg(msgsocket,msg2);
                // 创建 查询结果专用socket
                printf("start to accept socket for sql result\n");
                QTcpSocket * sqlsocket = msgserver->nextPendingConnection();
                printf("Succeeded to accept socket for sql result\n");

                bool ret2 = sendSqlResult(sqlsocket, sqlresult);
                sqlsocket->disconnectFromHost();
                printf("查询用代理已断开");
                sqlsocket->close();
                sqlsocket->disconnect();
            }else{
                myMsg msg2(msgType::SQL_FAILED);
                sendMsg(msgsocket,msg2);
            }
        }
        // 非正常消息：接受方或发送方出错，返回一个msgType::ERROR消息提醒对方
        else{
            printf("Warning: a illegal message(msgType::ERROR)\n");
            std::cout<< "Warning: a illegal message("<< type <<")\n";
            myMsg msg2(msgType::NONE);
            sendMsg(msgsocket,msg2);
        }
    }
}
