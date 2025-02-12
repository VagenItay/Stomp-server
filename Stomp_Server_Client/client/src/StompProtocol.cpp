#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include "../include/event.h"
#include "StompProtocol.h"
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>

StompProtocol::StompProtocol(bool connection, Game &game_) : isConnected(connection), game(game_), SubsCounter(0), receiptCounter(0),connectedUser("")
{
}
string StompProtocol::processReadFromSocket(vector<string> output, string answer)
{
    if (output[0] == "CONNECTED")
    {
        isConnected = true;
        return "login successful";
    }
    else if (output[0] == "MESSAGE")
    {
        string answer_keeper=answer;
        game.buildEventFromSend(answer_keeper);
        return answer;
    }
    else if (output[0] == "ERROR")
    {
        return answer;
    }
    else if (output[0] == "RECEIPT")
    {
        return processReceipt(output);
    }
}

string StompProtocol::processReceipt(vector<string> output)
{

    int receipt_id = stoi(output[1].substr(11));
    if (receiptIdToMsgHeader.at(receipt_id) == "SUBSCRIBE")
    {
        string topicName = receiptIdToTopicName.at(receipt_id);
        return "Joined channel " + topicName;
    }
    else if (receiptIdToMsgHeader.at(receipt_id) == "UNSUBSCRIBE")
    {
        string topicName = receiptIdToTopicName.at(receipt_id);
        return "Exited channel " + topicName;
    }
    else if (receiptIdToMsgHeader.at(receipt_id) == "DISCONNECT")
    {
        return "diss";
    }
}


string StompProtocol::processInputFromKeyboard(vector<string> input)
{
    if (input[0] == "login")
    {

        if (isConnected)
        {
            std::cout << "The client is already logged in, log out before trying again" << std::endl;
        }
        else
        {
            return generateFrameLogin(input);
        }
    }
    else if (isConnected && input[0] == "join")
    { // Subscribe to a topic
        return generateFrameJoin(input);
    }
    else if (isConnected && input[0] == "exit")
    {
        return generateFrameExit(input);
    }
    else if (isConnected && input[0] == "report")
    {
        return generateFrameReport(input);
    }
    else if (isConnected && input[0] == "logout")
    {
        return generateFrameLogout(input);
    }
    else
    {
        std::cout << "not legal commmand" << std::endl;
    }
}

void StompProtocol::processSummary(vector<string> input)
{
    if (isConnected)
        generateFrameSummary(input);
}
string StompProtocol::generateFrameLogin(vector<string> input)
{
    string st = "";
    st += "CONNECT\n";
    st += "accept-version:1.2 \n";
    st += "host:stomp.cs.bgu.ac.il \n";
    st += "login:" + input[2] + "\n";
    connectedUser = input[2];
    st += "passcode:" + input[3] + "\n\n";
    st += '\0';
    return st;
}

string StompProtocol::generateFrameJoin(vector<string> input)
{

    string st = "";
    st += "SUBSCRIBE\n";
    st += "destination:/" + input[1] + "\n";
    st += "id:" + std::to_string(SubsCounter); 
    st += "\n";
    st += "receipt:" + std::to_string(receiptCounter); 
    st += "\n\n";
    st += '\0';
    channelNameToSubID.insert({input[1], SubsCounter});
    receiptIdToMsgHeader.insert({receiptCounter, "SUBSCRIBE"});
    receiptIdToTopicName.insert({receiptCounter, input[1]});
    SubsCounter++;
    receiptCounter++;
    return st;
}

string StompProtocol::generateFrameExit(vector<string> input)
{
    string st = "";
    st += "UNSUBSCRIBE\n";
    st += "id:";
    st += std::to_string(channelNameToSubID.at(input[1])) + "\n";
    st += "receipt:" + std::to_string(receiptCounter); 
    st += "\n";
    st += "\n";
    st += '\0';
    receiptIdToMsgHeader.insert({receiptCounter, "UNSUBSCRIBE"});
    receiptIdToTopicName.insert({receiptCounter, input[1]});
    receiptCounter++;
    return st;
}
string StompProtocol::generateFrameReport(vector<string> input)
{
    names_and_events Names_And_Events = parseEventsFile(input[1]);
    vector<Event> events = Names_And_Events.events;
    string message = "";
    for (int i = 0; i < events.size(); i++)
    {
        message += generateFrameSend(Names_And_Events.team_a_name, Names_And_Events.team_b_name, events[i]);
        message += "splitHere\n";
    }
    message += '\0';
    return message;
}
string StompProtocol::generateFrameSend(string team_a, string team_b, Event event)
{
    // report -
    string st = "";
    st += "SEND\n";
    st += "destination:/" + team_a + "_" + team_b; 
    st += "\n";
    st += "\n";
    st += "user:" + connectedUser; 
    st += "\n";
    st += "team a:" + event.get_team_a_name() + "\n";
    st += "team b:" + event.get_team_b_name() + "\n";
    st += "event name:" + event.get_name() + "\n";      // event
    st += "time:" + to_string(event.get_time()) + "\n"; // event
    st += "general game updates:\n";                    // event
    for (const auto &kv : event.get_game_updates())
    {
        st += "  " + kv.first + ": " + kv.second + "\n";
    }
    st += "team a updates:\n"; // event
    for (const auto &kv : event.get_team_a_updates())
    {
        st += "  " + kv.first + ": " + kv.second + "\n";
    }
    st += "team b updates:\n"; // event
    for (const auto &kv : event.get_team_b_updates())
    {
        st += "  " + kv.first + ": " + kv.second + "\n";
    }
    st += "description:" + event.get_discription() + "\n\n"; // event
    return st;
}

void StompProtocol::generateFrameSummary(vector<string> input)
{
    Event summary = game.getMap().at(input[1]).at(input[2]);
    string st = summary.get_team_a_name() + " vs " + summary.get_team_b_name() + "\n";
    st += "Games stats:\n";
    st += "General stats:\n";
    for (const auto &kv : summary.get_game_updates())
    {
        st += "  " + kv.first + ": " + kv.second + "\n";
    }

    st += summary.get_team_a_name() + " stats:\n"; // event
    for (const auto &kv : summary.get_team_a_updates())
    {
        st += "  " + kv.first + ": " + kv.second + "\n";
    }

    st += summary.get_team_b_name() + " stats:\n"; // event
    for (const auto &kv : summary.get_team_b_updates())
    {
        st += "  " + kv.first + ": " + kv.second + "\n";
    }
    st += "Game event reports:\n" + summary.get_discription();
    ofstream temp_file;
    temp_file.open(input[3]);
    temp_file << st;
    temp_file.close();
}

string StompProtocol::generateFrameLogout(vector<string> input)
{
    string st = "DISCONNECT\n";
    st += "receipt:" + std::to_string(receiptCounter);
    st += "\n\n";
    st += '\0';
    receiptIdToMsgHeader.insert({receiptCounter, "DISCONNECT"});
    receiptCounter++;
    return st;
}

volatile bool &StompProtocol::getIsConnected()
{
    return isConnected;
}

