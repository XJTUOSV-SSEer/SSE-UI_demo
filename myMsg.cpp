#include "myMsg.h"

myMsg::myMsg() {
    this->mtype = msgType::NONE;
    this->msgContent = "\0";
}

myMsg:: myMsg(enum msgType mtype){
    this->mtype = mtype;
    this->msgContent = "\0";
}

myMsg:: myMsg(enum msgType mtype, std::string mcontent){
    this->mtype = mtype;
    this->msgContent = mcontent;
}

msgType myMsg:: getmsgType(){
    return this->mtype;
}

std::string myMsg:: getmsgContent(){
    return this->msgContent;
}

void myMsg:: setmsgType(enum msgType mtype){
    this->mtype = mtype;
}

void myMsg:: setmsgContent(std::string mcontent){
    this->msgContent = mcontent;
}
