#pragma once
#include "ConnectionHandler.h"
#include "StompProtocol.h"

class ReadSocket
{
private:
    ConnectionHandler &connectionHandler;
    StompProtocol &clientStompProtocol;
    atomic_bool &isFinished;
    atomic_bool &flag;
    atomic_bool &inCheckDiss;

    

public:
    ReadSocket(ConnectionHandler &connectionHandler_, StompProtocol &clientStompProtocol_, atomic_bool &isFinished_,
    atomic_bool &flag_,atomic_bool &inCheckDiss_);
    void operator()();

};