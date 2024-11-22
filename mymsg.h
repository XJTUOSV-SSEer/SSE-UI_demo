#ifndef MYMSG_H
#define MYMSG_H

#include <cstdio>
#include <string>
#include <sstream>

enum class msgType : int
{
    none        = 0,
    conn        = 1,
    connresult  = 2,
    sql         = 3,
    sqlresult   = 4,
    file        = 5,
    fileresult  = 6,
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
        oss << "Message Type: ";
        int type = static_cast<int>(mtype);
        oss << std::to_string(type);
        // switch (mtype)
        // {
        // case msgType::conn:
        //     oss << "0x1";
        //     break;
        // case msgType::connresult:
        //     oss << "0x2";
        //     break;
        // case msgType::sql:
        //     oss << "0x3";
        //     break;
        // case msgType::sqlresult:
        //     oss << "0x4";
        //     break;
        // case msgType::file:
        //     oss << "0x5";
        //     break;
        // case msgType::fileresult:
        //     oss << "0x6";
        //     break;
        // default:
        //     oss << "0x0";
        //     break;
        // }
        oss << ", Message Content: " << msgContent;
        return oss.str();
    }

    static myMsg fromString(const std::string& str)
    {
        std::istringstream iss(str);
        myMsg message;
        std::string typeStr;
        std::getline(iss, typeStr, ','); // Read until the first comma
        typeStr = typeStr.substr(12);
        int type = std::stoi(typeStr);

        message.mtype = static_cast<enum msgType>(type);

        // Read the rest of the string as msgContent
        std::getline(iss, message.msgContent, '\n'); // Assuming the content is separated by a newline

        return message;
    }

private:
    msgType mtype;
    std::string msgContent;
};

#endif // MYMSG_H
