#include <thread>
#include "ReadSocket.h"
#include <boost/algorithm/string.hpp>
#include <vector>
using namespace std;

ReadSocket::ReadSocket(ConnectionHandler &connectionHandler_, StompProtocol &clientStompProtocol_, atomic_bool &isFinished_, atomic_bool &flag_, atomic_bool &inCheckDiss_) : connectionHandler(connectionHandler_),
                                                                                                                                                                              clientStompProtocol(clientStompProtocol_), isFinished(isFinished_), flag(flag_), inCheckDiss(inCheckDiss_) {}

void ReadSocket::operator()()
{
    while (1)
    {
        if (isFinished)
            break;
        std::string answer;

        if (!connectionHandler.getLine(answer))
        {
            std::cout << "Disconnected. Exiting...\n"
                      << std::endl;
            break;
        }

        vector<string> output;
        boost::split(output, answer, boost::is_any_of("\n"));

        if (isFinished)
            break;

        string verifyOutput = clientStompProtocol.processReadFromSocket(output, answer);
        if (verifyOutput == "diss")
        {
            std::cout << "Exiting...\n" << std::endl;
            isFinished = true;
            flag = true;
            inCheckDiss = false;
            clientStompProtocol.getIsConnected() = false;
            connectionHandler.close();
            break;
        }
        else if (verifyOutput.substr(0, 5) == "ERROR")
        {
            isFinished = true;
            flag = true;
            connectionHandler.close();
            clientStompProtocol.getIsConnected() = false;
            std::cout << verifyOutput << std::endl;
            std::cout << "Exiting...\n" << std::endl;
            break;
        }
        else
        {
            std::cout << verifyOutput << std::endl;
        }
    }
}