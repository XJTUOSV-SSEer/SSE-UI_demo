#ifndef MYMSG_H
#define MYMSG_H

#include <cstdio>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

enum class msgType : int
{
    NONE                   = 0,
    CONNECTION_REQUEST      = 1,
    CONNECTION_SUCCESS      = 2,
    CONNECTION_FAILED       = 3,
    FILE_REQUEST            = 4,
    FILE_RECEIVE_SUCCESS    = 5,
    FILE_RECEIVE_FAILED     = 6,
    SQL_REQUEST             = 7,
    SQL_SUCCESS             = 8,
    SQL_FAILED              = 9,
};

class myMsg
{
public:
    myMsg();
    myMsg(msgType mtype);
    myMsg(msgType mtype, std::string mcontent);

    msgType getmsgType();
    std::string getmsgContent();

    void setmsgType(msgType);
    void setmsgContent(std::string);

    // @Override
    std::string toString() const{
        std::ostringstream oss;
        int type = static_cast<int>(mtype);
        oss << std::setw(4)<<std::setfill('0')<<type;
        oss << " ";
        oss << msgContent;
        return oss.str();
    }

    static myMsg fromString(char str[])
    {
        std::istringstream iss(str);
        myMsg message;
        std::string typepart(str,0,4);
        std::string contpart(&str[5]);
        typepart = typepart.erase(0, typepart.find_first_not_of('0'));
        int type = std::stoi(typepart);
        message.mtype = static_cast<enum msgType>(type);
        message.msgContent = contpart;
        return message;
    }

private:
    msgType mtype;
    std::string msgContent;
};

#endif // MYMSG_H
