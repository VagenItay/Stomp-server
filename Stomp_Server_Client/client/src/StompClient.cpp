#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include "../include/event.h"
#include "StompProtocol.h"
#include "ReadSocket.h"
#include <boost/algorithm/string.hpp>
#include <thread>
int main()
{
    Game game = Game();
    StompProtocol clientStompProtocol = StompProtocol(false, game);
    std::string host;
    string msgToSendLogin;
    short port;
    atomic_bool flag(true);
    atomic_bool inCheckDiss(false);
    while (1)
    {
        while (flag)
        {
            const short bufsize = 1024;
            char buf[bufsize];
            std::cin.getline(buf, bufsize);
            std::string command(buf);
            vector<string> input;
            boost::split(input, command, boost::is_any_of(" "));
            if (input[0] == "login")
            {
                string hostPort = input[1];
                vector<string> hostPortData;
                boost::split(hostPortData, hostPort, boost::is_any_of(":"));
                host = hostPortData[0];
                port = stoi(hostPortData[1]);
                msgToSendLogin = clientStompProtocol.processInputFromKeyboard(input);
                flag = false;
            }
            else
            {
                continue;
            }
        }

        ConnectionHandler connectionHandler(host, port);
        if (!connectionHandler.connect())
        {
            std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
            return 1;
        }

        atomic_bool isFinished(false);
        ReadSocket readsocket1(connectionHandler, clientStompProtocol, isFinished, flag, inCheckDiss);
        std::thread th1(&ReadSocket::operator(), &readsocket1);

        if (!connectionHandler.sendLine(msgToSendLogin))
        {
            std::cout << "Disconnected. Exiting...\n"
                      << std::endl;
        }

        while (1)
        {
            const short bufsize = 1024;
            char buf[bufsize];

            if (isFinished)
                break;

            std::cin.getline(buf, bufsize);

            std::string command(buf);
            vector<string> input;
            boost::split(input, command, boost::is_any_of(" "));

            if (isFinished)
                break;

            if (input[0] != "summary")
            {
                string msgToSend = clientStompProtocol.processInputFromKeyboard(input);

                if (!connectionHandler.sendLine(msgToSend))
                {
                    std::cout << "Disconnected. Exiting...\n"<< std::endl;
                    break;
                }

                if (input[0] == "disconnect")
                {
                    inCheckDiss = true;
                    while (inCheckDiss)
                    {
                        // waiting to get approve from server
                    }
                    break;
                }
            }
            else
            {
                if (isFinished)
                    break;
                clientStompProtocol.processSummary(input);
            }
        }

        th1.join();
    }
    return 0;
}