
#pragma once

#include "../include/ConnectionHandler.h"
#include "event.h"
#include <string>
#include "Game.h"
#include <iostream>
// #include <boost/asio.hpp>
#include <unordered_map>
using namespace std;
// using boost::asio::ip::tcp;

class StompProtocol
{
private:
    volatile bool isConnected;
    int SubsCounter;
    int receiptCounter;
    unordered_map<string,int> channelNameToSubID;
    unordered_map<int,string> receiptIdToMsgHeader;
    unordered_map<int,string> receiptIdToTopicName;
    string connectedUser;
    Game game;


public:
    StompProtocol(bool connect, Game& game);
    //StompProtocol();
    string processInputFromKeyboard(vector<string> input);
    string processReadFromSocket(vector<string> input, string ans);
    string processReceipt(vector<string> input);
    string processError(vector<string> input);
    string processMessage(vector<string> input);
    string generateFrameLogin(vector<string> input);
    string generateFrameJoin(vector<string> input);
    string generateFrameExit(vector<string> input);
    string generateFrameReport(vector<string> input);
    void generateFrameSummary(vector<string> input);
    string generateFrameLogout(vector<string> input);
    string generateFrameSend(string team_a,string team_b,Event event);
    volatile bool& getIsConnected();
    void processSummary(vector<string> input);

};
